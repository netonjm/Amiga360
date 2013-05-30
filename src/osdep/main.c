/*
 * UAE - The Un*x Amiga Emulator
 *
 * Start-up and support functions used by Win32
 */

#include "sysconfig.h"
#include "sysdeps.h"

#include <xtl.h>

#include "uae.h"
#include "options.h"
#include "debug.h"
#include "custom.h"
#include "events.h"

#include <SDL_main.h>

/*
 * Handle break signal
 */
#include <signal.h>

char* hdf_name[4];

#ifdef __cplusplus
static RETSIGTYPE sigbrkhandler(...)
#else
static RETSIGTYPE sigbrkhandler (int foo)
#endif
{
#ifdef DEBUGGER
    activate_debugger ();
#endif

#if !defined(__unix) || defined(__NeXT__)
    signal (SIGINT, sigbrkhandler);
#endif
}

void setup_brkhandler (void)
{

}

int qpcdivisor = 0;
HINSTANCE hInst;
char *start_path;
char start_path_data[MAX_DPATH];
 
static int isadminpriv (void)
{
   

    return TRUE;
}

 
static int osdetect (void)
{
 
    return 1;
}

extern int qpcdivisor;

frame_time_t read_processor_time_qpf (void)
{
	LARGE_INTEGER counter;
	frame_time_t t;
	QueryPerformanceCounter (&counter);
	if (qpcdivisor == 0)
		t = (frame_time_t)(counter.LowPart);
	else
		t = (frame_time_t)(counter.QuadPart >> qpcdivisor);
	if (!t)
		t++;
	return t;
}


void figure_processor_speed_qpf (void)
{	 
	uae_u64 qpfrate;
	 
	qpfrate = 49875000;
			   
 
	/* limit to 10MHz */
	qpcdivisor = 0;
	while (qpfrate > 10000000) {
		qpfrate >>= 1;
		qpcdivisor++;
	}
 
	syncbase = (unsigned long)qpfrate;

	rpt_available = 1;
}

extern FILE *debugfile;
extern struct uae_prefs currprefs, changed_prefs;

void unmount_all_hdfs(void) {
	int units = nr_units();
	while (units > 0) {
		units--;
		kill_filesys_unitconfig(&currprefs, units);
	}
}
static int hardfile_testrdb (char *filename)
{
	struct zfile *f = zfile_fopen (filename, "rb");
	uae_u8 tmp[8];
	int i;
	int result = 0;

	if (!f)
		return -1;
	for (i = 0; i < 16; i++) {
		zfile_fseek (f, i * 512, SEEK_SET);
		memset (tmp, 0, sizeof tmp);
		zfile_fread (tmp, 1, sizeof tmp, f);
		/*
		if (i == 0 && !memcmp (tmp + 2, "CIS", 3)) {
			hdf->controller = HD_CONTROLLER_PCMCIA_SRAM;
			break;
		}
		*/
		if (
			!memcmp (tmp, "RDSK\0\0\0", 7) || 
			!memcmp (tmp, "DRKS\0\0", 6) || 
			(tmp[0] == 0x53 && tmp[1] == 0x10 && tmp[2] == 0x9b && tmp[3] == 0x13 && tmp[4] == 0 && tmp[5] == 0)) 
		{
			// RDSK or ADIDE "encoded" RDSK
			result = 1;
			break;
		}
	}
	zfile_fclose (f);
	return result;
}


void mount_hdf(int index, char* name) {
	char * error = NULL;
	
	//check harddisk type
	int type = hardfile_testrdb (name);
	
	//invalid hardfile
	if (type < 0) {
		error = "invalid hardfile";
	} else {
		//RDB file system -> use autodetection
		int secspertrack = 0;
		int surfaces = 0;
		int reserved = 0;
		int blocksize = 512;
					
		//Old File System -> use default values
		if (type == 0) {
			secspertrack = 32;
			surfaces = 1;
			reserved = 2;
		} 		
		add_filesys_config (&currprefs, -1, NULL, NULL, name, 0, secspertrack, surfaces, reserved, blocksize, 0, 0, 0, 0);		 
	}
	 
}


void logging_init (void)
{
    static int first;
    char debugfilename[MAX_DPATH];

    if (first > 1) {
	write_log ("** RESTART **\n");
	return;
    }
    if (first == 1) {
	if (debugfile)
	    fclose (debugfile);
	debugfile = 0;
    }
#ifndef SINGLEFILE
#if 0
    if (currprefs.win32_logfile) {
	sprintf (debugfilename, "%swinuaelog.txt", start_path_data);
	if (!debugfile)
	    debugfile = fopen (debugfilename, "wt");
    } else if (!first) {
	sprintf (debugfilename, "%swinuaebootlog.txt", start_path_data);
	if (!debugfile)
	    debugfile = fopen (debugfilename, "wt");
    }
#endif
#endif
    first++;

//    write_log ("%s (%s %d.%d %s%s%s)", VersionStr, os_winnt ? "NT" : "W9X/ME",
//	osVersion.dwMajorVersion, osVersion.dwMinorVersion, osVersion.szCSDVersion,
//	strlen(osVersion.szCSDVersion) > 0 ? " " : "", os_winnt_admin ? "Admin" : "");
//    write_log (" %s %X.%X %d",
//	SystemInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_INTEL ? "32-bit x86" :
//	SystemInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64 ? "IA64" :
//	SystemInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 ? "AMD64" : "Unknown",
//	SystemInfo.wProcessorLevel, SystemInfo.wProcessorRevision,
//	SystemInfo.dwNumberOfProcessors);
    write_log ("\n(c) 1995-2001 Bernd Schmidt   - Core UAE concept and implementation."
	       "\n(c) 1998-2005 Toni Wilen      - Win32 port, core code updates."
	       "\n(c) 1996-2001 Brian King      - Win32 port, Picasso96 RTG, and GUI."
	       "\n(c) 1996-1999 Mathias Ortmann - Win32 port and bsdsocket support."
	       "\n(c) 2000-2001 Bernd Meyer     - JIT engine."
	       "\n(c) 2000-2005 Bernd Roesch    - MIDI input, many fixes."
	       "\nPress F12 to show the Settings Dialog (GUI), Alt-F4 to quit."
	       "\nEnd+F1 changes floppy 0, End+F2 changes floppy 1, etc."
	       "\n");
//    write_log ("EXE: '%s'\nDATA: '%s'\n", start_path_exe, start_path_data);
}

/* dummy to get this thing to build */
void filesys_init (void)
{
	
}

/*
 * Handle target-specific cfgfile options
 */
void target_save_options (FILE *f, const struct uae_prefs *p)
{
}

int target_parse_option (struct uae_prefs *p, const char *option, const char *value)
{
    return 0;
}

void target_default_options (struct uae_prefs *p, int type)
{
}

void target_startup_sequence (struct uae_prefs *p)
{ 
        //scan_roms (1);
}

