#include <xtl.h>
#include "MainApp.h"
#include "HDDList.h"
 
 
#include <SDL.h>
#include <SDL_endian.h>


extern "C"
{
#include "options.h"
#include "gui.h"
#include "uae.h"
#include "disk.h"
}

HXUIOBJ hHDDListScene;

extern int mainloopdone;

static int currDrive = 0;
char szHDD[200]; 
char szHDDPath[200];

LPCWSTR MultiCharToUniChar(char* mbString);

extern HXUIOBJ hMainScene;
extern int mainloopdone;
extern CAmiga360App app;
extern "C" LPDIRECT3DDEVICE9 D3D_Device;
extern "C" struct uae_prefs currprefs, changed_prefs;

std::vector<std::string> m_HDDData;

extern "C" void mount_hdf(int index, char* name);
extern "C" int kill_filesys_unitconfig (struct uae_prefs *p, int nr);

void InRescanHDDFirstFunc(XUIMessage *pMsg, InRescanHDDStruct* pData, char *szPath)
{
    XuiMessage(pMsg,XM_MESSAGE_ON_RESCAN_HDD);
    _XuiMessageExtra(pMsg,(XUIMessageData*) pData, sizeof(*pData));

	
}

// Handler for the XM_NOTIFY message
HRESULT CHDDListScene::OnNotifyPress( HXUIOBJ hObjPressed, 
       BOOL& bHandled )
    {
		int nIndex;
		wchar_t tmpFile[MAX_PATH];

        if ( hObjPressed == m_HDDList )
        {
			nIndex = m_HDDList.GetCurSel(); 
			 									 
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
				swprintf_s(tmpFile, L"DH0 : Empty");
				m_DeviceDrive1.SetText(tmpFile);			 
				break;
			case 1:
				swprintf_s(tmpFile, L"DH1 : Empty");
				m_DeviceDrive2.SetText(tmpFile);			 
				break;
			case 2:
				swprintf_s(tmpFile, L"DH2 : Empty");
				m_DeviceDrive3.SetText(tmpFile);			 
				break;
			case 3:
				swprintf_s(tmpFile, L"DH3 : Empty");
				m_DeviceDrive4.SetText(tmpFile);			 
				break;
		 
			}

			kill_filesys_unitconfig(&currprefs, currDrive+1);

			bHandled = TRUE;
			return S_OK;

		}
		else if (hObjPressed == m_InsertDiskDrive0)
		{
			nIndex = m_HDDList.GetCurSel(); 			 			 
		 				  
			switch (currDrive)
			{
			case 0:
				swprintf_s(tmpFile, L"DH0 : %S", (char *)m_HDDData[nIndex].c_str());
				m_DeviceDrive1.SetText(tmpFile);			 
				break;
			case 1:
				swprintf_s(tmpFile, L"DH1 : %S", (char *)m_HDDData[nIndex].c_str());
				m_DeviceDrive2.SetText(tmpFile);			 
				break;
			case 2:
				swprintf_s(tmpFile, L"DH2 : %S", (char *)m_HDDData[nIndex].c_str());
				m_DeviceDrive3.SetText(tmpFile);			 
				break;
			case 3:
				swprintf_s(tmpFile, L"DH3 : %S", (char *)m_HDDData[nIndex].c_str());
				m_DeviceDrive4.SetText(tmpFile);			 
				break;
			}
			
			

			strcpy(szHDD,"GAME:\\Hardfiles\\");
			strcat(szHDD,(char *)m_HDDData[nIndex].c_str());


			mount_hdf(0,szHDD);

			//config_changed = 1;
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
				swprintf_s(tmpFile, L"Current Drive : DH0");
				break;
			case 1:
				swprintf_s(tmpFile, L"Current Drive : DH1");
				break;
			case 2:
				swprintf_s(tmpFile, L"Current Drive : DH2");
				break;
			case 3:
				swprintf_s(tmpFile, L"Current Drive : DH3");
				break;
			}

			m_SwitchDrive.SetText(tmpFile);


			bHandled = TRUE;
			return S_OK;

			
		}
		 	 
		else if (hObjPressed == m_ResetAmiga)
		{		
			uae_reset(1);
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
HRESULT CHDDListScene::OnInit( XUIMessageInit* pInitData, BOOL& bHandled )
{

	char path_buffer[_MAX_PATH];
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];

	wchar_t hddname[MAX_PATH];

	currDrive = 0;
 
    // Retrieve controls for later use.
    GetChildById( L"XuiButtonBack", &m_Back );
    GetChildById( L"XuiPlay", &m_PlayHDD );
    //GetChildById( L"XuiMainMenu", &m_Back );		 
	GetChildById( L"XuiDriveList", &m_DriveList );
	GetChildById( L"XuiHDDList", &m_HDDList );
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
 

	m_HDDList.DiscardResources(XUI_DISCARD_ALL);
	m_HDDList.SetFocus();
	m_HDDList.SetCurSel(0);
 
	m_DriveList.SetShow(FALSE);
	m_DriveList.SetEnable(FALSE);		
 
	m_DiskDriveBG.SetShow(FALSE);
	m_DiskDriveBG.SetEnable(FALSE);
 
	m_DiskDriveHeading.SetShow(FALSE);
	m_DiskDriveHeading.SetEnable(FALSE);

	 
	if (strlen(currprefs.mountconfig[0].volname) == 0)
	{
		m_DeviceDrive1.SetText(L"DH0: Empty");
	}
	else
	{
		_splitpath( currprefs.mountconfig[0].volname, drive, dir, fname, ext );
		swprintf_s(hddname,L"DH0: %S%S",fname, ext);
		m_DeviceDrive1.SetText(hddname);
	}

	if (strlen(currprefs.floppyslots[1].df) == 0)
	{
		m_DeviceDrive2.SetText(L"DH1: Empty");
	}
	else
	{
		_splitpath( currprefs.mountconfig[1].devname, drive, dir, fname, ext );
		swprintf_s(hddname,L"DH1: %S%S",fname, ext);
		m_DeviceDrive2.SetText(hddname);
	}

	if (strlen(currprefs.floppyslots[2].df) == 0)
	{
		m_DeviceDrive3.SetText(L"DH2: Empty");
	}
	else
	{
		_splitpath( currprefs.mountconfig[2].devname, drive, dir, fname, ext );
		swprintf_s(hddname,L"DH2: %S%S",fname, ext);
		m_DeviceDrive3.SetText(hddname);
	}

	if (strlen(currprefs.floppyslots[3].df) == 0)
	{
		m_DeviceDrive4.SetText(L"DH3: Empty");
	}
	else
	{
		_splitpath( currprefs.mountconfig[3].devname, drive, dir, fname, ext );
		swprintf_s(hddname,L"DH3: %S%S",fname, ext);
		m_DeviceDrive4.SetText(hddname);
	}


	hHDDListScene = this->m_hObj;

	bHandled = TRUE;
    return S_OK;
 }



