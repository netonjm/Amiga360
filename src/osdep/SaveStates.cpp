#include <xmp.h>
#include "SaveStates.h"
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
#include "savestate.h"
}

extern int mainloopdone;

static int currDrive = 0;
char szSave[200]; 
char szSavePath[200];
std::vector<std::string> m_SaveListData;

extern HXUIOBJ hMainScene;
extern HXUIOBJ hRomListScene;
extern int mainloopdone;
extern CAmiga360App app;
extern "C" LPDIRECT3DDEVICE9 D3D_Device;
extern "C" struct uae_prefs currprefs, changed_prefs;
extern "C" void memory_reset (void);
extern LPCWSTR MultiCharToUniChar(char* mbString);

#define SAVE_PATH "GAME:\\SAVE\\"

void InRescanSaveFirstFunc(XUIMessage *pMsg, InRescanSaveStruct* pData, char *szPath)
{
    XuiMessage(pMsg,XM_MESSAGE_ON_RESCAN_SAVE);
    _XuiMessageExtra(pMsg,(XUIMessageData*) pData, sizeof(*pData));

	
}

#define TM_KEYBOARD 10001

// Handler for the XM_NOTIFY message
HRESULT CSaveListScene::OnNotifyPress( HXUIOBJ hObjPressed, 
       BOOL& bHandled )
    {
		int nIndex;
		wchar_t tmpFile[MAX_PATH];		 

        if ( hObjPressed == m_DeleteState )
        {
			char tmpPath[512];
			nIndex = m_SaveList.GetCurSel();
				
			if (nIndex >= 0)
			{
				LPCWSTR romName = m_SaveList.GetText(nIndex);
			 
				sprintf(tmpPath,"GAME:\\Save\\%S",romName);

				DeleteFile(tmpPath);

				XUIMessage xuiMsg;
				XUIMessageInit xuiMsgInit;
				XuiMessageInit( &xuiMsg, &xuiMsgInit, (void *)&m_SaveList );
				// send the XM_INIT message
				XuiSendMessage( m_SaveList.m_hObj, &xuiMsg );
			}

			bHandled = TRUE;
			return S_OK;
			
        }
		else if ( hObjPressed == m_SaveState)
		{
			
 
			nIndex = m_SaveList.GetCurSel();
			
			memset (result, 0x00, sizeof(result));

			if (nIndex >= 0)
			{
				
				memset(&overlapped,0,sizeof(overlapped));
				LPCWSTR romName = m_SaveList.GetText(nIndex);			 
				wcscpy_s(tmpName,255,romName);
				
				XShowKeyboardUI(0,VKBD_DEFAULT|VKBD_HIGHLIGHT_TEXT,tmpName,L"Enter a SaveState filename",L"Store your current SaveState",result,42,&overlapped);
				SetTimer(TM_KEYBOARD,50); 
				
			}

			bHandled = TRUE;

			return S_OK;

		}
		else if ( hObjPressed == m_LoadState)
		{
 			char tmpPath[512];
			nIndex = m_SaveList.GetCurSel();
				
			if (nIndex >= 0)
			{
				LPCWSTR romName = m_SaveList.GetText(nIndex);
			 
				sprintf(tmpPath,"GAME:\\Save\\%S",romName);
 
				mainloopdone = 1;

				savestate_initsave (tmpPath, 0,0,0);
			 
				savestate_state = STATE_DORESTORE;
 

				
			}
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
	

HRESULT CSaveListScene::OnTimer( XUIMessageTimer *pTimer, BOOL& bHandled )
{

	switch (pTimer->nId)
    {
		case TM_KEYBOARD:
        {
            if (XHasOverlappedIoCompleted(&overlapped))
            {
                    if (overlapped.dwExtendedError == ERROR_SUCCESS)
                    {

							char saveFile[255];

							sprintf(saveFile,"GAME:\\Save\\%S",result);
							savestate_initsave (saveFile, 0,0,0);
							save_state (saveFile, "e-uae");
							XUIMessage xuiMsg;
							XUIMessageInit xuiMsgInit;
							XuiMessageInit( &xuiMsg, &xuiMsgInit, (void *)&m_SaveList );
							// send the XM_INIT message
							XuiSendMessage( m_SaveList.m_hObj, &xuiMsg );

                    }
                    KillTimer(TM_KEYBOARD);

                    bHandled = TRUE;
            }
            break;
        }
	}

	return S_OK;
}

    //----------------------------------------------------------------------------------
    // Performs initialization tasks - retreives controls.
    //----------------------------------------------------------------------------------
HRESULT CSaveListScene::OnInit( XUIMessageInit* pInitData, BOOL& bHandled )
{
 
    // Retrieve controls for later use.
    GetChildById( L"XuiSaveState", &m_SaveState ); 
	GetChildById( L"XuiDeleteState", &m_DeleteState ); 
	GetChildById( L"XuiLoadState", &m_LoadState );
    GetChildById( L"XuiBack", &m_Back );		 
	GetChildById( L"XuiDriveList", &m_DriveList );
	GetChildById( L"XuiSaveList", &m_SaveList );
	 
	//GetChildById( L"XuiBackVideoRoms", &m_BackVideo );
	GetChildById( L"XuiCurrentDeviceText", &m_DeviceText);
	GetChildById( L"XuiResetAmiga", &m_ResetAmiga);
	GetChildById( L"XuiDiskDriveBG", &m_DiskDriveBG);
	GetChildById( L"XuDiskDriveLabel", &m_DiskDriveHeading);

	GetChildById( L"XuiButtonDF0", &m_InsertDiskDrive0);
	GetChildById( L"XuiChangeDrive", &m_SwitchDrive);
 

	//phObj = this->m_hObj;

	m_SaveList.DiscardResources(XUI_DISCARD_ALL);
	m_SaveList.SetFocus();
	m_SaveList.SetCurSel(0);
 
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



CSaveList::CSaveList()
{
 

}
 

HRESULT CSaveList::OnNotify( XUINotify *hObj, BOOL& bHandled )
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
 

HRESULT CSaveList::OnRescanSave( char *szPath,  BOOL& bHandled )
{ 
	
	DeleteItems(0, m_SaveListData.size());
	 
	strcpy(szSave, "GAME:\\SAVE\\");
	strcat(szSave, "*.*");
  
	m_SaveListData.clear();

	HANDLE hFind;	
	WIN32_FIND_DATAA oFindData;

	hFind = FindFirstFile(szSave, &oFindData);

	if (hFind != INVALID_HANDLE_VALUE)
	{
		do
		{		
			
			m_SaveListData.push_back(_strlwr(oFindData.cFileName));
			 

		} while (FindNextFile(hFind, &oFindData));

	
	
	}
	 
	InsertItems( 0, m_SaveListData.size() );

	bHandled = TRUE;	
    return( S_OK );
}

HRESULT CSaveList::OnInit(XUIMessageInit *pInitData, BOOL& bHandled)
{
 
	XUIMessage xuiMsg;
	InRescanSaveStruct msgData;
	InRescanSaveFirstFunc( &xuiMsg, &msgData, "GAME:\\Save\\" );
	XuiSendMessage( m_hObj, &xuiMsg );

	bHandled = TRUE;
    return S_OK;
}

HRESULT CSaveList::OnGetItemCountAll(
        XUIMessageGetItemCount *pGetItemCountData, 
        BOOL& bHandled)
    {
        pGetItemCountData->cItems = m_SaveListData.size();
        bHandled = TRUE;
        return S_OK;
    }


// Gets called every frame
HRESULT CSaveList::OnGetSourceDataText(
    XUIMessageGetSourceText *pGetSourceTextData, 
    BOOL& bHandled)
{
	
    
    if( ( 0 == pGetSourceTextData->iData ) && ( ( pGetSourceTextData->bItemData ) ) ) {

			LPCWSTR lpszwBuffer = MultiCharToUniChar((char *)m_SaveListData[pGetSourceTextData->iItem].c_str());

            pGetSourceTextData->szText = lpszwBuffer;

            bHandled = TRUE;
        }
        return S_OK;

}
