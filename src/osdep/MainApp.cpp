//--------------------------------------------------------------------------------------
// XuiTutorial.cpp
//
// Shows how to display and use a simple XUI scene.
//
// Xbox Advanced Technology Group.
// Copyright (C) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
#include <xtl.h>
#include <xui.h>
#include <xuiapp.h>
#include <xfilecache.h>
#include "MainApp.h"
#include "RomList.h"
#include "HDDList.h"
#include "AmigaOptions.h"
#include "OptionsMore.h"
#include "SaveStates.h"
#include "ConfigFiles.h"
#include "XUIDefines.h"
#include "SDL.h"
#include "SDL_mixer.h"
#include "KickStart.h"

extern int mainloopdone;

// xbox utility functions

#define DEVICE_MEMORY_UNIT0 1
#define DEVICE_MEMORY_UNIT1 2
#define DEVICE_MEMORY_ONBOARD 3
#define DEVICE_CDROM0 4
#define DEVICE_HARDISK0_PART1 5
#define DEVICE_HARDISK0_SYSPART 6
#define DEVICE_USB0 7
#define DEVICE_USB1 8
#define DEVICE_USB2 9
#define DEVICE_TEST 10
#define DEVICE_CACHE 11

typedef struct _STRING {
    USHORT Length;
    USHORT MaximumLength;
    PCHAR Buffer;
} STRING;

extern "C"
{
#include "options.h"
#include "gui.h"
#include "uae.h"
#include "disk.h"
}
extern "C" int __stdcall ObCreateSymbolicLink( STRING*, STRING*);

void Mount( int Device, char* MountPoint )
{
	char MountConv[260];
	sprintf_s( MountConv,"\\??\\%s", MountPoint );

	char * SysPath = NULL;
	switch( Device )
	{
		case DEVICE_MEMORY_UNIT0:
			SysPath = "\\Device\\Mu0";
			break;
		case DEVICE_MEMORY_UNIT1:
			SysPath = "\\Device\\Mu1";
			break;
		case DEVICE_MEMORY_ONBOARD:
			SysPath = "\\Device\\BuiltInMuSfc";
			break;
		case DEVICE_CDROM0:
			SysPath = "\\Device\\Cdrom0";
			break;
		case DEVICE_HARDISK0_PART1:
			SysPath = "\\Device\\Harddisk0\\Partition1";
			break;
		case DEVICE_HARDISK0_SYSPART:
			SysPath = "\\Device\\Harddisk0\\SystemPartition";
			break;
		case DEVICE_USB0:
			SysPath = "\\Device\\Mass0";
			break;
		case DEVICE_USB1:
			SysPath = "\\Device\\Mass1";
			break;
		case DEVICE_USB2:
			SysPath = "\\Device\\Mass2";
			break;
		case DEVICE_CACHE:
			SysPath = "\\Device\\Harddisk0\\Cache0";
			break;
	}

	STRING sSysPath = { (USHORT)strlen( SysPath ), (USHORT)strlen( SysPath ) + 1, SysPath };
	STRING sMountConv = { (USHORT)strlen( MountConv ), (USHORT)strlen( MountConv ) + 1, MountConv };
	int res = ObCreateSymbolicLink( &sMountConv, &sSysPath );


}

//--------------------------------------------------------------------------------------
// Scene implementation class.
//--------------------------------------------------------------------------------------
class CAmiga360Menu : public CXuiSceneImpl
{
 
public:
	CXuiControl m_SignInLabel;
protected:
    
	CXuiControl m_Reset;
	CXuiControl m_Back;
    CXuiControl m_button2;
	CXuiControl m_button3;
	CXuiControl m_button4;
	CXuiControl m_button5;
	CXuiControl m_Storage;	 

	CXuiControl m_Tab1;
	CXuiTextElement m_Song;
	CXuiTextElement m_Format;
   
 
    // Message map.
    XUI_BEGIN_MSG_MAP()
        XUI_ON_XM_INIT( OnInit )
        XUI_ON_XM_NOTIFY_PRESS( OnNotifyPress )
		XUI_ON_XM_RENDER( OnRender )
    XUI_END_MSG_MAP()
 
public:
 

	// Handler for the XM_NOTIFY message
    HRESULT OnNotifyPress( HXUIOBJ hObjPressed, 
       BOOL& bHandled )
    {

		if (hObjPressed == m_Back)
		{
			
			mainloopdone = 1;
			bHandled = TRUE;
			return S_OK;

		}
		else if (hObjPressed == m_button5)
		{
			XLaunchNewImage("",XLAUNCH_KEYWORD_DASH);
		}
		else if (hObjPressed == m_button4)
		{

			wchar_t msg[1024];
			char Information[1024];
			char line[255];


			
			strcpy (Information, "------------------------------------------------------------------------------------\n");
			sprintf(line, "P-UAE %d.%d.%d\n", UAEMAJOR, UAEMINOR, UAESUBREV);
			strcat (Information, line);
			sprintf(line, "Git Commit: %s\n", PACKAGE_COMMIT);
			strcat (Information, line);
			sprintf(line, "Build date: " __DATE__ " " __TIME__ "\n");
			strcat (Information, line);
			strcat (Information, "------------------------------------------------------------------------------------\n");
			strcat (Information, "Copyright 1995-2002 Bernd Schmidt\n");
			strcat (Information, "          1999-2011 Toni Wilen\n");
			strcat (Information, "          2003-2007 Richard Drummond\n");
			strcat (Information, "          2006-2011 Mustafa 'GnoStiC' Tufan\n");
			strcat (Information, "          2011 Amiga360 Port by Lantus\n\n");
			strcat (Information, "See the source for a full list of contributors.\n");
			strcat (Information, "This is free software; see the file COPYING for copying conditions.  There is NO\n");
			strcat (Information, "warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n");
			strcat (Information, "------------------------------------------------------------------------------------\n");			

			swprintf_s(msg,L"%S",Information);
			const WCHAR * button_text = L"OK"; 
			ShowMessageBoxEx(L"XuiMessageBox1",this->m_hObj,L"Amiga360 - About", msg, 1, (LPCWSTR*)&button_text,NULL,  NULL, NULL); 
			bHandled = TRUE;
			
		}
		else if (hObjPressed == m_Reset)
		{
			uae_reset(1);
			mainloopdone = 1;
			bHandled = TRUE;
		}
 
		return S_OK;	 
    }

