 /*
  * UAE - The Un*x Amiga Emulator
  *
  * Interface to the Tcl/Tk GUI
  *
  * Copyright 1996 Bernd Schmidt
  */

#include <xtl.h>
#include <xui.h>
#include <xuiapp.h>
#include <xfilecache.h>
#include "MainApp.h"
#include "RomList.h"
#include "AmigaOptions.h"
#include "XUIDefines.h"
#include "SDL.h"
//#include "SDL_mixer.h"

#include "sysconfig.h"
#include "sysdeps.h"


extern "C"
{
#include "options.h"
#include "uae.h"
#include "gui.h"
#include "disk.h"
#include "picasso96.h"
void gui_message (const char *format,...);
}


extern "C" struct uae_prefs currprefs, changed_prefs;
extern "C" struct picasso_vidbuf_description picasso_vidinfo;
extern "C" int current_width, current_height;
extern "C" int screen_is_picasso;
extern "C" LPDIRECT3DDEVICE9 D3D_Device;
extern "C" LPDIRECT3D9 D3D; 
 
//--------------------------------------------------------------------------------------
// Name: main
// Desc: Application entry point.
//--------------------------------------------------------------------------------------

IDirect3DDevice9 *pDevice;
D3DPRESENT_PARAMETERS d3dpp;
int mainloopdone = 0;

CAmiga360App app;	
HXUIOBJ hMainScene;

HRESULT RenderGame( IDirect3DDevice9 *pDevice )
	{
    // Render game graphics.
    pDevice->Clear(
        0,
        NULL,
        D3DCLEAR_TARGET | D3DCLEAR_STENCIL | D3DCLEAR_ZBUFFER,
        D3DCOLOR_ARGB( 255, 32, 32, 64 ),
        1.0,
        0 );

    return S_OK;
}



HRESULT RenderUI()
{
 
	RenderGame( pDevice );

	// Update XUI
	app.RunFrame();

	// Render XUI
	app.Render();

	// Update XUI Timers
	XuiTimersRun();

	pDevice->Present(NULL,NULL,NULL,NULL);


    return S_OK;
}
 
HRESULT InitD3D( IDirect3DDevice9 **ppDevice, 
    D3DPRESENT_PARAMETERS *pd3dPP )
{
    IDirect3D9 *pD3D;

    pD3D = Direct3DCreate9( D3D_SDK_VERSION );

    // Set up the presentation parameters.
    ZeroMemory( pd3dPP, sizeof( D3DPRESENT_PARAMETERS ) );
    pd3dPP->BackBufferWidth        = 1280;
    pd3dPP->BackBufferHeight       = 720;
    pd3dPP->BackBufferFormat       = D3DFMT_X8R8G8B8;
    pd3dPP->BackBufferCount        = 1;
    pd3dPP->MultiSampleType        = D3DMULTISAMPLE_NONE;
    pd3dPP->SwapEffect             = D3DSWAPEFFECT_DISCARD;
    pd3dPP->PresentationInterval   = D3DPRESENT_INTERVAL_ONE;
    
	D3D = pD3D;

    // Create the device.
    return pD3D->CreateDevice(
                    0, 
                    D3DDEVTYPE_HAL,
                    NULL,
                    D3DCREATE_HARDWARE_VERTEXPROCESSING,
                    pd3dPP,
                    ppDevice );

	
}

 

int gui_init (void)
{
	// Declare an instance of the XUI framework application.
		
    HRESULT hr;
 
  /* We're going to be requesting certain things from our audio
     device, so we set them up beforehand */
    
	pDevice = D3D_Device;
    // Initialize the application.    
	 
    hr = app.InitShared( pDevice, &d3dpp, 
        XuiD3DXTextureLoader );
 
    if( FAILED( hr ) )
    {  
        OutputDebugString( "Failed intializing application.\n" );
        return 0;
    }

    // Register a default typeface
    hr = app.RegisterDefaultTypeface( L"Arial Unicode MS", L"file://game:/media/font.ttf" );

    if( FAILED( hr ) )
    {
        OutputDebugString( "Failed to register default typeface.\n" );
        return 0;
    }
    // Load the skin file used for the scene.
	app.LoadSkin( L"file://game:/media/skin.xzp#Skin\\skin.xur" );     	
	XuiSceneCreate( L"file://game:/media/skin.xzp#Skin\\", L"Main.xur", NULL, &hMainScene );	
	XuiSceneNavigateFirst( app.GetRootObj(), hMainScene, XUSER_INDEX_FOCUS );
 
	strcpy(changed_prefs.romfile,"GAME:\\KickStart\\kick.rom");
	changed_prefs.nr_floppies = 1;	

	config_changed = 1;

	return 1;
}

void gui_fps (int fps, int idle)
{
    gui_data.fps  = fps;
    gui_data.idle = idle;
}

void gui_flicker_led (int led, int unitnum, int status)
{
}

void gui_led (int led, int on)
{
}

void gui_filename (int num, const char *name)
{
}

void gui_handle_events (void)
{
}

int gui_update (void)
{


	return 0;
}

void gui_exit (void)
{
}


void gui_display(int shortcut)
{	
	int menu_exitcode = -1;
	
	mainloopdone = 0;
 

	while(!mainloopdone)
	{
		RenderGame( pDevice );
 	 		 
		// Update XUI
		app.RunFrame();

		// Render XUI
		app.Render();
		
		// Update XUI Timers
		XuiTimersRun();

 
		pDevice->Present(NULL,NULL,NULL,NULL);
	}  


	if (screen_is_picasso)
		SDL_SetVideoMode(picasso_vidinfo.width, picasso_vidinfo.height, 32, 0);
	else
		SDL_SetVideoMode(current_width, current_height, 32, 0);
  
}

HRESULT ShowMessageBoxEx(LPCWSTR szVisual, HXUIOBJ hParent, LPCWSTR szTitle, LPCWSTR szText, int nButtons, LPCWSTR *pButtonsText, int nFocusButton, DWORD dwFlags, HXUIOBJ* phObj = NULL)
{
	return XuiShowMessageBoxEx(szVisual, hParent, NULL, szTitle, szText, nButtons, pButtonsText, nFocusButton, dwFlags, phObj);
}

void gui_message (const char *format,...)
{
	wchar_t msg[255];
	const WCHAR * button_text = L"OK"; 
	swprintf_s(msg,L"%S",format);
	ShowMessageBoxEx(L"XuiMessageBox1",NULL,L"Amiga 360", msg, 1, (LPCWSTR*)&button_text,NULL,  XUI_MB_CENTER_ON_PARENT, NULL); 
}

void gui_disk_image_change (int unitnum, const TCHAR *name, bool writeprotected) {}
void gui_lock (void) {}
void gui_unlock (void) {}


void gui_gameport_button_change (int port, int button, int onoff)
{

}

void gui_gameport_axis_change (int port, int axis, int state, int max)
{
      
}

