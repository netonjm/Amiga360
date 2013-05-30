#include <xtl.h>
#include <xui.h>
#include <xuiapp.h>
#include <algorithm>
#include <new>
#include <iostream>
#include <vector>


#ifndef OPTIONS_H
#define OPTIONS_H




class COptionsScene : public CXuiSceneImpl
{

protected:

	CXuiControl m_machineA500;
	CXuiControl m_machineA1000;
	CXuiControl m_machineA600;
	CXuiControl m_machineA1200;
	CXuiControl m_machineA4000;

	CXuiControl m_Kickstart11;
	CXuiControl m_Kickstart12;
	CXuiControl m_Kickstart13;
	CXuiControl m_Kickstart20;
	CXuiControl m_Kickstart31;

	CXuiControl m_CPU68000;
	CXuiControl m_CPU68010;
	CXuiControl m_CPU68020;
	CXuiControl m_CPU68020EC;
	CXuiControl m_CPU68040;
	CXuiControl m_CPU68060;

	CXuiControl m_ChipSetOCS;
	CXuiControl m_ChipSetECS_A;
	CXuiControl m_ChipSetECS_D;
	CXuiControl m_ChipSetECS;
	CXuiControl m_ChipSetAGA;

	CXuiControl m_DrivesOne;
	CXuiControl m_DrivesTwo;
	CXuiControl m_DrivesThree;
	CXuiControl m_DrivesFour;

	CXuiControl m_ChipMemory512;
	CXuiControl m_ChipMemory1024;
	CXuiControl m_ChipMemory2048;
	CXuiControl m_ChipMemory4096;	 
	CXuiControl m_ChipMemory8192;

	CXuiControl m_SlowMemoryNone;
	CXuiControl m_SlowMemory512;
	CXuiControl m_SlowMemory1024;
	CXuiControl m_SlowMemory1500;
	CXuiControl m_SlowMemory1800;


	CXuiControl m_FastMemoryNone;
	CXuiControl m_FastMemory1024;
	CXuiControl m_FastMemory2048;
	CXuiControl m_FastMemory4096;	 
	CXuiControl m_FastMemory8192;

	CXuiControl m_Joy1;
	CXuiControl m_Joy2;
	CXuiControl m_Joy3;


	CXuiRadioGroup m_GroupMachine;
	CXuiRadioGroup m_GroupKickstart;
	CXuiRadioGroup m_GroupCPU;
	CXuiRadioGroup m_GroupChipset;
	CXuiRadioGroup m_GroupDrives;
	CXuiRadioGroup m_GroupChipMem;
	CXuiRadioGroup m_GroupBogoMem;
	CXuiRadioGroup m_GroupFast;
	CXuiRadioGroup m_GroupZ3Fast;
	CXuiRadioGroup m_GroupRTG;
	CXuiRadioGroup m_GroupJoy;

	CXuiCheckbox m_FasterRTG; 
	CXuiCheckbox m_BSDSocket;
	CXuiCheckbox m_Picasso96;
	CXuiCheckbox m_DriveClick;
	CXuiCheckbox m_24BitAddressing;
	CXuiCheckbox m_ImmediateBlits;
	CXuiCheckbox m_CycleExact;
	CXuiCheckbox m_ShowLeds;
	 
	CXuiControl m_More;

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
    XUI_IMPLEMENT_CLASS( COptionsScene, L"OptionsScene", XUI_CLASS_SCENE )
};


#endif