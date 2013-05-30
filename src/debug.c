/*
 * UAE - The Un*x Amiga Emulator
 *
 * Debugger
 *
 * (c) 1995 Bernd Schmidt
 * (c) 2006 Toni Wilen
 *
 */

#include "sysconfig.h"
#include "sysdeps.h"

#include <ctype.h>
#include <signal.h>

#include "options.h"
#include "uae.h"
#include "memory.h"
#include "custom.h"
#include "newcpu.h"
#include "cpu_prefetch.h"
#include "debug.h"
#include "cia.h"
#include "xwin.h"
#include "identify.h"
#include "audio.h"
#include "disk.h"
#include "savestate.h"
#include "autoconf.h"
#include "filesys.h"
#include "akiko.h"
#include "inputdevice.h"
#include "crc32.h"
#include "cpummu.h"
#include "rommgr.h"
#include "inputrecord.h"

 
void deactivate_debugger (void)
{
	 
}
 

void debug_help (void)
{
	 
}

 
#define MAX_LINECOUNTER 1000

static int debug_out (const TCHAR *format, ...)
{
 
	return 1;
}

static void ignore_ws (TCHAR **c)
{
	 
}

 
static int readregx (TCHAR **c, uae_u32 *valp)
{
	 
	return 1;
}

static uae_u32 readbinx (TCHAR **c)
{
	uae_u32 val = 0;

	 
	return val;
}

static uae_u32 readhexx (TCHAR **c)
{
	uae_u32 val = 0;
	 
	return val;
}

static uae_u32 readintx (TCHAR **c)
{
	uae_u32 val = 0;
	 
	return val ;
}


static int checkvaltype (TCHAR **c, uae_u32 *val)
{
	 
	return 0;
}

static int readsize (int val, TCHAR **c)
{
	 
	return 1;
}

static uae_u32 readint (TCHAR **c)
{
 
	return 0;
}
static uae_u32 readhex (TCHAR **c)
{
	return 0;
}
static uae_u32 readint_s (TCHAR **c, int *size)
{
	return 0;
}
static uae_u32 readhex_s (TCHAR **c, int *size)
{
	return 0;
}
static uae_u32 readbin (TCHAR **c)
{
	return 0;
}

static TCHAR next_char (TCHAR **c)
{
	return NULL;
}

static TCHAR peek_next_char (TCHAR **c)
{
	return NULL;
}

static int more_params (TCHAR **c)
{
	return 0;
}

static int next_string (TCHAR **c, TCHAR *out, int max, int forceupper)
{
 
	return 0;
}

static void converter (TCHAR **c)
{
	 
}

int notinrom (void)
{
	 
	return 0;
}

static uae_u32 lastaddr (void)
{
	 
	return 0;
}

static uaecptr nextaddr2 (uaecptr addr, int *next)
{
	 
	return 0;
}

static uaecptr nextaddr (uaecptr addr, uaecptr last, uaecptr *end)
{
	 

	return 0;
}

int safe_addr(uaecptr addr, int size)
{
	 
	return 0;
}

uaecptr dumpmem2 (uaecptr addr, TCHAR *out, int osize)
{
 
	return 0;
}

static void dumpmem (uaecptr addr, uaecptr *nxmem, int lines)
{
	 
}

static void dump_custom_regs (int aga)
{
 
}

static void dump_vectors (uaecptr addr)
{
	 
}

static void disassemble_wait (FILE *file, unsigned long insn)
{
 
}
 

void record_dma_reset (void)
{
	 
}

void record_copper_reset (void)
{
	/* Start a new set of copper records.  */
	 
}

STATIC_INLINE uae_u32 ledcolor (uae_u32 c, uae_u32 *rc, uae_u32 *gc, uae_u32 *bc, uae_u32 *a)
{
	 
	return 0;
}

STATIC_INLINE void putpixel (uae_u8 *buf, int bpp, int x, xcolnr c8)
{
	 
}
 
void debug_draw_cycles (uae_u8 *buf, int bpp, int line, int width, int height, uae_u32 *xredcolors, uae_u32 *xgreencolors, uae_u32 *xbluescolors)
{
	 
}




void record_dma_event (int evt, int hpos, int vpos)
{
	 
}

struct dma_rec *record_dma (uae_u16 reg, uae_u16 dat, uae_u32 addr, int hpos, int vpos, int type)
{
	 
