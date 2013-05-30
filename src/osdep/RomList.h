#include <xtl.h>
#include <xui.h>
#include <xuiapp.h>


extern "C"
{
#include "sysconfig.h"
#include "sysdeps.h"
#include "savestate.h"
#include "uae.h"

}

#ifndef ROMLIST_H
#define ROMLIST_H

#define XM_MESSAGE_ON_RESCAN_ROMS  XM_USER



typedef struct
{
    char *szPath;    
}
InRescanRomsStruct;

void InRescanRomsFirstFunc(XUIMessage *pMsg, InRescanRomsStruct* pData, char *szPath);



// Define the message map macro
#define XUI_ON_XM_MESSAGE_ON_RESCAN_ROMS(MemberFunc)\
    if (pMessage->dwMessage == XM_MESSAGE_ON_RESCAN_ROMS)\
    {\
        InRescanRomsStruct *pData = (InRescanRomsStruct *) pMessage->pvData;\
        return MemberFunc(pData->szPath,  pMessage->bHandled);\
    }

class CRomList : CXuiListImpl
{
public:

	XUI_IMPLEMENT_CLASS(CRomList, L"SnesRomList", XUI_CLASS_LIST);

	XUI_BEGIN_MSG_MAP()
		XUI_ON_XM_INIT(OnInit)
		XUI_ON_XM_GET_SOURCE_TEXT(OnGetSourceDataText)
		XUI_ON_XM_GET_ITEMCOUNT_ALL(OnGetItemCountAll)	
		XUI_ON_XM_NOTIFY( OnNotify )
		XUI_ON_XM_MESSAGE_ON_RESCAN_ROMS( OnRescanRoms )
	XUI_END_MSG_MAP()

	CRomList();
    HRESULT OnInit( XUIMessageInit* pInitData, BOOL& bHandled );
	HRESULT OnNotify( XUINotify *hObj, BOOL& bHandled );
	HRESULT OnGetSourceDataText(XUIMessageGetSourceText *pGetSourceTextData, BOOL& bHandled);
	HRESULT OnGetItemCountAll(XUIMessageGetItemCount *pGetItemCountData, BOOL& bHandled);
	HRESULT OnRescanRoms ( char *szPath, BOOL& bHandled );
	 
};


//--------------------------------------------------------------------------------------
// Scene implementation class.
//--------------------------------------------------------------------------------------
class CRomListScene : public CXuiSceneImpl
{

protected:

    // Control and Element wrapper objects.
    CXuiControl m_Back;
    CXuiControl m_PlayRom;	 
	CXuiElement m_PreviewImage;
	CXuiList m_RomList;
	CXuiControl   m_Kick;
	CXuiTextElement m_DeviceText;
	CXuiControl m_ResetAmiga;
	CXuiList m_DriveList;
	
	CXuiControl m_DiskDriveBG;
	CXuiControl m_DiskDriveHeading;
	CXuiControl m_InsertDiskDrive0; 
	CXuiControl m_SwitchDrive; 
	CXuiControl m_DiskEject; 


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
    XUI_IMPLEMENT_CLASS( CRomListScene, L"RomListScene", XUI_CLASS_SCENE )
};


#endif