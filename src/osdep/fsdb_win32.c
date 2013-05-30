 /*
  * UAE - The Un*x Amiga Emulator
  *
  * Library of functions to make emulated filesystem as independent as
  * possible of the host filesystem's capabilities.
  * This is the Win32 version.
  *
  * Copyright 1997 Mathias Ortmann
  * Copyright 1999 Bernd Schmidt
  */

#ifdef FILESYS

#include "sysconfig.h"
#include "sysdeps.h"

#include "fsdb.h"
#include <xtl.h>

struct my_openfile_s {
	HANDLE h;
};

struct my_opendir_s {
	HANDLE h;
	WIN32_FIND_DATA fd;
	int first;
};

/* these are deadly (but I think allowed on the Amiga): */
#define NUM_EVILCHARS 7
static const char evilchars[NUM_EVILCHARS] = { '\\', '*', '?', '\"', '<', '>', '|' };

/* Return nonzero for any name we can't create on the native filesystem.  */
int fsdb_name_invalid (const char *n)
{
    int i;
    char a = n[0];
    char b = (a == '\0' ? a : n[1]);
    char c = (b == '\0' ? b : n[2]);
    char d = (c == '\0' ? c : n[3]);
    int l = strlen (n), ll;

    if (a >= 'a' && a <= 'z')
	a -= 32;
    if (b >= 'a' && b <= 'z')
	b -= 32;
    if (c >= 'a' && c <= 'z')
	c -= 32;

    /* reserved dos devices */
    ll = 0;
    if (a == 'A' && b == 'U' && c == 'X') ll = 3; /* AUX  */
    if (a == 'C' && b == 'O' && c == 'N') ll = 3; /* CON  */
    if (a == 'P' && b == 'R' && c == 'N') ll = 3; /* PRN  */
    if (a == 'N' && b == 'U' && c == 'L') ll = 3; /* NUL  */
    if (a == 'L' && b == 'P' && c == 'T'  && (d >= '0' && d <= '9')) ll = 4;  /* LPT# */
    if (a == 'C' && b == 'O' && c == 'M'  && (d >= '0' && d <= '9')) ll = 4; /* COM# */
    /* AUX.anything, CON.anything etc.. are also illegal names */
    if (ll && (l == ll || (l > ll && n[ll] == '.')))
	return 1;

    /* spaces and periods at the end are a no-no */
    i = l - 1;
    if (n[i] == '.' || n[i] == ' ')
	return 1;

    /* these characters are *never* allowed */
    for (i = 0; i < NUM_EVILCHARS; i++) {
	if (strchr (n, evilchars[i]) != 0)
	    return 1;
    }

    /* the reserved fsdb filename */
    if (strcmp (n, FSDB_FILE) == 0)
	return 1;
    return 0; /* the filename passed all checks, now it should be ok */
}

static uae_u32 filesys_parse_mask (uae_u32 mask)
{
    return mask ^ 0xf;
}

int fsdb_exists (char *nname)
{
    if (GetFileAttributes(nname) == 0xFFFFFFFF)
	return 0;
    return 1;
}


int my_rename (const TCHAR *oldname, const TCHAR *newname)
{
	return MoveFile (oldname, newname) == 0 ? -1 : 0;
}


int my_rmdir (const TCHAR *name)
{
	return rmdir (name);
}


int my_readdir (struct my_opendir_s *mod, TCHAR *name)
{
	if (mod->first) {
		_tcscpy (name, mod->fd.cFileName);
		mod->first = 0;
		return 1;
	}
	if (!FindNextFile (mod->h, &mod->fd))
		return 0;
	_tcscpy (name, mod->fd.cFileName);
	return 1;
}

void my_close (struct my_openfile_s *mos)
{
	CloseHandle (mos->h);
	xfree (mos);
}

int my_unlink (const TCHAR *name)
{
	return 1;
}

int my_truncate (const TCHAR *name, uae_u64 len)
{
	HANDLE hFile;
	BOOL bResult = FALSE;
	int result = -1;

	if ((hFile = CreateFile (name, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL ) ) != INVALID_HANDLE_VALUE )
	{
		LARGE_INTEGER li;
		li.QuadPart = len;
		li.LowPart = SetFilePointer (hFile, li.LowPart, &li.HighPart, FILE_BEGIN);
		if (li.LowPart == INVALID_SET_FILE_POINTER && GetLastError () != NO_ERROR) {
			write_log (L"truncate: SetFilePointer() failure for %s to posn %d\n", name, len);
		} else {
			if (SetEndOfFile (hFile) == TRUE)
				result = 0;
		}
		CloseHandle (hFile);
	} else {
		write_log (L"truncate: CreateFile() failed to open %s\n", name);
	}
	return result;
}

