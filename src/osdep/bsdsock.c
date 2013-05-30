/*
 * UAE - The Un*x Amiga Emulator
 *
 * bsdsocket.library emulation - Win32 OS-dependent part
 *
 * Copyright 1997,98 Mathias Ortmann
 * Copyright 1999,2000 Brian King
 *
 * GNU Public License
 *
 */
#ifdef BSDSOCKET
#include <xtl.h>
#include <winsockx.h>
#include "sysconfig.h"
#include "sysdeps.h"


#include <stddef.h>
#include <process.h>

#include "options.h"
#include "include/memory.h"
#include "custom.h"
#include "events.h"
#include "newcpu.h"
#include "autoconf.h"
#include "traps.h"
#include "bsdsocket.h"


//#include "osdep/exectasks.h"
#include "threaddep/thread.h"
#include "native2amiga.h"



static HWND hSockWnd;
static long FAR PASCAL SocketWindowProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );

extern HWND hAmigaWnd;
int hWndSelector = 0; /* Set this to zero to get hSockWnd */
CRITICAL_SECTION csSigQueueLock;

DWORD threadid;
#ifdef __GNUC__
#define THREAD(func,arg) CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)func,(LPVOID)arg,0,&threadid)
#else
#define THREAD(func,arg) _beginthreadex( NULL, 0, func, (void *)arg, 0, (unsigned *)&threadid )
#endif

#define SETERRNO seterrno(sb,WSAGetLastError()-WSABASEERR)
#define SETHERRNO setherrno(sb,WSAGetLastError()-WSABASEERR)
#define WAITSIGNAL waitsig(sb)

#define SETSIGNAL addtosigqueue(sb,0)
#define CANCELSIGNAL cancelsig(sb)

#define FIOSETOWN _IOW('f', 124, long)    /* set owner (struct Task *) */
#define FIOGETOWN _IOR('f', 123, long)    /* get owner (struct Task *) */

#define BEGINBLOCKING if (sb->ftable[sd-1] & SF_BLOCKING) sb->ftable[sd-1] |= SF_BLOCKINGINPROGRESS
#define ENDBLOCKING sb->ftable[sd-1] &= ~SF_BLOCKINGINPROGRESS

static WSADATA wsbData;

int PASCAL WSAEventSelect(SOCKET,HANDLE,long);

#define MAX_SELECT_THREADS 64
static HANDLE hThreads[MAX_SELECT_THREADS];
uae_u32 *threadargs[MAX_SELECT_THREADS];
static HANDLE hEvents[MAX_SELECT_THREADS];

#define MAX_GET_THREADS 64
static HANDLE hGetThreads[MAX_GET_THREADS];
uae_u32 *threadGetargs[MAX_GET_THREADS];
static HANDLE hGetEvents[MAX_GET_THREADS];


static HANDLE hSockThread;
static HANDLE hSockReq, hSockReqHandled;
static unsigned int __stdcall sock_thread(void *);

CRITICAL_SECTION SockThreadCS;
#define PREPARE_THREAD EnterCriticalSection( &SockThreadCS )
#define TRIGGER_THREAD { SetEvent( hSockReq ); WaitForSingleObject( hSockReqHandled, INFINITE ); LeaveCriticalSection( &SockThreadCS ); }

#define SOCKVER_MAJOR 2
#define SOCKVER_MINOR 2

#define SF_RAW_UDP 0x10000000
#define SF_RAW_RAW 0x20000000
#define SF_RAW_RUDP 0x08000000
#define SF_RAW_RICMP 0x04000000

typedef struct hostent {
  char FAR      *h_name;
  char FAR  FAR **h_aliases;
  short         h_addrtype;
  short         h_length;
  char FAR  FAR **h_addr_list;
} HOSTENT, *PHOSTENT, FAR *LPHOSTENT;


