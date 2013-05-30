#include <xtl.h>
#include "MainApp.h"
#include "RomList.h"
 
 
#include <SDL.h>
#include <SDL_endian.h>


extern "C"
{
#include "options.h"
#include "gui.h"
#include "uae.h"
#include "disk.h"
}

HXUIOBJ hKickScene;
HXUIOBJ hRomListScene;

extern int mainloopdone;

static int currDrive = 0;
wchar_t ucString[42];
char szRoms[200]; 
char szRomPath[200];
extern int doDriveClick;

LPCWSTR MultiCharToUniChar(char* mbString);

extern HXUIOBJ hMainScene;
extern int mainloopdone;
extern CAmiga360App app;
extern "C" LPDIRECT3DDEVICE9 D3D_Device;
extern "C" struct uae_prefs currprefs, changed_prefs;

std::vector<std::string> m_ListData;

extern "C" void mount_hdf(int index, char* name);

void InRescanRomsFirstFunc(XUIMessage *pMsg, InRescanRomsStruct* pData, char *szPath)
{
    XuiMessage(pMsg,XM_MESSAGE_ON_RESCAN_ROMS);
    _XuiMessageExtra(pMsg,(XUIMessageData*) pData, sizeof(*pData));

	
}

// Handler for the XM_NOTIFY message
HRESULT CRomListScene::OnNotifyPress( HXUIOBJ hObjPressed, 
       BOOL& bHandled )
    {
		int nIndex;
		wchar_t tmpFile[MAX_PATH];

        if ( hObjPressed == m_RomList )
        {
			nIndex = m_RomList.GetCurSel(); 
			 									 
			mainloopdone = 1;
			bHandled = TRUE;
			return S_OK;
			
        }
		else if ( hObjPressed == m_DriveList )
        {
            nIndex = m_DriveList.GetCurSel();
            //m_text1.SetText(m_commonlist.GetText(nIndex));;
            // handle list item press here...

			bHandled = TRUE;
			return S_OK;

        }
		else if (hObjPressed == m_DiskEject)
		{

			switch (currDrive)
			{
			case 0:
				swprintf_s(tmpFile, L"DF0 : Empty");
				m_DeviceDrive1.SetText(tmpFile);			 
				break;
			case 1:
				swprintf_s(tmpFile, L"DF1 : Empty");
				m_DeviceDrive2.SetText(tmpFile);			 
				break;
			case 2:
				swprintf_s(tmpFile, L"DF2 : Empty");
				m_DeviceDrive3.SetText(tmpFile);			 
				break;
			case 3:
				swprintf_s(tmpFile, L"DF3 : Empty");
				m_DeviceDrive4.SetText(tmpFile);			 
				break;
			}

			disk_eject(currDrive);

			bHandled = TRUE;
			return S_OK;

		}
		else if (hObjPressed == m_InsertDiskDrive0)
		{
			nIndex = m_RomList.GetCurSel(); 			 			 
		 				  
			switch (currDrive)
			{
			case 0:
				swprintf_s(tmpFile, L"DF0 : %S", (char *)m_ListData[nIndex].c_str());
				m_DeviceDrive1.SetText(tmpFile);			 
				break;
			case 1:
				swprintf_s(tmpFile, L"DF1 : %S", (char *)m_ListData[nIndex].c_str());
				m_DeviceDrive2.SetText(tmpFile);			 
				break;
			case 2:
				swprintf_s(tmpFile, L"DF2 : %S", (char *)m_ListData[nIndex].c_str());
				m_DeviceDrive3.SetText(tmpFile);			 
				break;
			case 3:
				swprintf_s(tmpFile, L"DF3 : %S", (char *)m_ListData[nIndex].c_str());
				m_DeviceDrive4.SetText(tmpFile);			 
				break;
			}
			
			

			strcpy(szRoms,"GAME:\\ROMS\\");
			strcat(szRoms,(char *)m_ListData[nIndex].c_str());


			if (currprefs.nr_floppies-1 < currDrive ) {
				currprefs.nr_floppies = currDrive  + 1;
			}

		 
			if (currprefs.floppyslots[currDrive].dfxtype < 0) {
				currprefs.floppyslots[currDrive].dfxtype = 0;
				DISK_check_change();
			}

 

			strcpy(changed_prefs.floppyslots[currDrive].df, szRoms);
			disk_insert (currDrive, changed_prefs.floppyslots[currDrive].df);
 

			config_changed = 1;
			bHandled = TRUE;
			return S_OK;
			


		}
		else if (hObjPressed == m_SwitchDrive)
		{

			
			currDrive++;

			if (currDrive > 3)
			{
				currDrive = 0;
			}

			switch (currDrive)
			{
			case 0:
				swprintf_s(tmpFile, L"Current Drive : DF0");
				break;
			case 1:
				swprintf_s(tmpFile, L"Current Drive : DF1");
				break;
			case 2:
				swprintf_s(tmpFile, L"Current Drive : DF2");
				break;
			case 3:
				swprintf_s(tmpFile, L"Current Drive : DF3");
				break;
			}

			m_SwitchDrive.SetText(tmpFile);


			bHandled = TRUE;
			return S_OK;

			
		}
		else if (hObjPressed == m_Kick)
		{

			XuiSceneCreate( L"file://game:/media/skin.xzp#Skin\\", L"KickStart.xur", NULL, &hKickScene );
			this->NavigateForward(hKickScene);	

			bHandled = TRUE;
			return S_OK;
		}		 
		else if (hObjPressed == m_ResetAmiga)
		{		
			uae_reset(0);
			mainloopdone = 1;
			return S_OK;
		}
		else if (hObjPressed == m_Back)
		{

			this->NavigateBack(hMainScene);
			bHandled = TRUE;
			return S_OK;

		}
		
 
		 
        bHandled = TRUE;
        return S_OK;
    }


    //----------------------------------------------------------------------------------
    // Performs initialization tasks - retreives controls.
    //----------------------------------------------------------------------------------