struct my_openfile_s *my_open (const TCHAR *name, int flags)
{
	struct my_openfile_s *mos;
	HANDLE h;
	DWORD DesiredAccess = GENERIC_READ;
	DWORD ShareMode = FILE_SHARE_READ | FILE_SHARE_WRITE;
	DWORD CreationDisposition = OPEN_EXISTING;
	DWORD FlagsAndAttributes = FILE_ATTRIBUTE_NORMAL;
	DWORD attr;

	mos = xmalloc (struct my_openfile_s, 1);
	if (!mos)
		return NULL;
	attr = GetFileAttributes (name);
	if (flags & O_TRUNC)
		CreationDisposition = CREATE_ALWAYS;
	else if (flags & O_CREAT)
		CreationDisposition = OPEN_ALWAYS;
	if (flags & O_WRONLY)
		DesiredAccess = GENERIC_WRITE;
	if (flags & O_RDONLY) {
		DesiredAccess = GENERIC_READ;
		CreationDisposition = OPEN_EXISTING;
	}
	if (flags & O_RDWR)
		DesiredAccess = GENERIC_READ | GENERIC_WRITE;
	if (CreationDisposition == CREATE_ALWAYS && attr != -1 &&
		(attr & (FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_HIDDEN)))
		SetFileAttributes (name, FILE_ATTRIBUTE_NORMAL);
	h = CreateFile (name, DesiredAccess, ShareMode, NULL, CreationDisposition, FlagsAndAttributes, NULL);
	if (h == INVALID_HANDLE_VALUE) {
		DWORD err = GetLastError();
		if (err == ERROR_ACCESS_DENIED && (DesiredAccess & GENERIC_WRITE)) {
			DesiredAccess &= ~GENERIC_WRITE;
			h = CreateFile (name, DesiredAccess, ShareMode, NULL, CreationDisposition, FlagsAndAttributes, NULL);
			if (h == INVALID_HANDLE_VALUE)
				err = GetLastError();
		}
		if (h == INVALID_HANDLE_VALUE) {
			write_log (L"failed to open '%s' %x %x err=%d\n", name, DesiredAccess, CreationDisposition, err);
			xfree (mos);
			mos = NULL;
			goto err;
		}
	}
	mos->h = h;
err:
	//write_log (L"open '%s' = %x\n", name, mos ? mos->h : 0);
	return mos;
}

unsigned int my_read (struct my_openfile_s *mos, void *b, unsigned int size)
{
	DWORD read = 0;
	ReadFile (mos->h, b, size, &read, NULL);
	return read;
}

unsigned int my_write (struct my_openfile_s *mos, void *b, unsigned int size)
{
	DWORD written = 0;
	WriteFile (mos->h, b, size, &written, NULL);
	return written;
}

void my_closedir (struct my_opendir_s *mod)
{
	if (mod)
		FindClose (mod->h);
	xfree (mod);
}

uae_s64 int my_lseek (struct my_openfile_s *mos, uae_s64 int offset, int whence)
{
	LARGE_INTEGER li;

	li.QuadPart = offset;
	li.LowPart = SetFilePointer (mos->h, li.LowPart, &li.HighPart,
		whence == SEEK_SET ? FILE_BEGIN : (whence == SEEK_END ? FILE_END : FILE_CURRENT));
	if (li.LowPart == INVALID_SET_FILE_POINTER && GetLastError () != NO_ERROR)
		li.QuadPart = -1;
	return li.QuadPart;
}

/* For an a_inode we have newly created based on a filename we found on the
 * native fs, fill in information about this file/directory.  */
int fsdb_fill_file_attrs (a_inode *base, a_inode *aino)
{
    DWORD mode;

    if ((mode = GetFileAttributes (aino->nname)) == 0xFFFFFFFF) {
	write_log ("GetFileAttributes('%s') failed! error=%d, aino=%p dir=%d\n", aino->nname,GetLastError(),aino,aino->dir);
	return 0;
    }

    aino->dir = (mode & FILE_ATTRIBUTE_DIRECTORY) ? 1 : 0;
    aino->amigaos_mode = A_FIBF_EXECUTE | A_FIBF_READ;
    if (FILE_ATTRIBUTE_ARCHIVE & mode)
	aino->amigaos_mode |= A_FIBF_ARCHIVE;
    if (! (FILE_ATTRIBUTE_READONLY & mode))
	aino->amigaos_mode |= A_FIBF_WRITE | A_FIBF_DELETE;
    aino->amigaos_mode = filesys_parse_mask (aino->amigaos_mode);
    return 1;
}

 
int fsdb_set_file_attrs (a_inode *aino)
{
	uae_u32 tmpmask;
	
	uae_u32 mode;

	tmpmask = filesys_parse_mask (aino->amigaos_mode);

	mode = GetFileAttributes (aino->nname);
	if (mode == -1)
		return ERROR_OBJECT_NOT_AROUND;
	mode &= FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_ARCHIVE | FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_HIDDEN;

	mode = 0;

    /* Unix dirs behave differently than AmigaOS ones.  */
    /* windows dirs go where no dir has gone before...  */
    if (! aino->dir) {
	if ((tmpmask & (A_FIBF_READ | A_FIBF_DELETE)) == 0)
	    mode |= FILE_ATTRIBUTE_READONLY;
	if (tmpmask & A_FIBF_ARCHIVE)
	    mode |= FILE_ATTRIBUTE_ARCHIVE;
	else
	    mode &= ~FILE_ATTRIBUTE_ARCHIVE;

	SetFileAttributes(aino->nname, mode);
    }

    aino->dirty = 1;
    return 0;
}

