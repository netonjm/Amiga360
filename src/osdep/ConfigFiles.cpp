#include <xmp.h>
#include "ConfigFiles.h"
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
char szConfig[200]; 
std::vector<std::string> m_ConfigListData;

extern HXUIOBJ hMainScene;
extern HXUIOBJ hRomListScene;
extern int mainloopdone;
extern CAmiga360App app;
extern "C" LPDIRECT3DDEVICE9 D3D_Device;
extern "C" struct uae_prefs currprefs, changed_prefs;
extern "C" void memory_reset (void);
extern "C" int cfgfile_load (struct uae_prefs *p, const TCHAR *filename, int *type, int ignorelink, int userconfig);
extern "C" void unmount_all_hdfs(void);
extern "C" void mount_hdf(int index, char* name);
extern "C" int kill_filesys_unitconfig (struct uae_prefs *p, int nr);
extern "C" int cfgfile_save (struct uae_prefs *p, const TCHAR *filename, int type);
extern LPCWSTR MultiCharToUniChar(char* mbString);
extern int doDriveClick;

#define CONFIG_PATH "GAME:\\CONFIG\\"

#define TM_KEYBOARD 10001

void InRescanConfigFirstFunc(XUIMessage *pMsg, InRescanConfigStruct* pData, char *szPath)
{
    XuiMessage(pMsg,XM_MESSAGE_ON_RESCAN_CONFIG);
    _XuiMessageExtra(pMsg,(XUIMessageData*) pData, sizeof(*pData));	
}

