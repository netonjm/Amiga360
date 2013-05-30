#include <xmp.h>
#include "KickStart.h"
#include "MainApp.h"
#include "XUIDefines.h"
 
#include <SDL.h>
#include <SDL_endian.h>

extern "C"
{
#include "options.h"
#include "gui.h"
#include "uae.h"
#include "disk.h"
#include "memory.h"
}

extern int mainloopdone;

static int currDrive = 0;
char szKick[200]; 
char szKickPath[200];
std::vector<std::string> m_KickListData;

extern HXUIOBJ hRomListScene;
extern HXUIOBJ hMainScene;
extern int mainloopdone;
extern CAmiga360App app;
extern "C" LPDIRECT3DDEVICE9 D3D_Device;
extern "C" struct uae_prefs currprefs, changed_prefs;
extern "C" void memory_reset (void);
extern LPCWSTR MultiCharToUniChar(char* mbString);

void InRescanKicksFirstFunc(XUIMessage *pMsg, InRescanKicksStruct* pData, char *szPath)
{
    XuiMessage(pMsg,XM_MESSAGE_ON_RESCAN_KICKS);
    _XuiMessageExtra(pMsg,(XUIMessageData*) pData, sizeof(*pData));

	
}

// Handler for the XM_NOTIFY message
HRESULT CKickListScene::OnNotifyPress( HXUIOBJ hObjPressed, 
       BOOL& bHandled )
    {
		int nIndex;
		wchar_t tmpFile[MAX_PATH];

        if ( hObjPressed == m_KickList )
        {
			nIndex = m_KickList.GetCurSel(); 
			
			sprintf(changed_prefs.romfile, "GAME:\\KickStart\\%s", (char *)m_KickListData[nIndex].c_str());
			memory_reset();
			uae_reset(1);
			
			
		    config_changed = 1;
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
		else if (hObjPressed == m_InsertDiskDrive0)
		{
			nIndex = m_KickList.GetCurSel(); 			 			 
		 				  
 
			//swprintf_s(tmpFile, L"DF0 : %S", (char *)m_ListData[nIndex].c_str());
 
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
HRESULT CKickListScene::OnInit( XUIMessageInit* pInitData, BOOL& bHandled )
{
 
    // Retrieve controls for later use.
    GetChildById( L"XuiAddToFavorites", &m_AddToFavorites ); 
    GetChildById( L"XuiMainMenu", &m_Back );		 
	GetChildById( L"XuiDriveList", &m_DriveList );
	GetChildById( L"XuiKickList", &m_KickList );
	 
	GetChildById( L"XuiBackVideoRoms", &m_BackVideo );
	GetChildById( L"XuiCurrentDeviceText", &m_DeviceText);
	GetChildById( L"XuiResetAmiga", &m_ResetAmiga);
	GetChildById( L"XuiDiskDriveBG", &m_DiskDriveBG);
	GetChildById( L"XuDiskDriveLabel", &m_DiskDriveHeading);

	GetChildById( L"XuiButtonDF0", &m_InsertDiskDrive0);
	GetChildById( L"XuiChangeDrive", &m_SwitchDrive);
 

	//phObj = this->m_hObj;

	m_KickList.DiscardResources(XUI_DISCARD_ALL);
	m_KickList.SetFocus();
	m_KickList.SetCurSel(0);
 
	//swprintf_s(DeviceText, L"Current Device : %S", szRomPath);
	//m_DeviceText.SetText(DeviceText);

	//BuildFavorites(); 

	//m_DriveList.EnableInput(FALSE);
	m_DriveList.SetShow(FALSE);
	m_DriveList.SetEnable(FALSE);		
		
	//m_DiskDriveBG.EnableInput(FALSE);
	m_DiskDriveBG.SetShow(FALSE);
	m_DiskDriveBG.SetEnable(FALSE);

	//m_DiskDriveHeading.EnableInput(FALSE);
	m_DiskDriveHeading.SetShow(FALSE);
	m_DiskDriveHeading.SetEnable(FALSE);


	 
		
	bHandled = TRUE;
    return S_OK;
 }



CKickList::CKickList()
{
 

}
 

HRESULT CKickList::OnNotify( XUINotify *hObj, BOOL& bHandled )
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
 

HRESULT CKickList::OnRescanKicks( char *szPath,  BOOL& bHandled )
{ 
	
	DeleteItems(0, m_KickListData.size());
	 
	strcpy(szKick, "GAME:\\KickStart\\");
	strcat(szKick, "*.rom");
  
	m_KickListData.clear();

	HANDLE hFind;	
	WIN32_FIND_DATAA oFindData;

	hFind = FindFirstFile(szKick, &oFindData);

	if (hFind != INVALID_HANDLE_VALUE)
	{
		do
		{		
			
			m_KickListData.push_back(_strlwr(oFindData.cFileName));
			 

		} while (FindNextFile(hFind, &oFindData));

	
	
	}
	 
	InsertItems( 0, m_KickListData.size() );

	bHandled = TRUE;	
    return( S_OK );
}

HRESULT CKickList::OnInit(XUIMessageInit *pInitData, BOOL& bHandled)
{
 
	XUIMessage xuiMsg;
	InRescanKicksStruct msgData;
	InRescanKicksFirstFunc( &xuiMsg, &msgData, "GAME:\\KickStart\\" );
	XuiSendMessage( m_hObj, &xuiMsg );

	bHandled = TRUE;
    return S_OK;
}

HRESULT CKickList::OnGetItemCountAll(
        XUIMessageGetItemCount *pGetItemCountData, 
        BOOL& bHandled)
    {
        pGetItemCountData->cItems = m_KickListData.size();
        bHandled = TRUE;
        return S_OK;
    }


// Gets called every frame
HRESULT CKickList::OnGetSourceDataText(
    XUIMessageGetSourceText *pGetSourceTextData, 
    BOOL& bHandled)
{
	
    
    if( ( 0 == pGetSourceTextData->iData ) && ( ( pGetSourceTextData->bItemData ) ) ) {

			LPCWSTR lpszwBuffer = MultiCharToUniChar((char *)m_KickListData[pGetSourceTextData->iItem].c_str());

            pGetSourceTextData->szText = lpszwBuffer;

            bHandled = TRUE;
        }
        return S_OK;

}