CHDDList::CHDDList()
{
 

}
 

HRESULT CHDDList::OnNotify( XUINotify *hObj, BOOL& bHandled )
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
 

HRESULT CHDDList::OnRescanHDD( char *szPath,  BOOL& bHandled )
{ 
 
	DeleteItems(0, m_HDDData.size());
	 
	strcpy(szHDD, "GAME:\\Hardfiles\\");
	strcat(szHDD, "*.hdf");
  
	m_HDDData.clear();

	HANDLE hFind;	
	WIN32_FIND_DATAA oFindData;

	hFind = FindFirstFile(szHDD, &oFindData);
	
	if (hFind != INVALID_HANDLE_VALUE)
	{
		do
		{		
			m_HDDData.push_back(_strlwr(oFindData.cFileName));
		} while (FindNextFile(hFind, &oFindData));

		std::sort(m_HDDData.begin(),m_HDDData.end());		 	
	}
	 
	InsertItems( 0, m_HDDData.size() );

	bHandled = TRUE;	
    return( S_OK );
}

HRESULT CHDDList::OnInit(XUIMessageInit *pInitData, BOOL& bHandled)
{
 
	XUIMessage xuiMsg;
	InRescanHDDStruct msgData;
	InRescanHDDFirstFunc( &xuiMsg, &msgData, "GAME:\\Hardfiles\\" );
	XuiSendMessage( m_hObj, &xuiMsg );

	bHandled = TRUE;
    return S_OK;
}

HRESULT CHDDList::OnGetItemCountAll(
        XUIMessageGetItemCount *pGetItemCountData, 
        BOOL& bHandled)
    {
        pGetItemCountData->cItems = m_HDDData.size();
        bHandled = TRUE;
        return S_OK;
    }


// Gets called every frame
HRESULT CHDDList::OnGetSourceDataText(
    XUIMessageGetSourceText *pGetSourceTextData, 
    BOOL& bHandled)
{
	
    
    if( ( 0 == pGetSourceTextData->iData ) && ( ( pGetSourceTextData->bItemData ) ) ) {

			LPCWSTR lpszwBuffer = MultiCharToUniChar((char *)m_HDDData[pGetSourceTextData->iItem].c_str());

            pGetSourceTextData->szText = lpszwBuffer;

            bHandled = TRUE;
        }
        return S_OK;

}
 