// Handler for the XM_NOTIFY message
HRESULT CConfigListScene::OnNotifyPress( HXUIOBJ hObjPressed, 
       BOOL& bHandled )
    {
		int nIndex;
		wchar_t tmpFile[MAX_PATH];

        if ( hObjPressed == m_DeleteConfig )
        {
			char tmpPath[512];
			nIndex = m_ConfigList.GetCurSel();
				
			if (nIndex >= 0)
			{
				LPCWSTR romName = m_ConfigList.GetText(nIndex);
			 
				sprintf(tmpPath,"GAME:\\Config\\%S",romName);
				
				DeleteFile(tmpPath);

				XUIMessage xuiMsg;
				XUIMessageInit xuiMsgInit;
				XuiMessageInit( &xuiMsg, &xuiMsgInit, (void *)&m_ConfigList );
				// send the XM_INIT message
				XuiSendMessage( m_ConfigList.m_hObj, &xuiMsg );

				m_ConfigList.SetFocus();
				m_ConfigList.SetCurSel(0);
			}
			
			bHandled = TRUE;
			return S_OK;
			
        }
		else if ( hObjPressed == m_SaveConfig)
		{
			nIndex = m_ConfigList.GetCurSel();
			
			memset (result, 0x00, sizeof(result));

			if (nIndex >= 0)
			{
				
				memset(&overlapped,0,sizeof(overlapped));
				LPCWSTR romName = m_ConfigList.GetText(nIndex);			 
				wcscpy_s(tmpName,255,romName);
				
				XShowKeyboardUI(0,VKBD_DEFAULT|VKBD_HIGHLIGHT_TEXT,tmpName,L"Enter a config filename",L"Store your current configuration",result,42,&overlapped);
				SetTimer(TM_KEYBOARD,50); 
				
			}

			bHandled = TRUE;
			 
		}
		else if ( hObjPressed == m_LoadConfig)
		{
 			char tmpPath[512];
			nIndex = m_ConfigList.GetCurSel();
				
			if (nIndex >= 0)
			{
				LPCWSTR romName = m_ConfigList.GetText(nIndex);
			 
				sprintf(tmpPath,"GAME:\\Config\\%S",romName);
 
				int type2 = CONFIG_TYPE_HARDWARE;			 
				default_prefs(& changed_prefs, 0);
				changed_prefs.chipmem_size = 0;
				changed_prefs.bogomem_size = 0;
				unmount_all_hdfs();
				int ret = cfgfile_load (& changed_prefs, tmpPath, &type2, 1, 0);
				if (ret) {
					mainloopdone = 1;

					check_prefs_changed_cpu();
					check_prefs_changed_custom ();
					check_prefs_changed_audio ();	 
					check_prefs_changed_gfx();					
					uae_restart(0,tmpPath);
					
				}				  				
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
	

    //----------------------------------------------------------------------------------
    // Performs initialization tasks - retreives controls.
    //----------------------------------------------------------------------------------
HRESULT CConfigListScene::OnInit( XUIMessageInit* pInitData, BOOL& bHandled )
{
 
    // Retrieve controls for later use.
    GetChildById( L"XuiSaveConfig", &m_SaveConfig ); 
	GetChildById( L"XuiDeleteConfig", &m_DeleteConfig ); 
	GetChildById( L"XuiLoadConfig", &m_LoadConfig );
	GetChildById( L"XuiConfigList", &m_ConfigList );
    GetChildById( L"XuiBack", &m_Back );		 
 
	//phObj = this->m_hObj;

	m_ConfigList.DiscardResources(XUI_DISCARD_ALL);
	m_ConfigList.SetFocus();
	m_ConfigList.SetCurSel(0);
 
 
	bHandled = TRUE;
    return S_OK;
 }



CConfigList::CConfigList()
{
 

}
 

HRESULT CConfigList::OnNotify( XUINotify *hObj, BOOL& bHandled )
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
 
HRESULT CConfigListScene::OnTimer( XUIMessageTimer *pTimer, BOOL& bHandled )
{

	switch (pTimer->nId)
    {
		case TM_KEYBOARD:
        {
            if (XHasOverlappedIoCompleted(&overlapped))
            {
                    if (overlapped.dwExtendedError == ERROR_SUCCESS)
                    {

							char configFile[255];

							sprintf(configFile,"GAME:\\Config\\%S",result);
							cfgfile_save(&currprefs,configFile,0);
							XUIMessage xuiMsg;
							XUIMessageInit xuiMsgInit;
							XuiMessageInit( &xuiMsg, &xuiMsgInit, (void *)&m_ConfigList );
							// send the XM_INIT message
							XuiSendMessage( m_ConfigList.m_hObj, &xuiMsg );

                    }
                    KillTimer(TM_KEYBOARD);

                    bHandled = TRUE;
            }
            break;
        }
	}

	return S_OK;
}

HRESULT CConfigList::OnRescanConfig( char *szPath,  BOOL& bHandled )
{ 
	
	DeleteItems(0, m_ConfigListData.size());
	 
	strcpy(szConfig, "GAME:\\CONFIG\\");
	strcat(szConfig, "*.*");
  
	m_ConfigListData.clear();

	HANDLE hFind;	
	WIN32_FIND_DATAA oFindData;

	hFind = FindFirstFile(szConfig, &oFindData);

	if (hFind != INVALID_HANDLE_VALUE)
	{
		do
		{					
			m_ConfigListData.push_back((oFindData.cFileName));			 

		} while (FindNextFile(hFind, &oFindData));

		std::sort(m_ConfigListData.begin(),m_ConfigListData.end());

	}
		
	InsertItems( 0, m_ConfigListData.size() );



	bHandled = TRUE;	
    return( S_OK );
}

HRESULT CConfigList::OnInit(XUIMessageInit *pInitData, BOOL& bHandled)
{
 
	XUIMessage xuiMsg;
	InRescanConfigStruct msgData;
	InRescanConfigFirstFunc( &xuiMsg, &msgData, "GAME:\\Config\\" );
	XuiSendMessage( m_hObj, &xuiMsg );

	bHandled = TRUE;
    return S_OK;
}

HRESULT CConfigList::OnGetItemCountAll(
        XUIMessageGetItemCount *pGetItemCountData, 
        BOOL& bHandled)
    {
        pGetItemCountData->cItems = m_ConfigListData.size();
        bHandled = TRUE;
        return S_OK;
    }


// Gets called every frame
HRESULT CConfigList::OnGetSourceDataText(
    XUIMessageGetSourceText *pGetSourceTextData, 
    BOOL& bHandled)
{
	
    
    if( ( 0 == pGetSourceTextData->iData ) && ( ( pGetSourceTextData->bItemData ) ) ) {

			LPCWSTR lpszwBuffer = MultiCharToUniChar((char *)m_ConfigListData[pGetSourceTextData->iItem].c_str());

            pGetSourceTextData->szText = lpszwBuffer;

            bHandled = TRUE;
        }
        return S_OK;

}