/* return supported combination */
int fsdb_mode_supported (const a_inode *aino)
{
	int mask = aino->amigaos_mode;
	if (0 && aino->dir)
		return 0;
	if (fsdb_mode_representable_p (aino, mask))
		return mask;
	mask &= ~(A_FIBF_SCRIPT | A_FIBF_READ | A_FIBF_EXECUTE);
	if (fsdb_mode_representable_p (aino, mask))
		return mask;
	mask &= ~A_FIBF_WRITE;
	if (fsdb_mode_representable_p (aino, mask))
		return mask;
	mask &= ~A_FIBF_DELETE;
	if (fsdb_mode_representable_p (aino, mask))
		return mask;
	return 0;
}

int fsdb_mode_representable_p (const a_inode *aino, int amigaos_mode)
{
	int mask = amigaos_mode ^ 15;

	if (0 && aino->dir)
		return amigaos_mode == 0;

	if (mask & A_FIBF_SCRIPT) /* script */
		return 0;
	if ((mask & 15) == 15) /* xxxxRWED == OK */
		return 1;
	if (!(mask & A_FIBF_EXECUTE)) /* not executable */
		return 0;
	if (!(mask & A_FIBF_READ)) /* not readable */
		return 0;
	if ((mask & 15) == (A_FIBF_READ | A_FIBF_EXECUTE)) /* ----RxEx == ReadOnly */
		return 1;
	return 0;
}

char *fsdb_create_unique_nname (a_inode *base, const char *suggestion)
{
    char *c;
    char tmp[256] = "__uae___";
    int i;

    strncat (tmp, suggestion, 240);

    /* replace the evil ones... */
    for (i=0; i < NUM_EVILCHARS; i++)
	while ((c = strchr (tmp, evilchars[i])) != 0)
	    *c = '_';

    while ((c = strchr (tmp, '.')) != 0)
	*c = '_';
    while ((c = strchr (tmp, ' ')) != 0)
	*c = '_';

    for (;;) {
	char *p = build_nname (base->nname, tmp);
	if (access (p, R_OK) < 0 && errno == ENOENT) {
	    write_log ("unique name: %s\n", p);
	    return p;
	}
	free (p);

	/* tmpnam isn't reentrant and I don't really want to hack configure
	 * right now to see whether tmpnam_r is available...  */
	for (i = 0; i < 8; i++) {
	    tmp[i+8] = "_abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"[rand () % 63];
	}
    }
}

struct my_opendir_s *my_opendir (const TCHAR *name)
{
	struct my_opendir_s *mod;
	TCHAR tmp[MAX_DPATH];

	_tcscpy (tmp, name);
	_tcscat (tmp, "\\");
	_tcscat (tmp, "*.*");
	mod = xmalloc (struct my_opendir_s, 1);
	if (!mod)
		return NULL;
	mod->h = FindFirstFile(tmp, &mod->fd);
	if (mod->h == INVALID_HANDLE_VALUE) {
		xfree (mod);
		return NULL;
	}
	mod->first = 1;
	return mod;
}
 
int dos_errno (void)
{
    DWORD e = GetLastError ();

    //write_log ("ec=%d\n", e);
    switch (e) {
     case ERROR_NOT_ENOUGH_MEMORY:
     case ERROR_OUTOFMEMORY:
	return ERROR_NO_FREE_STORE;

     case ERROR_FILE_EXISTS:
     case ERROR_ALREADY_EXISTS:
	return ERROR_OBJECT_EXISTS;

     case ERROR_WRITE_PROTECT:
     case ERROR_ACCESS_DENIED:
	return ERROR_WRITE_PROTECTED;

     case ERROR_FILE_NOT_FOUND:
     case ERROR_INVALID_DRIVE:
	return ERROR_OBJECT_NOT_AROUND;

     case ERROR_HANDLE_DISK_FULL:
	return ERROR_DISK_IS_FULL;

     case ERROR_SHARING_VIOLATION:
     case ERROR_BUSY:
	return ERROR_OBJECT_IN_USE;

     case ERROR_CURRENT_DIRECTORY:
	return ERROR_DIRECTORY_NOT_EMPTY;

     case ERROR_NEGATIVE_SEEK:
     case ERROR_SEEK_ON_DEVICE:
	return ERROR_SEEK_ERROR;

     default:
	write_log ("Unimplemented error %d\n", e);
	return ERROR_NOT_IMPLEMENTED;
    }
}
#endif