	return NULL;
}

static void decode_dma_record (int hpos, int vpos, int toggle, bool logfile)
{
	 
}
void log_dma_record (void)
{
	 
}

void record_copper (uaecptr addr, int hpos, int vpos)
{
	 
}

static struct cop_rec *find_copper_records (uaecptr addr)
{
	 
	return 0;
}

/* simple decode copper by Mark Cox */
static void decode_copper_insn (FILE* file, unsigned long insn, unsigned long addr)
{
	 

}

static uaecptr decode_copperlist (FILE* file, uaecptr address, int nolines)
{
	 return 0;
}

static int copper_debugger (TCHAR **c)
{
	 
	return 0;
}
 
static void clearcheater(void)
{
 
}
static int addcheater(uaecptr addr, int size)
{
 
	return 1;
}
static void listcheater(int mode, int size)
{
	 
}

static void deepcheatsearch (TCHAR **c)
{
 
}

/* cheat-search by Toni Wilen (originally by Holger Jakob) */
static void cheatsearch (TCHAR **c)
{
	 
}

 

static void illg_free (void)
{
	 
}

static void illg_init (void)
{
    
}

/* add special custom register check here */
static void illg_debug_check (uaecptr addr, int rwi, int size, uae_u32 val)
{
	return;
}

static void illg_debug_do (uaecptr addr, int rwi, int size, uae_u32 val)
{
	 
}

static int debug_mem_off (uaecptr addr)
{
	return 0;
}

 

static void smc_free (void)
{
 
}

static void smc_detect_init (TCHAR **c)
{
	 
}
 
static void smc_detector (uaecptr addr, int rwi, int size, uae_u32 *valp)
{
	 
}

uae_u8 *save_debug_memwatch (int *len, uae_u8 *dstptr)
{
	 
	return 0;
}

uae_u8 *restore_debug_memwatch (uae_u8 *src)
{
	 
	return 0;
}

void restore_debug_memwatch_finish (void)
{
	 
}

static int memwatch_func (uaecptr addr, int rwi, int size, uae_u32 *valp)
{
 
	return 1;
}

static int mmu_hit (uaecptr addr, int size, int rwi, uae_u32 *v);

static uae_u32 REGPARAM2 mmu_lget (uaecptr addr)
{
	 
	return 0;
}
static uae_u32 REGPARAM2 mmu_wget (uaecptr addr)
{
	return 0;
}
static uae_u32 REGPARAM2 mmu_bget (uaecptr addr)
{
	return 0;
}
static void REGPARAM2 mmu_lput (uaecptr addr, uae_u32 v)
{
 
}
static void REGPARAM2 mmu_wput (uaecptr addr, uae_u32 v)
{
 
}
static void REGPARAM2 mmu_bput (uaecptr addr, uae_u32 v)
{
 
}

static uae_u32 REGPARAM2 debug_lget (uaecptr addr)
{
	return 0;
}
static uae_u32 REGPARAM2 mmu_lgeti (uaecptr addr)
{
	return 0;
}
static uae_u32 REGPARAM2 mmu_wgeti (uaecptr addr)
{
	return 0;
}

static uae_u32 REGPARAM2 debug_wget (uaecptr addr)
{
	return 0;
}
static uae_u32 REGPARAM2 debug_bget (uaecptr addr)
{
	return 0;
}
static uae_u32 REGPARAM2 debug_lgeti (uaecptr addr)
{
	return 0;
}
static uae_u32 REGPARAM2 debug_wgeti (uaecptr addr)
{
	return 0;
}
static void REGPARAM2 debug_lput (uaecptr addr, uae_u32 v)
{
 
}
static void REGPARAM2 debug_wput (uaecptr addr, uae_u32 v)
{
 
}
static void REGPARAM2 debug_bput (uaecptr addr, uae_u32 v)
{
 
}
static int REGPARAM2 debug_check (uaecptr addr, uae_u32 size)
{
	return 0;
}
static uae_u8 *REGPARAM2 debug_xlate (uaecptr addr)
{
	return 0;
}

uae_u16 debug_wputpeekdma (uaecptr addr, uae_u32 v)
{
	return 0;
}
uae_u16 debug_wgetpeekdma (uaecptr addr, uae_u32 v)
{
	return 0;
}