__forceinline struct hostent *BuildHostEnt(char *name, char *hname, char *alias, char *addr_list)
{
	struct hostent *host1;
	host1 = (struct hostent *) malloc (sizeof(struct hostent));
	host1->h_name = hname;
	host1->h_addrtype = AF_INET;
	host1->h_aliases = (char **)malloc(sizeof (strlen(alias) + 1));
	host1->h_aliases[0] = alias;
	host1->h_aliases[1] = NULL;
	host1->h_length = 4;
	host1->h_addr_list = (char **)malloc(sizeof (strlen(addr_list) + 1));
	host1->h_addr_list[0] = addr_list;
	host1->h_addr_list[1] = NULL;
	return host1;
}


typedef struct ip_option_information {
    u_char Ttl;		/* Time To Live (used for traceroute) */
    u_char Tos; 	/* Type Of Service (usually 0) */
    u_char Flags; 	/* IP header flags (usually 0) */
    u_char OptionsSize; /* Size of options data (usually 0, max 40) */
    u_char FAR *OptionsData;   /* Options data buffer */
} IPINFO, *PIPINFO, FAR *LPIPINFO;

static void bsdsetpriority (HANDLE thread)
{
//    int pri = os_winnt ? THREAD_PRIORITY_NORMAL : priorities[currprefs.win32_active_priority].value;
    int pri = THREAD_PRIORITY_NORMAL;
    SetThreadPriority(thread, pri);
}

static int mySockStartup( void )
{
   
	int result = 0;
    SOCKET dummy;
    DWORD lasterror;

    if (WSAStartup(MAKEWORD( SOCKVER_MAJOR, SOCKVER_MINOR ), &wsbData))
    {
	    lasterror = WSAGetLastError();

	    if( lasterror == WSAVERNOTSUPPORTED )
	    {

	    }
	    else
		write_log ( "BSDSOCK: ERROR - Unable to initialize Windows socket layer! Error code: %d\n", lasterror );
	    return 0;
    }

    if (LOBYTE (wsbData.wVersion) != SOCKVER_MAJOR || HIBYTE (wsbData.wVersion) != SOCKVER_MINOR )
    {


	return 0;
    }
    else
    {
	write_log ( "BSDSOCK: using %s\n", wsbData.szDescription );
	// make sure WSP/NSPStartup gets called from within the regular stack
	// (Windows 95/98 need this)
	if( ( dummy = socket( AF_INET,SOCK_STREAM,IPPROTO_TCP ) ) != INVALID_SOCKET )
	{
	    closesocket( dummy );
	    result = 1;
	}
	else
	{
	    write_log ( "BSDSOCK: ERROR - WSPStartup/NSPStartup failed! Error code: %d\n",WSAGetLastError() );
	    result = 0;
	}
    }

    return result;
    
}

static int socket_layer_initialized = 0;

int init_socket_layer(void)
{
    int result = 0;
 
    if( currprefs.socket_emu )
    {
	if( ( result = mySockStartup() ) )
	{
		InitializeCriticalSection(&csSigQueueLock);

	    if( hSockThread == NULL )
	    {
	 
		InitializeCriticalSection( &SockThreadCS );
		hSockReq = CreateEvent( NULL, FALSE, FALSE, NULL );
		hSockReqHandled = CreateEvent( NULL, FALSE, FALSE, NULL );

		 
		hSockThread = (void *)THREAD(sock_thread,NULL);
			 
	    }
	}
    }

    socket_layer_initialized = result;

    return result;
}

void deinit_socket_layer(void)
{
	int i;
    if( currprefs.socket_emu )
    {
	WSACleanup();
	if( socket_layer_initialized )
	{
	    DeleteCriticalSection( &csSigQueueLock );
	    if( hSockThread )
	    {
		DeleteCriticalSection( &SockThreadCS );
		CloseHandle( hSockReq );
		hSockReq = NULL;
		CloseHandle( hSockReqHandled );
		WaitForSingleObject( hSockThread, INFINITE );
		CloseHandle( hSockThread );
	    }
	    for (i = 0; i < MAX_SELECT_THREADS; i++)
	    {
		if (hThreads[i])
		{
		    CloseHandle( hThreads[i] );
		}
	    }
	}
    }     
}

