#include <xtl.h>
#include <xui.h>
#include <xuiapp.h>
#include <algorithm>
#include <new>
#include <iostream>
#include <vector>

extern "C"
{
#include "sysconfig.h"
#include "sysdeps.h" 
#include "uae.h"

}

#ifndef SAVELIST_H
#define SAVELIST_H

#define XM_MESSAGE_ON_RESCAN_SAVE  XM_USER



typedef struct
{
    char *szPath;    
}
InRescanSaveStruct;

void InRescanSaveFirstFunc(XUIMessage *pMsg, InRescanSaveStruct* pData, char *szPath);



// Define the message map macro
#define XUI_ON_XM_MESSAGE_ON_RESCAN_SAVE(MemberFunc)\
    if (pMessage->dwMessage == XM_MESSAGE_ON_RESCAN_SAVE)\
    {\
        InRescanSaveStruct *pData = (InRescanSaveStruct *) pMessage->pvData;\
        return MemberFunc(pData->szPath,  pMessage->bHandled);\
    }

class CSaveList : CXuiListImpl
{
public:

	XUI_IMPLEMENT_CLASS(CSaveList, L"SaveList", XUI_CLASS_LIST);

	XUI_BEGIN_MSG_MAP()
		XUI_ON_XM_INIT(OnInit)
		XUI_ON_XM_GET_SOURCE_TEXT(OnGetSourceDataText)
		XUI_ON_XM_GET_ITEMCOUNT_ALL(OnGetItemCountAll)	
		XUI_ON_XM_NOTIFY( OnNotify )
		XUI_ON_XM_MESSAGE_ON_RESCAN_SAVE( OnRescanSave )
	XUI_END_MSG_MAP()

	CSaveList();
    HRESULT OnInit( XUIMessageInit* pInitData, BOOL& bHandled );
	HRESULT OnNotify( XUINotify *hObj, BOOL& bHandled );
	HRESULT OnGetSourceDataText(XUIMessageGetSourceText *pGetSourceTextData, BOOL& bHandled);
	HRESULT OnGetItemCountAll(XUIMessageGetItemCount *pGetItemCountData, BOOL& bHandled);
	HRESULT OnRescanSave ( char *szPath, BOOL& bHandled );
	 
};


//--------------------------------------------------------------------------------------
// Scene implementation class.
//--------------------------------------------------------------------------------------
class CSaveListScene : public CXuiSceneImpl
{

protected:

    // Control and Element wrapper objects.
    CXuiControl m_LoadState;
    CXuiControl m_SaveState;
	CXuiControl m_DeleteState;
	CXuiControl m_Back; 
	CXuiElement m_PreviewImage;
	CXuiList m_SaveList;
	CXuiVideo   m_BackVideo;
	CXuiTextElement m_DeviceText;
	CXuiControl m_ResetAmiga;
	CXuiList m_DriveList;
	
	CXuiControl m_DiskDriveBG;
	CXuiControl m_DiskDriveHeading;
	CXuiControl m_InsertDiskDrive0; 
	CXuiControl m_SwitchDrive; 
	

	CXuiTextElement m_DeviceDrive1;
	CXuiTextElement m_DeviceDrive2;
	CXuiTextElement m_DeviceDrive3;
	CXuiTextElement m_DeviceDrive4;


	WCHAR result[42];
	WCHAR tmpName[255];     
	XOVERLAPPED overlapped;
   

    // Message map.
    XUI_BEGIN_MSG_MAP()
		XUI_ON_XM_INIT( OnInit )
		XUI_ON_XM_NOTIFY_PRESS( OnNotifyPress )
		XUI_ON_XM_TIMER( OnTimer )
    XUI_END_MSG_MAP()

	


 
public:
    HRESULT OnInit( XUIMessageInit* pInitData, BOOL& bHandled );
	HRESULT OnNotifyPress( HXUIOBJ hObjPressed, BOOL& bHandled );
	HRESULT OnTimer( XUIMessageTimer *pTimer, BOOL& bHandled );
 
public:

    // Define the class. The class name must match the ClassOverride property
    // set for the scene in the UI Authoring tool.
    XUI_IMPLEMENT_CLASS( CSaveListScene, L"SaveListScene", XUI_CLASS_SCENE )
};


#endif