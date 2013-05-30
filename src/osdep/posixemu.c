/*
 * UAE - The Un*x Amiga Emulator
 *
 * Win32 interface
 *
 * Copyright 1997 Mathias Ortmann
 */

#include "sysconfig.h"
#include "sysdeps.h"

#include <xtl.h>
#include <sys/timeb.h>

#include "posixemu.h"
#include "filesys.h"

static DWORD lasterror;
/* Our Win32 implementation of this function */
void gettimeofday (struct timeval *tv, void *blah)
{
#if 1
	struct timeb time;

	ftime (&time);

	tv->tv_sec = time.time;
	tv->tv_usec = time.millitm * 1000;
#else
	SYSTEMTIME st;
	FILETIME ft;
	uae_u64 v, sec;
	GetSystemTime (&st);
	SystemTimeToFileTime (&st, &ft);
	v = (ft.dwHighDateTime << 32) | ft.dwLowDateTime;
	v /= 10;
	sec = v / 1000000;
	tv->tv_usec = (unsigned long)(v - (sec * 1000000));
	tv->tv_sec = (unsigned long)(sec - 11644463600);
#endif
}

/* convert time_t to/from AmigaDOS time */
#define secs_per_day (24 * 60 * 60)
#define diff ((8 * 365 + 2) * secs_per_day)

static void get_time (time_t t, long *days, long *mins, long *ticks)
{
	/* time_t is secs since 1-1-1970 */
	/* days since 1-1-1978 */
	/* mins since midnight */
	/* ticks past minute @ 50Hz */

	t -= diff;
	*days = t / secs_per_day;
	t -= *days * secs_per_day;
	*mins = t / 60;
	t -= *mins * 60;
	*ticks = t * 50;
}

static DWORD getattr (const TCHAR *name, LPFILETIME lpft, uae_s64 *size)
{
	HANDLE hFind;
	WIN32_FIND_DATA fd;

	if ((hFind = FindFirstFile (name, &fd)) == INVALID_HANDLE_VALUE) {
		fd.dwFileAttributes = GetFileAttributes (name);
		return fd.dwFileAttributes;
	}
	FindClose (hFind);

	if (lpft)
		*lpft = fd.ftLastWriteTime;
	if (size)
		*size = (((uae_u64)fd.nFileSizeHigh) << 32) | fd.nFileSizeLow;

	return fd.dwFileAttributes;
}
 

#ifndef HAVE_TRUNCATE
int truncate (const char *name, long int len)
{
    HANDLE hFile;
    BOOL bResult = FALSE;
    int result = -1;

    if ((hFile = CreateFile (name, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
			     OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL)) != INVALID_HANDLE_VALUE) {
	if (SetFilePointer (hFile, len, NULL, FILE_BEGIN) == (DWORD)len) {
	    if (SetEndOfFile (hFile) == TRUE)
		result = 0;
	} else {
	    write_log ("SetFilePointer() failure for %s to posn %d\n", name, len);
	}
	CloseHandle (hFile);
    } else {
	write_log ( "CreateFile() failed to open %s\n", name );
    }

    if (result == -1)
	lasterror = GetLastError ();
    return result;
}
#endif

int isspecialdrive (const char *name)
{
    
    return 0;
}

static int setfiletime (const TCHAR *name, unsigned int days, int minute, int tick, int tolocal)
{
	FILETIME LocalFileTime, FileTime;
	HANDLE hFile;

	if ((hFile = CreateFile (name, GENERIC_WRITE,FILE_SHARE_READ | FILE_SHARE_WRITE,NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_BACKUP_SEMANTICS, NULL)) == INVALID_HANDLE_VALUE)
		return 0;

	for (;;) {
		ULARGE_INTEGER lft;

		lft.QuadPart = (((uae_u64)(377*365+91+days)*(uae_u64)1440+(uae_u64)minute)*(uae_u64)(60*50)+(uae_u64)tick)*(uae_u64)200000;
		LocalFileTime.dwHighDateTime = lft.HighPart;
		LocalFileTime.dwLowDateTime = lft.LowPart;
		if (tolocal) {
			if (!LocalFileTimeToFileTime (&LocalFileTime, &FileTime))
				FileTime = LocalFileTime;
		} else {
			FileTime = LocalFileTime;
		}
		if (!SetFileTime (hFile, &FileTime, &FileTime, &FileTime)) {
			if (days > 47846) { // > 2108-12-31 (fat limit)
				days = 47846;
				continue;
			}
			if (days < 730) { // < 1980-01-01 (fat limit)
				days = 730;
				continue;
			}
		}
		break;
	}

	CloseHandle (hFile);

	return 1;
}

int posixemu_stat (const TCHAR *name, struct _stat64 *statbuf)
{
	DWORD attr;
	FILETIME ft, lft;

	if ((attr = getattr (name, &ft, &statbuf->st_size)) == (DWORD)~0) {
		return -1;
	} else {
		statbuf->st_mode = (attr & FILE_ATTRIBUTE_READONLY) ? FILEFLAG_READ : FILEFLAG_READ | FILEFLAG_WRITE;
		if (attr & FILE_ATTRIBUTE_ARCHIVE)
			statbuf->st_mode |= FILEFLAG_ARCHIVE;
		if (attr & FILE_ATTRIBUTE_DIRECTORY)
			statbuf->st_mode |= FILEFLAG_DIR;
		FileTimeToLocalFileTime (&ft,&lft);
		statbuf->st_mtime = (long)((*(__int64 *)&lft-((__int64)(369*365+89)*(__int64)(24*60*60)*(__int64)10000000))/(__int64)10000000);
	}
	return 0;
}

int posixemu_utime (const TCHAR *name, struct utimbuf *ttime)
{
	int result = -1, tolocal;
	long days, mins, ticks;
	time_t actime;

	if (!ttime) {
		actime = time (NULL);
		tolocal = 0;
	} else {
		tolocal = 1;
		actime = ttime->actime;
	}
	get_time (actime, &days, &mins, &ticks);

	if (setfiletime (name, days, mins, ticks, tolocal))
		result = 0;

	return result;
}