void debug_putlpeek (uaecptr addr, uae_u32 v)
{
 
}
void debug_wputpeek (uaecptr addr, uae_u32 v)
{
 
}
void debug_bputpeek (uaecptr addr, uae_u32 v)
{
 
}
void debug_bgetpeek (uaecptr addr, uae_u32 v)
{
 
}
void debug_wgetpeek (uaecptr addr, uae_u32 v)
{
 
}
void debug_lgetpeek (uaecptr addr, uae_u32 v)
{
 
}
 
 
static int deinitialize_memwatch (void)
{
	return 0;
}

static void initialize_memwatch (int mode)
{
     
}

int debug_bankchange (int mode)
{
	return 0;
}

static TCHAR *getsizechar (int size)
{
	 
	return "";
}

void memwatch_dump2 (TCHAR *buf, int bufsize, int num)
{
	 
}

static void memwatch_dump (int num)
{
	 
}

static void memwatch (TCHAR **c)
{
	 
}

static void writeintomem (TCHAR **c)
{
	 
}

static uae_u8 *dump_xlate (uae_u32 addr)
{
	return 0;
}

static void memory_map_dump_2 (int log)
{
	 
}
void memory_map_dump (void)
{
	 
}

STATIC_INLINE uaecptr BPTR2APTR (uaecptr addr)
{
	return 0;
}
/*static TCHAR *BSTR2CSTR (uae_u8 *bstr)
{
	TCHAR *s;
	char *cstr = xmalloc (char, bstr[0] + 1);
	if (cstr) {
		memcpy (cstr, bstr + 1, bstr[0]);
		cstr[bstr[0]] = 0;
	}
	s = au (cstr);
	xfree (cstr);
	return s;
}*/

static void print_task_info (uaecptr node)
{
	 
}

static void show_exec_tasks (void)
{
	 
}

static uaecptr get_base (const uae_char *name)
{
	return 0;
}

static TCHAR *getfrombstr(uaecptr pp)
{
	return "";
}

static void show_exec_lists (TCHAR t)
{
	 
}

#if 0
static int trace_same_insn_count;
static uae_u8 trace_insn_copy[10];
static struct regstruct trace_prev_regs;
#endif
 

int instruction_breakpoint (TCHAR **c)
{
	 
	return 1;
}

static int process_breakpoint(TCHAR **c)
{
	 
	return 1;
}

static void savemem (TCHAR **cc)
{
	 
}

static void searchmem (TCHAR **cc)
{
	 
}

static int staterecorder (TCHAR **cc)
{
#if 0
	TCHAR nc;

	if (!more_params (cc)) {
		if (savestate_dorewind (1)) {
			debug_rewind = 1;
			return 1;
		}
		return 0;
	}
	nc = next_char (cc);
	if (nc == 'l') {
		savestate_listrewind ();
		return 0;
	}
#endif
	return 0;
}

 
void debugtest (enum debugtest_item di, const TCHAR *format, ...)
{
	 
}

static void debugtest_set (TCHAR **inptr)
{
	 
}

static void debug_sprite (TCHAR **inptr)
{
	 

}

static void disk_debug (TCHAR **inptr)
{
	 
}

static void find_ea (TCHAR **inptr)
{
 
}

static void m68k_modify (TCHAR **inptr)
{
	 
}

 
static bool debug_line (TCHAR *input)
{
	 
	return false;
}

static void debug_1 (void)
{
	 
}

static void addhistory (void)
{
	 
}

void debug (void)
{
	 
}

const TCHAR *debuginfo (int mode)
{
	return "";
}

 
void mmu_do_hit (void)
{
	 
}

static void mmu_do_hit_pre (struct mmudata *md, uaecptr addr, int size, int rwi, uae_u32 v)
{
 
}

static int mmu_hit (uaecptr addr, int size, int rwi, uae_u32 *v)
{
 
	return 0;
}

static void mmu_free_node(struct mmunode *mn)
{
 
}

static void mmu_free(void)
{
 
}

static int getmmubank(struct mmudata *snptr, uaecptr p)
{
 
	return 0;
}

int mmu_init(int mode, uaecptr parm, uaecptr parm2)
{
	 
	return 1;
}

void debug_parser (const TCHAR *cmd, TCHAR *out, uae_u32 outsize)
{
	 
}
