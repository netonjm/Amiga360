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

#ifndef KICKLIST_H
#define KICKLIST_H

#define XM_MESSAGE_ON_RESCAN_KICKS  XM_USER



typedef struct
{
    char *szPath;    
}
InRescanKicksStruct;

void InRescanKicksFirstFunc(XUIMessage *pMsg, InRescanKicksStruct* pData, char *szPath);



// Define the message map macro
#define XUI_ON_XM_MESSAGE_ON_RESCAN_KICKS(MemberFunc)\
    if (pMessage->dwMessage == XM_MESSAGE_ON_RESCAN_KICKS)\
    {\
        InRescanKicksStruct *pData = (InRescanKicksStruct *) pMessage->pvData;\
        return MemberFunc(pData->szPath,  pMessage->bHandled);\
    }

class CKickList : CXuiListImpl
{
public:

	XUI_IMPLEMENT_CLASS(CKickList, L"KickList", XUI_CLASS_LIST);

	XUI_BEGIN_MSG_MAP()
		XUI_ON_XM_INIT(OnInit)
		XUI_ON_XM_GET_SOURCE_TEXT(OnGetSourceDataText)
		XUI_ON_XM_GET_ITEMCOUNT_ALL(OnGetItemCountAll)	
		XUI_ON_XM_NOTIFY( OnNotify )
		XUI_ON_XM_MESSAGE_ON_RESCAN_KICKS( OnRescanKicks )
	XUI_END_MSG_MAP()

	CKickList();
    HRESULT OnInit( XUIMessageInit* pInitData, BOOL& bHandled );
	HRESULT OnNotify( XUINotify *hObj, BOOL& bHandled );
	HRESULT OnGetSourceDataText(XUIMessageGetSourceText *pGetSourceTextData, BOOL& bHandled);
	HRESULT OnGetItemCountAll(XUIMessageGetItemCount *pGetItemCountData, BOOL& bHandled);
	HRESULT OnRescanKicks ( char *szPath, BOOL& bHandled );
	 
};


//--------------------------------------------------------------------------------------
// Scene implementation class.
//--------------------------------------------------------------------------------------
class CKickListScene : public CXuiSceneImpl
{

protected:

    // Control and Element wrapper objects.
    CXuiControl m_AddToFavorites;
    CXuiControl m_PlayKick;
	CXuiControl m_Back; 
	CXuiElement m_PreviewImage;
	CXuiList m_KickList;
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
   

    // Message map.
    XUI_BEGIN_MSG_MAP()
		XUI_ON_XM_INIT( OnInit )
		XUI_ON_XM_NOTIFY_PRESS( OnNotifyPress )
    XUI_END_MSG_MAP()

	


 
public:
    HRESULT OnInit( XUIMessageInit* pInitData, BOOL& bHandled );
	HRESULT OnNotifyPress( HXUIOBJ hObjPressed, BOOL& bHandled );
 
public:

    // Define the class. The class name must match the ClassOverride property
    // set for the scene in the UI Authoring tool.
    XUI_IMPLEMENT_CLASS( CKickListScene, L"KickListScene", XUI_CLASS_SCENE )
};


#endif