#ifdef BSDSOCKET

void locksigqueue(void)
{
    EnterCriticalSection(&csSigQueueLock);
}

void unlocksigqueue(void)
{
    LeaveCriticalSection(&csSigQueueLock);
}

// Asynchronous completion notification

// We use window messages posted to hAmigaWnd in the range from 0xb000 to 0xb000+MAXPENDINGASYNC*2
// Socket events cause even-numbered messages, task events odd-numbered messages
// Message IDs are allocated on a round-robin basis and deallocated by the main thread.

// WinSock tends to choke on WSAAsyncCancelMessage(s,w,m,0,0) called too often with an event pending

// @@@ Enabling all socket event messages for every socket by default and basing things on that would
// be cleaner (and allow us to write a select() emulation that doesn't need to be kludge-aborted).
// However, the latency of the message queue is too high for that at the moment (setting up a dummy
// window from a separate thread would fix that).

// Blocking sockets with asynchronous event notification are currently not safe to use.

struct socketbase *asyncsb[MAXPENDINGASYNC];
SOCKET asyncsock[MAXPENDINGASYNC];
uae_u32 asyncsd[MAXPENDINGASYNC];
int asyncindex;

int host_sbinit(TrapContext *context, SB)
{
	sb->sockAbort = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);

	if (sb->sockAbort == INVALID_SOCKET)
		return 0;
	if ((sb->hEvent = CreateEvent(NULL,FALSE,FALSE,NULL)) == NULL)
		return 0;

	sb->mtable = xcalloc(unsigned int, sb->dtablesize);

	return 1;
}

void host_closesocketquick(int s)
{
	BOOL val = 1;

	if( s )
	{
	    setsockopt((SOCKET)s,SOL_SOCKET,SO_DONTLINGER,(char *)&val,sizeof(val));
	    shutdown(s,1);
	    closesocket((SOCKET)s);
	}	 
}

void host_sbcleanup(SB)
{
	int i;

	for (i = 0; i < MAXPENDINGASYNC; i++) if (asyncsb[i] == sb) asyncsb[i] = NULL;

	if (sb->hEvent != NULL) CloseHandle(sb->hEvent);

	for (i = sb->dtablesize; i--; )
	{
		if (sb->dtable[i] != (int)INVALID_SOCKET) host_closesocketquick(sb->dtable[i]);

		if (sb->mtable[i]) asyncsb[(sb->mtable[i]-0xb000)/2] = NULL;
	}

	shutdown(sb->sockAbort,1);
	closesocket(sb->sockAbort);

	free(sb->mtable);	 
}

void host_sbreset(void)
{
	memset(asyncsb,0,sizeof asyncsb);
	memset(asyncsock,0,sizeof asyncsock);
	memset(asyncsd,0,sizeof asyncsd);
	memset(threadargs,0,sizeof threadargs);
}

void sockmsg(unsigned int msg, unsigned long wParam, unsigned long lParam)
{
	 
}

static unsigned int allocasyncmsg(SB,uae_u32 sd,SOCKET s)
{
	 
	int i;
	locksigqueue();

	for (i = asyncindex+1; i != asyncindex; i++)
	{
		if (i == MAXPENDINGASYNC) i = 0;

		if (!asyncsb[i])
		{
			asyncsb[i] = sb;
			if (++asyncindex == MAXPENDINGASYNC) asyncindex = 0;
			unlocksigqueue();

			if (s == INVALID_SOCKET)
			{
				return i*2+0xb001;
			}
			else
			{
				asyncsd[i] = sd;
				asyncsock[i] = s;
				return i*2+0xb000;
			}
		}
	}

	unlocksigqueue();

	write_log ("BSDSOCK: ERROR - Async operation completion table overflow\n");

	return 0;
}