HRESULT CRomListScene::OnInit( XUIMessageInit* pInitData, BOOL& bHandled )
{
 
	char path_buffer[_MAX_PATH];
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];

	wchar_t floppyname[MAX_PATH];

	currDrive = 0;

    // Retrieve controls for later use.
    GetChildById( L"XuiButtonBack", &m_Back );
    GetChildById( L"XuiPlay", &m_PlayRom );
    //GetChildById( L"XuiMainMenu", &m_Back );		 
	GetChildById( L"XuiDriveList", &m_DriveList );
	GetChildById( L"XuiRomList", &m_RomList );
	GetChildById( L"XuiRomPreview", &m_PreviewImage );	 		 
	GetChildById( L"XuiButtonKick", &m_Kick );
	GetChildById( L"XuiCurrentDeviceText", &m_DeviceText);
	GetChildById( L"XuiResetAmiga", &m_ResetAmiga);
	GetChildById( L"XuiDiskDriveBG", &m_DiskDriveBG);
	GetChildById( L"XuDiskDriveLabel", &m_DiskDriveHeading);

	GetChildById( L"XuiButtonDF0", &m_InsertDiskDrive0);
	GetChildById( L"XuiChangeDrive", &m_SwitchDrive);

	GetChildById( L"XuiButtonEject", &m_DiskEject);


	GetChildById( L"XuiCurrentDeviceDrive1", &m_DeviceDrive1);
	GetChildById( L"XuiCurrentDeviceDrive2", &m_DeviceDrive2);
	GetChildById( L"XuiCurrentDeviceDrive3", &m_DeviceDrive3);
	GetChildById( L"XuiCurrentDeviceDrive4", &m_DeviceDrive4);
 

	m_RomList.DiscardResources(XUI_DISCARD_ALL);
	m_RomList.SetFocus();
	m_RomList.SetCurSel(0);
 
	m_DriveList.SetShow(FALSE);
	m_DriveList.SetEnable(FALSE);		
 
	m_DiskDriveBG.SetShow(FALSE);
	m_DiskDriveBG.SetEnable(FALSE);
 
	m_DiskDriveHeading.SetShow(FALSE);
	m_DiskDriveHeading.SetEnable(FALSE);
		 
	if (strlen(currprefs.floppyslots[0].df) == 0)
	{
		m_DeviceDrive1.SetText(L"DF0: Empty");
	}
	else
	{
		_splitpath( currprefs.floppyslots[0].df, drive, dir, fname, ext );
		swprintf_s(floppyname,L"DF0: %S%S",fname, ext);
		m_DeviceDrive1.SetText(floppyname);
	}

	if (strlen(currprefs.floppyslots[1].df) == 0)
	{
		m_DeviceDrive2.SetText(L"DF1: Empty");
	}
	else
	{
		_splitpath( currprefs.floppyslots[1].df, drive, dir, fname, ext );
		swprintf_s(floppyname,L"DF1: %S%S",fname, ext);
		m_DeviceDrive2.SetText(floppyname);
	}

	if (strlen(currprefs.floppyslots[2].df) == 0)
	{
		m_DeviceDrive3.SetText(L"DF2: Empty");
	}
	else
	{
		_splitpath( currprefs.floppyslots[2].df, drive, dir, fname, ext );
		swprintf_s(floppyname,L"DF2: %S%S",fname, ext);
		m_DeviceDrive3.SetText(floppyname);
	}

	if (strlen(currprefs.floppyslots[3].df) == 0)
	{
		m_DeviceDrive4.SetText(L"DF3: Empty");
	}
	else
	{
		_splitpath( currprefs.floppyslots[3].df, drive, dir, fname, ext );
		swprintf_s(floppyname,L"DF3: %S%S",fname, ext);
		m_DeviceDrive4.SetText(floppyname);
	}

	hRomListScene = this->m_hObj;

	bHandled = TRUE;
    return S_OK;
 }