	HRESULT OnRender(XUIMessageRender *pRenderData, BOOL &bHandled)
	{		
 
		return S_OK;
	}


    //----------------------------------------------------------------------------------
    // Performs initialization tasks - retreives controls.
    //----------------------------------------------------------------------------------
    HRESULT OnInit( XUIMessageInit* pInitData, BOOL& bHandled )
    {
		 	  		 
        GetChildById( L"XuiFavorites", &m_button2 );
        GetChildById( L"XuiOptions", &m_button3 );
		GetChildById( L"XuiAbout", &m_button4 );
		GetChildById( L"XuiQuit", &m_button5 );
		GetChildById( L"XuiStorageDevice", &m_Storage );		 
		GetChildById( L"XuiTextPlayingSong", &m_Song );
		GetChildById( L"XuiTextPlayingFormat", &m_Format );
		GetChildById( L"XuiButtonReset", &m_Reset);
		
		GetChildById( L"XuiReturn", &m_Back);

 		Mount(DEVICE_USB0,"Usb0:");
 		Mount(DEVICE_USB1,"Usb1:");
 		Mount(DEVICE_USB2,"Usb2:");
 		Mount(DEVICE_HARDISK0_PART1,"Hdd1:");
 		Mount(DEVICE_HARDISK0_SYSPART,"HddX:");
 		Mount(DEVICE_MEMORY_UNIT0,"Memunit0:");
 		Mount(DEVICE_MEMORY_UNIT1,"Memunit1:");
		Mount(DEVICE_MEMORY_ONBOARD,"OnBoardMU:"); 
		Mount(DEVICE_CDROM0,"Dvd:"); 


 		CXuiSceneCache sceneCache;
		
		sceneCache.Init();
		sceneCache.CacheScene(L"file://game:/media/skin.xzp#Skin\\SaveStates.xur",2,NULL);
		sceneCache.CacheScene(L"file://game:/media/skin.xzp#Skin\\RomsList.xur",2,NULL);
		sceneCache.CacheScene(L"file://game:/media/skin.xzp#Skin\\Options.xur",2,NULL);
		sceneCache.CacheScene(L"file://game:/media/skin.xzp#Skin\\Music.xur",2,NULL);
		sceneCache.CacheScene(L"file://game:/media/skin.xzp#Skin\\KickStart.xur",2,NULL);
		sceneCache.CacheScene(L"file://game:/media/skin.xzp#Skin\\Config.xur",2,NULL);
		sceneCache.CacheScene(L"file://game:/media/skin.xzp#Skin\\Harddisks.xur",2,NULL);		  
		
        return S_OK;
    }
	
	
 

public:

    // Define the class. The class name must match the ClassOverride property
    // set for the scene in the UI Authoring tool.
    XUI_IMPLEMENT_CLASS( CAmiga360Menu, L"MainScene", XUI_CLASS_SCENE )
	
};

 
//--------------------------------------------------------------------------------------
// Name: RegisterXuiClasses
// Desc: Registers all the scene classes.
//--------------------------------------------------------------------------------------
HRESULT CAmiga360App::RegisterXuiClasses()
{
    // We must register the video control classes     
	XuiSoundXAudioRegister();
	CAmiga360Menu::Register();
	CRomList::Register();
	CRomListScene::Register();
	CHDDList::Register();
	CHDDListScene::Register();
	COptionsScene::Register();
	CKickList::Register();
	CKickListScene::Register();
	CSaveList::Register();
	CSaveListScene::Register();
	CConfigListScene::Register();
	CConfigList::Register();
	COptionsMoreScene::Register();
	
    // Register any other classes necessary for the app/scene
    return S_OK;
}

 
//--------------------------------------------------------------------------------------
// Name: UnregisterXuiClasses
// Desc: Unregisters all the scene classes.
//--------------------------------------------------------------------------------------
HRESULT CAmiga360App::UnregisterXuiClasses()
{
    CAmiga360Menu::Unregister();
	CRomList::Unregister();
	CRomListScene::Unregister();
	COptionsScene::Unregister();
	CKickList::Unregister();
	CKickListScene::Unregister();
	CSaveList::Unregister();
	CSaveListScene::Unregister();
	CHDDList::Unregister();
	CHDDListScene::Unregister();
	CConfigListScene::Unregister();
	CConfigList::Unregister();
	COptionsMoreScene::Unregister();
    return S_OK;
}
 