static void cancelasyncmsg(unsigned int wMsg)
{
	SB;

	wMsg = (wMsg-0xb000)/2;

	sb = asyncsb[wMsg];

	if (sb != NULL)
	{
		asyncsb[wMsg] = NULL;
		CANCELSIGNAL;
	}	 
}

void sockabort(SB)
{
    locksigqueue();

    unlocksigqueue();   
}

void setWSAAsyncSelect(SB, uae_u32 sd, SOCKET s, long lEvent )
{
	 if (sb->mtable[sd-1])
		{
		long wsbevents = 0;
		long eventflags;
		int i;
		locksigqueue();


		eventflags = sb->ftable[sd-1]  & REP_ALL;

		if (eventflags & REP_ACCEPT) wsbevents |= FD_ACCEPT;
		if (eventflags & REP_CONNECT) wsbevents |= FD_CONNECT;
		//if (eventflags & REP_OOB) wsbevents |= FD_OOB;
		if (eventflags & REP_READ) wsbevents |= FD_READ;
		if (eventflags & REP_WRITE) wsbevents |= FD_WRITE;
		if (eventflags & REP_CLOSE) wsbevents |= FD_CLOSE;
		wsbevents |= lEvent;
		i = (sb->mtable[sd-1]-0xb000)/2;
		asyncsb[i] = sb;
		asyncsd[i] = sd;
		asyncsock[i] = s;
		//WSAEventSelect(s,hWndSelector ? hAmigaWnd : hSockWnd,sb->mtable[sd-1],wsbevents);

		unlocksigqueue();
		}
}


// address cleaning
static void prephostaddr(SOCKADDR_IN *addr)
{
    addr->sin_family = AF_INET;
}

static void prepamigaaddr(struct sockaddr *realpt, int len)
{
    // little endian address family value to the byte sin_family member
 
    ((char *)realpt)[1] = *((char *)realpt);

    // set size of address
    *((char *)realpt) = len;
}


int host_dup2socket(SB, int fd1, int fd2)
	{
    
	return -1;
	}

int host_socket(SB, int af, int type, int protocol)
{
 
   int sd;
    SOCKET s;
    unsigned long nonblocking = 1;

	BSDTRACE(("socket(%s,%s,%d) -> ",af == AF_INET ? "AF_INET" : "AF_other",type == SOCK_STREAM ? "SOCK_STREAM" : type == SOCK_DGRAM ? "SOCK_DGRAM " : "SOCK_RAW",protocol));

    if ((s = socket(af,type,protocol)) == INVALID_SOCKET)
    {
		//SETERRNO;
		BSDTRACE(("failed (%d)\n",sb->sb_errno));
		return -1;
    }
    else
	sd = getsd(sb,(int)s);

	sb->ftable[sd-1] = SF_BLOCKING;
    ioctlsocket(s,FIONBIO,&nonblocking);
    BSDTRACE(("%d\n",sd));

	if (type == 3)
		{
		if (protocol==IPPROTO_UDP)
			{
			sb->ftable[sd-1] |= SF_RAW_UDP;
			}
		if (protocol==IPPROTO_ICMP)
			{
			struct sockaddr_in sin;

			sin.sin_family = AF_INET;
			sin.sin_addr.s_addr = INADDR_ANY;
			bind(s,(struct sockaddr *)&sin,sizeof(sin)) ;
			}
		if (protocol==IPPROTO_RAW)
			{
			sb->ftable[sd-1] |= SF_RAW_RAW;
			}
		}
	return sd-1;
}

uae_u32 host_bind(SB, uae_u32 sd, uae_u32 name, uae_u32 namelen)
{
     

    return 1;
}

