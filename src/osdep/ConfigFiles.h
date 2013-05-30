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

#ifndef CONFIGLIST_H
#define CONFIGLIST_H

#define XM_MESSAGE_ON_RESCAN_CONFIG  XM_USER



typedef struct
{
    char *szPath;    
}
InRescanConfigStruct;

void InRescanConfigFirstFunc(XUIMessage *pMsg, InRescanConfigStruct* pData, char *szPath);



// Define the message map macro
#define XUI_ON_XM_MESSAGE_ON_RESCAN_CONFIG(MemberFunc)\
    if (pMessage->dwMessage == XM_MESSAGE_ON_RESCAN_CONFIG)\
    {\
        InRescanConfigStruct *pData = (InRescanConfigStruct *) pMessage->pvData;\
        return MemberFunc(pData->szPath,  pMessage->bHandled);\
    }

class CConfigList : CXuiListImpl
{
public:

	XUI_IMPLEMENT_CLASS(CConfigList, L"ConfigList", XUI_CLASS_LIST);

	XUI_BEGIN_MSG_MAP()
		XUI_ON_XM_INIT(OnInit)
		XUI_ON_XM_GET_SOURCE_TEXT(OnGetSourceDataText)
		XUI_ON_XM_GET_ITEMCOUNT_ALL(OnGetItemCountAll)	
		XUI_ON_XM_NOTIFY( OnNotify )
		XUI_ON_XM_MESSAGE_ON_RESCAN_CONFIG( OnRescanConfig )		
	XUI_END_MSG_MAP()

	CConfigList();
    HRESULT OnInit( XUIMessageInit* pInitData, BOOL& bHandled );
	HRESULT OnNotify( XUINotify *hObj, BOOL& bHandled );
	HRESULT OnGetSourceDataText(XUIMessageGetSourceText *pGetSourceTextData, BOOL& bHandled);
	HRESULT OnGetItemCountAll(XUIMessageGetItemCount *pGetItemCountData, BOOL& bHandled);
	HRESULT OnRescanConfig ( char *szPath, BOOL& bHandled );	
	 
};


//--------------------------------------------------------------------------------------
// Scene implementation class.
//--------------------------------------------------------------------------------------
class CConfigListScene : public CXuiSceneImpl
{

protected:

	WCHAR result[42];
	WCHAR tmpName[255];

    // Control and Element wrapper objects.
    CXuiControl m_LoadConfig;
    CXuiControl m_SaveConfig;
	CXuiControl m_DeleteConfig;
	CXuiControl m_Back; 
 
	CXuiList m_ConfigList;


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
    XUI_IMPLEMENT_CLASS( CConfigListScene, L"ConfigListScene", XUI_CLASS_SCENE )
};


#endif