CRomList::CRomList()
{
 

}
 

HRESULT CRomList::OnNotify( XUINotify *hObj, BOOL& bHandled )
{
 
	int nIndex = 0;
	switch(hObj->dwNotify)
	{
		case XN_SELCHANGED:
			 						 
			nIndex = XuiListGetCurSel( this->m_hObj, NULL );
			
		 
						 
			break;

	}

	
	return S_OK;

}
 

HRESULT CRomList::OnRescanRoms( char *szPath,  BOOL& bHandled )
{ 
	
 
	DeleteItems(0, m_ListData.size());
	 
	strcpy(szRoms, "GAME:\\ROMS\\");
	strcat(szRoms, "*.*");
  
	m_ListData.clear();

	HANDLE hFind;	
	WIN32_FIND_DATAA oFindData;

	hFind = FindFirstFile(szRoms, &oFindData);
	
	if (hFind != INVALID_HANDLE_VALUE)
	{
		do
		{					 
			m_ListData.push_back(_strlwr(oFindData.cFileName));			  

		} while (FindNextFile(hFind, &oFindData));

		std::sort(m_ListData.begin(),m_ListData.end());
		 	
	}
	 
	InsertItems( 0, m_ListData.size() );

	bHandled = TRUE;	
    return( S_OK );
}

HRESULT CRomList::OnInit(XUIMessageInit *pInitData, BOOL& bHandled)
{
 
	XUIMessage xuiMsg;
	InRescanRomsStruct msgData;
	InRescanRomsFirstFunc( &xuiMsg, &msgData, "GAME:\\ROMS\\" );
	XuiSendMessage( m_hObj, &xuiMsg );

	bHandled = TRUE;
    return S_OK;
}

HRESULT CRomList::OnGetItemCountAll(
        XUIMessageGetItemCount *pGetItemCountData, 
        BOOL& bHandled)
    {
        pGetItemCountData->cItems = m_ListData.size();
        bHandled = TRUE;
        return S_OK;
    }


// Gets called every frame
HRESULT CRomList::OnGetSourceDataText(
    XUIMessageGetSourceText *pGetSourceTextData, 
    BOOL& bHandled)
{
	
    
    if( ( 0 == pGetSourceTextData->iData ) && ( ( pGetSourceTextData->bItemData ) ) ) {

			LPCWSTR lpszwBuffer = MultiCharToUniChar((char *)m_ListData[pGetSourceTextData->iItem].c_str());

            pGetSourceTextData->szText = lpszwBuffer;

            bHandled = TRUE;
        }
        return S_OK;

}
 

LPCWSTR MultiCharToUniChar(char* mbString)
{
	int len = strlen(mbString) + 1;	
	mbstowcs(ucString, mbString, len);
	return (LPCWSTR)ucString;
}