uae_u32 host_listen(SB, uae_u32 sd, uae_u32 backlog)
{
   
    return 1;
}

void host_accept(SB, uae_u32 sd, uae_u32 name, uae_u32 namelen)
{
    

}

typedef enum
{
    connect_req,
    recvfrom_req,
    sendto_req,
    abort_req,
    last_req
} threadsock_e;

struct threadsock_packet
{
    threadsock_e packet_type;
    union
    {
	struct sendto_params
	{
	    char *buf;
	    char *realpt;
	    uae_u32 sd;
	    uae_u32 msg;
	    uae_u32 len;
	    uae_u32 flags;
	    uae_u32 to;
	    uae_u32 tolen;
	} sendto_s;
	struct recvfrom_params
	{
	    char *realpt;
	    uae_u32 addr;
	    uae_u32 len;
	    uae_u32 flags;
	    struct sockaddr *rp_addr;
	    int *hlen;
	} recvfrom_s;
	struct connect_params
	{
	    char *buf;
	    uae_u32 namelen;
	} connect_s;
	struct abort_params
	{
	    SOCKET *newsock;
	} abort_s;
    } params;
    SOCKET s;
    SB;
} sockreq;

BOOL HandleStuff( void )
{
    
    return TRUE;
}

static long FAR PASCAL SocketWindowProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
 
}



static unsigned int __stdcall sock_thread(void *blah)
{
  
    return 0;
}


void host_connect(TrapContext *context, SB, uae_u32 sd, uae_u32 name, uae_u32 namelen)
{
    SOCKET s;
    int success = 0;
    unsigned int wMsg;
    char buf[MAXADDRLEN];


	sd++;
    BSDTRACE(("connect(%d,0x%lx,%d) -> ",sd,name,namelen));

    s = (SOCKET)getsock(sb,(int)sd);

    if (s != INVALID_SOCKET)
    {
	if (namelen <= MAXADDRLEN)
	{
	    if (sb->mtable[sd-1] || (wMsg = allocasyncmsg(sb,sd,s)) != 0)
	    {
		if (sb->mtable[sd-1] == 0)
			{
			//WSAAsyncSelect(s,hWndSelector ? hAmigaWnd : hSockWnd,wMsg,FD_CONNECT);
			WSAEventSelect(s,hSockReq,FD_CONNECT);
			}
		else
			{
			setWSAAsyncSelect(sb,sd,s,FD_CONNECT);
			}


		BEGINBLOCKING;
		PREPARE_THREAD;

		memcpy(buf,get_real_address (name),namelen);
		prephostaddr((SOCKADDR_IN *)buf);

		sockreq.packet_type = connect_req;
		sockreq.s = s;
		sockreq.sb = sb;
		sockreq.params.connect_s.buf = buf;
		sockreq.params.connect_s.namelen = namelen;

		TRIGGER_THREAD;


		if (sb->resultval)
		{
		    if (sb->sb_errno == WSAEWOULDBLOCK-WSABASEERR)
		    {
			if (sb->ftable[sd-1] & SF_BLOCKING)
			{
			    bsdsocklib_seterrno(sb,0);


				WAITSIGNAL;

			    if (sb->eintr)
			    {
				// Destroy socket to cancel abort, replace it with fake socket to enable proper closing.
				// This is in accordance with BSD behaviour.
				shutdown(s,1);
				closesocket(s);
				sb->dtable[sd-1] = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
			    }
			}
			else
					{
		    bsdsocklib_seterrno(sb,36);	// EINPROGRESS

					}
		    }
		else
			{
			CANCELSIGNAL; // Cancel pending signal

			}
			}

			ENDBLOCKING;
			if (sb->mtable[sd-1] == 0)
				{
				cancelasyncmsg(wMsg);
				}
			else
				{
				setWSAAsyncSelect(sb,sd,s,0);
				}
			}

	}
	else
	    write_log ("BSDSOCK: WARNING - Excessive namelen (%d) in connect()!\n",namelen);
    }
    BSDTRACE(("%d\n",sb->sb_errno));
}

void host_sendto(SB, uae_u32 sd, uae_u32 msg, uae_u32 len, uae_u32 flags, uae_u32 to, uae_u32 tolen)
{
    

}

void host_recvfrom(SB, uae_u32 sd, uae_u32 msg, uae_u32 len, uae_u32 flags, uae_u32 addr, uae_u32 addrlen)
{
  

}

uae_u32 host_shutdown(SB, uae_u32 sd, uae_u32 how)
{
   

    return -1;
}

void host_setsockopt(SB, uae_u32 sd, uae_u32 level, uae_u32 optname, uae_u32 optval, uae_u32 len)
{
     
}

uae_u32 host_getsockopt(SB, uae_u32 sd, uae_u32 level, uae_u32 optname, uae_u32 optval, uae_u32 optlen)
{
 

	return -1;
}

uae_u32 host_getsockname(SB, uae_u32 sd, uae_u32 name, uae_u32 namelen)
{
 

	return -1;
}

uae_u32 host_getpeername(SB, uae_u32 sd, uae_u32 name, uae_u32 namelen)
{
	 

	return -1;
}

uae_u32 host_IoctlSocket(SB, uae_u32 sd, uae_u32 request, uae_u32 arg)
{
 

	return 0;
}

int host_CloseSocket(SB, int sd)
{
   
    return -1;
}

// For the sake of efficiency, we do not malloc() the fd_sets here.
// 64 sockets should be enough for everyone.
static void makesocktable(SB, uae_u32 fd_set_amiga, struct fd_set *fd_set_win, int nfds, SOCKET addthis)
{
 
}

static void makesockbitfield(SB, uae_u32 fd_set_amiga, struct fd_set *fd_set_win, int nfds)
{
	 
}

static void fd_zero(uae_u32 fdset, uae_u32 nfds)
{
	 
}

// This seems to be the only way of implementing a cancelable WinSock2 select() call... sigh.
static unsigned int __stdcall thread_WaitSelect(void *index2)
{
  
    return 0;
}

void host_WaitSelect(SB, uae_u32 nfds, uae_u32 readfds, uae_u32 writefds, uae_u32 exceptfds, uae_u32 timeout, uae_u32 sigmp)
{
 
}

uae_u32 host_Inet_NtoA(SB, uae_u32 in)
{
 

	return 0;
}

uae_u32 host_inet_addr(uae_u32 cp)
{
	 
	return 0;
}

int isfullscreen (void);
BOOL CheckOnline(SB)
	{
	 return FALSE;
	}

static unsigned int __stdcall thread_get(void *index2)
{
    
    return 0;
}


void host_gethostbynameaddr (TrapContext *context, SB, uae_u32 name, uae_u32 namelen, long addrtype)
{
 
	HOSTENT *h;
	WSAEVENT hEvent;
	XNDNS* pDns;
	char ip[40];
	char ipname[100];

	int size, numaliases = 0, numaddr = 0;
	uae_u32 aptr;
	char *name_rp;
	int i;

	uae_u32 args[6];

	uae_u32 addr;
	uae_u32 *addr_list[2];

	char buf[1024];
	unsigned int wMsg = 0;
 

//	char on = 1;
//	InternetSetOption(0,INTERNET_OPTION_SETTINGS_CHANGED,&on,strlen(&on));
//  Do not use: Causes locks with some machines

	name_rp = get_real_address (name);
 
	BSDTRACE(("gethostbyname(%s) -> ",name_rp));

	// workaround for numeric host "names"
	if ((addr = inet_addr(name_rp)) != INADDR_NONE)
	{
 
		bsdsocklib_seterrno(sb,0);
		((HOSTENT *)buf)->h_name = name_rp;
		((HOSTENT *)buf)->h_aliases = NULL;
		((HOSTENT *)buf)->h_addrtype = AF_INET;
		((HOSTENT *)buf)->h_length = 4;
		((HOSTENT *)buf)->h_addr_list = (char **)&addr_list;
		addr_list[0] = &addr;
		addr_list[1] = NULL;

		FD_ISSET 
	}
	else
	{	
 
		hEvent = WSACreateEvent();
		pDns = NULL;
		XNetDnsLookup(name_rp, hEvent, &pDns);
		WaitForSingleObject( (HANDLE)hEvent, INFINITE);
		if ( pDns && pDns->iStatus == 0 )
		{
			//DNS lookup succeeded

			unsigned long ulHostIp;
			memcpy(&ulHostIp, &(pDns->aina[0].s_addr), 4);
			//sprintf(ip,"%d.%d.%d.%d", (ulHostIp & 0xFF), (ulHostIp & 0xFF00) >> 8, (ulHostIp & 0xFF0000) >> 16, (ulHostIp & 0xFF000000) >> 24 );
			sprintf(ip,"%d.%d.%d.%d", (ulHostIp & 0xFF000000) >> 24 , (ulHostIp & 0xFF0000) >> 16,  (ulHostIp & 0xFF00) >> 8, (ulHostIp & 0xFF) );

			write_log("Resolved to IP : %s\n",ip);

			h = BuildHostEnt(name_rp,name_rp,"",ip);

			XNetDnsRelease(pDns);
			WSACloseEvent(hEvent);
		 
		}
	}

	// compute total size of hostent
	size = 28;
	if (h->h_name != NULL) size += strlen(h->h_name)+1;

	if (h->h_aliases != NULL)
		while (h->h_aliases[numaliases]) size += strlen(h->h_aliases[numaliases++])+5;

	if (h->h_addr_list != NULL)
	{
		while (h->h_addr_list[numaddr]) numaddr++;
		size += numaddr*(h->h_length+4);
	}

	if (sb->hostent)
	{
	uae_FreeMem(  context, sb->hostent, sb->hostentsize );
	}

	sb->hostent = uae_AllocMem( context, size, 0 );

	if (!sb->hostent)
	{
		write_log ("BSDSOCK: WARNING - gethostby%s() ran out of Amiga memory (couldn't allocate %ld bytes) while returning result of lookup for '%s'\n",addrtype == -1 ? "name" : "addr",size,(char *)name);
		bsdsocklib_seterrno(sb,12); // ENOMEM
		return;
	}

	sb->hostentsize = size;

	aptr = sb->hostent+28+numaliases*4+numaddr*4;

	// transfer hostent to Amiga memory
	put_long (sb->hostent+4,sb->hostent+20);
	put_long (sb->hostent+8,h->h_addrtype);
	put_long (sb->hostent+12,h->h_length);
	put_long (sb->hostent+16,sb->hostent+24+numaliases*4);

	for (i = 0; i < numaliases; i++) put_long (sb->hostent+20+i*4,addstr(&aptr,h->h_aliases[i]));
	put_long (sb->hostent+20+numaliases*4,0);
	for (i = 0; i < numaddr; i++) put_long (sb->hostent+24+(numaliases+i)*4,addmem(&aptr,h->h_addr_list[i],h->h_length));
	put_long (sb->hostent+24+numaliases*4+numaddr*4,0);
	put_long (sb->hostent,aptr);
	addstr(&aptr,h->h_name);

	BSDTRACE(("OK (%s)\n",h->h_name));
	bsdsocklib_seterrno(sb,0);

 
}

void host_getprotobyname(SB, uae_u32 name)
{
	 

}


void host_getservbynameport(SB, uae_u32 nameport, uae_u32 proto, uae_u32 type)
{
	 
 
}



uae_u32 host_gethostname(uae_u32 name, uae_u32 namelen)
{
	 
}

#endif
#endif
