#include <xmp.h>
#include "AmigaOptions.h"
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
#include "inputdevice.h"
#include "custom.h"
#include "driveclick.h"
}

extern "C"  struct uae_prefs currprefs;
extern "C"  struct uae_prefs changed_prefs;  
extern "C" void memory_reset (void);
extern int mainloopdone;
 
 

// Handler for the XM_NOTIFY message
HRESULT COptionsScene::OnNotifyPress( HXUIOBJ hObjPressed, BOOL& bHandled )
{
 
	
	switch(m_GroupJoy.GetCurSel())
	{
		case 0:
			memset (&changed_prefs.jports[0], 0, sizeof (struct jport));
			memset (&changed_prefs.jports[1], 0, sizeof (struct jport));
			changed_prefs.jports[0].id  = JSEM_MICE;
			changed_prefs.jports[1].id  = JSEM_JOYS;
			changed_prefs.mouse_settings[0]->enabled = 1;
			config_changed = 1;
			inputdevice_config_change();			 
			break;
		case 1:
			memset (&changed_prefs.jports[0], 0, sizeof (struct jport));
			memset (&changed_prefs.jports[1], 0, sizeof (struct jport));			
			changed_prefs.jports[0].id  = JSEM_JOYS + 1;
			changed_prefs.jports[1].id  = JSEM_JOYS;
			changed_prefs.mouse_settings[1]->enabled = 0;	
			config_changed = 1;
			inputdevice_config_change();			 
			break;
		case 2:		 
			memset (&changed_prefs.jports[0], 0, sizeof (struct jport));
			memset (&changed_prefs.jports[1], 0, sizeof (struct jport));
			changed_prefs.jports[0].id  = JSEM_MICE;
			changed_prefs.jports[1].id  = JSEM_MICE + 1;
			changed_prefs.mouse_settings[0]->enabled = 0;			 		 
			changed_prefs.mouse_settings[1]->enabled = 1;		
			config_changed = 1;
			inputdevice_config_change();						 
			break;	 
 
	}

  
	 
	
	switch(m_GroupCPU.GetCurSel())
	{
		case 0:
			changed_prefs.cpu_model = 68000;		 			 
			check_prefs_changed_cpu();
			break;
		case 1:
			changed_prefs.cpu_model = 68010;			 			 
			check_prefs_changed_cpu();
			break;
		case 2:
			changed_prefs.cpu_model = 68020;		 
			check_prefs_changed_cpu();
			changed_prefs.address_space_24 = 0;			 
			break;
		case 3:
			changed_prefs.cpu_model = 68020;			 
			check_prefs_changed_cpu();
			changed_prefs.address_space_24 = 1;			 
			break;
		case 4:
			changed_prefs.cpu_model = 68030;	
			check_prefs_changed_cpu();
			changed_prefs.address_space_24 = 0;			 
			break;
		case 5:
			changed_prefs.cpu_model = 68040;	
			check_prefs_changed_cpu();
			changed_prefs.address_space_24 = 0;			 
			break;		

	}

	switch (m_GroupChipset.GetCurSel())
	{
	case 0:
		changed_prefs.chipset_mask = 0;
		break;
	case 1:
		changed_prefs.chipset_mask = CSMASK_ECS_AGNUS;
		break;
	case 2:
		changed_prefs.chipset_mask = CSMASK_ECS_DENISE;
		break;
	case 3:
		changed_prefs.chipset_mask = (CSMASK_ECS_AGNUS | CSMASK_ECS_DENISE);
		break;
	case 4:
		changed_prefs.chipset_mask = (CSMASK_AGA | CSMASK_ECS_AGNUS | CSMASK_ECS_DENISE);
		break;
	}
 

	switch(m_GroupChipMem.GetCurSel())
	{
		case 0:
			m_GroupFast.SetEnable(true);
			changed_prefs.chipmem_size = 0x80000; 
			changed_prefs.bogomem_size = 0x0; 
			memory_reset();
			break;
		case 1:
			m_GroupFast.SetEnable(true);
			changed_prefs.chipmem_size = 0x100000; 
			changed_prefs.bogomem_size = 0x0; 
			memory_reset();
			break;
		case 2:
			m_GroupFast.SetEnable(true);
			changed_prefs.chipmem_size = 0x200000; 
			changed_prefs.bogomem_size = 0x0; 
			memory_reset();
			break;
		case 3:
			m_GroupFast.SetCurSel(0);
			m_GroupFast.SetEnable(false);
			changed_prefs.fastmem_size = 0x00;
			changed_prefs.chipmem_size = 0x400000; 
			changed_prefs.bogomem_size = 0x0; 
			memory_reset();
			break;
		case 4:
			m_GroupFast.SetCurSel(0);
			m_GroupFast.SetEnable(false);
			changed_prefs.chipmem_size = 0x800000; 
			changed_prefs.fastmem_size = 0x00;
			changed_prefs.bogomem_size = 0x0; 
			memory_reset();
			break;
	}


	switch (m_GroupFast.GetCurSel())
	{
		case 0x00000000:
			changed_prefs.fastmem_size = 0x00000000; 
			memory_reset(); 
			break;
		case 0x100000:
			changed_prefs.fastmem_size = 0x100000; 
			memory_reset(); 
			break;
		case 0x200000:
			changed_prefs.fastmem_size = 0x200000; 
			memory_reset(); 
			break;
		case 0x400000:
			changed_prefs.fastmem_size = 0x400000; 
			memory_reset(); 
			break;
		case 0x800000:
			changed_prefs.fastmem_size = 0x800000; 
			memory_reset(); 
			break;
	}



	switch(m_GroupBogoMem.GetCurSel())
	{
		case 0:			 
			changed_prefs.bogomem_size = 0x0; 
			memory_reset();
			break;
		case 1:			 
			changed_prefs.bogomem_size = 0x80000; 
			memory_reset();
			break;
		case 2:			 
			changed_prefs.bogomem_size =  0x100000; 
			memory_reset();
			break;
		case 3:			 
			changed_prefs.bogomem_size =  0x180000; 
			memory_reset();
			break;
		case 4:			 
			changed_prefs.bogomem_size = 0x1c0000; 
			memory_reset();
			break;
	}

	switch(m_GroupFast.GetCurSel())
	{
		case 0:
			changed_prefs.fastmem_size = 0x00;		 
			memory_reset();
			break;
		case 1:
			changed_prefs.fastmem_size = 0x100000;			 
			memory_reset();
			break;
		case 2:
			changed_prefs.fastmem_size = 0x200000;			 
			memory_reset();
			break;
		case 3:
			changed_prefs.fastmem_size = 0x400000;
			memory_reset();
			break;
		case 4:
			changed_prefs.fastmem_size = 0x800000;
			memory_reset();
			break;
	}

	switch(m_GroupZ3Fast.GetCurSel())
	{
		case 0:
			changed_prefs.z3fastmem_size = 0x00;		 
			memory_reset();
			break;
		case 1:
			changed_prefs.z3fastmem_size = 0x800000;			 
			memory_reset();
			break;
		case 2:
			changed_prefs.z3fastmem_size = 0x2000000;
			memory_reset();
			break;
		case 3:
			changed_prefs.z3fastmem_size = 0x4000000;
			memory_reset();
			break;
		case 4:
			changed_prefs.z3fastmem_size = 0x8000000;
			memory_reset();
			break;
	}

	switch(m_GroupRTG.GetCurSel())
	{
		case 0:
			changed_prefs.gfxmem_size = 0x00;		 
			memory_reset();
			break;
		case 1:
			changed_prefs.gfxmem_size = 0x800000;			 
			memory_reset();
			break;
		case 2:
			changed_prefs.gfxmem_size = 0x2000000;
			memory_reset();
			break;
		case 3:
			changed_prefs.gfxmem_size = 0x4000000;
			memory_reset();
			break;
		case 4:
			changed_prefs.gfxmem_size = 0x8000000;
			memory_reset();
			break;
	}

 

	if (m_FasterRTG.IsChecked())
	{
		changed_prefs.picasso96_nocustom = 1;
		check_prefs_changed_gfx();
	}
	else
	{
		changed_prefs.picasso96_nocustom = 0;
		check_prefs_changed_gfx();
	}
 
	
	if (m_BSDSocket.IsChecked())
	{
		currprefs.socket_emu = 1;
	}
	else
	{
		currprefs.socket_emu = 0;
	}

	if (m_Picasso96.IsChecked())
	{
		currprefs.picasso96_modeflags = 212;	
		check_prefs_changed_gfx();
	}
	else
	{
		currprefs.picasso96_modeflags = 0;		 
		check_prefs_changed_gfx();
	}

	if (m_DriveClick.IsChecked())
	{
		changed_prefs.floppyslots[0].dfxclick = -1;
		changed_prefs.dfxclickvolume = 33;
		changed_prefs.dfxclickchannelmask = 0xFFFF;	
		check_prefs_changed_audio ();	 
		driveclick_check_prefs();
	}
	else
	{
		changed_prefs.floppyslots[0].dfxclick = 0; 		
		check_prefs_changed_audio ();	 
		driveclick_check_prefs();
	}
	 	 		
	
	if (m_24BitAddressing.IsChecked())
	{
		changed_prefs.address_space_24 = 1;
		check_prefs_changed_custom ();
	}
	else
	{
		changed_prefs.address_space_24 = 0;
		check_prefs_changed_custom ();
	}

	if (m_CycleExact.IsChecked())
	{
		changed_prefs.cpu_cycle_exact = 1;
		check_prefs_changed_custom ();
		check_prefs_changed_gfx();
	}
	else
	{
		changed_prefs.cpu_cycle_exact = 0;
		check_prefs_changed_custom ();
		check_prefs_changed_gfx();
	}


	if (m_ImmediateBlits.IsChecked())
	{
		changed_prefs.immediate_blits = 1;
		check_prefs_changed_custom ();
		check_prefs_changed_gfx();
	}
	else
	{
		changed_prefs.immediate_blits = 0;
		check_prefs_changed_custom ();
		check_prefs_changed_gfx();
	}


	if (m_ShowLeds.IsChecked())
	{
		currprefs.leds_on_screen = 1;
	}
	else
	{
		currprefs.leds_on_screen = 0;
	}

	
	if( XuiControlIsBackButton( hObjPressed ) )
	{

		config_changed = 1;		 
		fixup_prefs (&currprefs);
	
		this->NavigateBack();
	}		 
	
    bHandled = TRUE;
    return S_OK;
 }


    //----------------------------------------------------------------------------------
    // Performs initialization tasks - retreives controls.
    //----------------------------------------------------------------------------------
HRESULT COptionsScene::OnInit( XUIMessageInit* pInitData, BOOL& bHandled )
{ 
        // Retrieve controls for later use.
        GetChildById( L"XuiRadioButtonA500", &m_machineA500 );
        GetChildById( L"XuiRadioButtonA1000", &m_machineA1000 );
        GetChildById( L"XuiRadioButtonA600", &m_machineA600 );		 
		GetChildById( L"XuiRadioButtonA1200", &m_machineA1200 );
		GetChildById( L"XuiRadioButtonA4000", &m_machineA4000 );

		GetChildById( L"XuiRomPreview", &m_Kickstart11 );	 		 
		GetChildById( L"XuiBackVideoRoms", &m_Kickstart12 );
		GetChildById( L"XuiCurrentDeviceText", &m_Kickstart13);
		GetChildById( L"XuiNextDeviceButton", &m_Kickstart20);
		GetChildById( L"XuiDiskDriveBG", &m_Kickstart31);

		GetChildById( L"XuDiskDriveLabel", &m_CPU68000);
		GetChildById( L"XuiButtonDF0", &m_CPU68010);
		GetChildById( L"XuiButtonDF1", &m_CPU68020);
		GetChildById( L"XuiButtonDF1", &m_CPU68020EC);
		GetChildById( L"XuiButtonDF1", &m_CPU68040);
		GetChildById( L"XuiButtonDF1", &m_CPU68060);

		GetChildById( L"XuiCurrentDeviceDrive1", &m_ChipSetOCS);
		GetChildById( L"XuiCurrentDeviceDrive2", &m_ChipSetECS_A);
		GetChildById( L"XuiCurrentDeviceDrive3", &m_ChipSetECS_D);
		GetChildById( L"XuiCurrentDeviceDrive4", &m_ChipSetECS);
		GetChildById( L"XuiCurrentDeviceDrive4", &m_ChipSetAGA);

		GetChildById( L"XuiCurrentDeviceDrive1", &m_DrivesOne);
		GetChildById( L"XuiCurrentDeviceDrive2", &m_DrivesTwo);
		GetChildById( L"XuiCurrentDeviceDrive3", &m_DrivesThree);
		GetChildById( L"XuiCurrentDeviceDrive4", &m_DrivesFour);
		 
		GetChildById( L"XuiRadioButtonChip512", &m_ChipMemory512);
		GetChildById( L"XuiRadioButtonChip1024", &m_ChipMemory1024);
		GetChildById( L"XuiRadioButtonChip2048", &m_ChipMemory2048);
		GetChildById( L"XuiRadioButtonChip4096", &m_ChipMemory4096);		 
		GetChildById( L"XuiRadioButtonChip8192", &m_ChipMemory8192);
 
		GetChildById( L"XuiRadioButtonFAST0", &m_FastMemoryNone);		 
		GetChildById( L"XuiRadioButtonFAST1", &m_FastMemory1024);
		GetChildById( L"XuiRadioButtonFAST2", &m_FastMemory2048);
		GetChildById( L"XuiRadioButtonFAST4", &m_FastMemory4096);
		GetChildById( L"XuiRadioButtonFAST8", &m_FastMemory8192);

		GetChildById( L"XuiRadioButtonJoy1", &m_Joy1);
		GetChildById( L"XuiRadioButtonJoy2", &m_Joy2);
		GetChildById( L"XuiRadioButtonJoy3", &m_Joy3);

		GetChildById( L"XuiRadioGroupMachines", &m_GroupMachine);
		GetChildById( L"XuiRadioGroupKickStarts", &m_GroupKickstart);
		GetChildById( L"XuiRadioGroupCPU", &m_GroupCPU);
		GetChildById( L"XuiRadioGroupChipset", &m_GroupChipset);
		GetChildById( L"XuiRadioGroupDrives", &m_GroupDrives);
		GetChildById( L"XuiRadioGroupChipRAM", &m_GroupChipMem);
		GetChildById( L"XuiRadioGroupFastRAM", &m_GroupFast);
		GetChildById( L"XuiRadioGroupJoystick", &m_GroupJoy);
		GetChildById( L"XuiRadioGroupSlowRAM", &m_GroupBogoMem);
		GetChildById( L"XuiRadioGroupFastRAMZ3", &m_GroupZ3Fast);
		GetChildById( L"XuiRadioGroupRTGRam", &m_GroupRTG);
 	 
		
		GetChildById( L"XuiFasterRTG", &m_FasterRTG);
		GetChildById( L"XuiBSDSocket", &m_BSDSocket);
		GetChildById( L"XuiPicasso96", &m_Picasso96);
		GetChildById( L"XuiDriveClick", &m_DriveClick);
		GetChildById( L"Xui24BitAddressing", &m_24BitAddressing);
		GetChildById( L"XuiImmediateBlits", &m_ImmediateBlits);
		GetChildById( L"XuiCycleExact", &m_CycleExact);
		GetChildById( L"XuiShowLEDS", &m_ShowLeds);

		GetChildById( L"XuiMore",&m_More);
  
	 
		m_GroupMachine.SetCurSel(0);
		m_GroupKickstart.SetCurSel(2);
		//m_GroupCPU.SetCurSel(0);
		m_GroupChipset.SetCurSel(0);
		m_GroupDrives.SetCurSel(0);
		//m_GroupChipMem.SetCurSel(0);
		//m_GroupFast.SetCurSel(0);

		int cpu = currprefs.cpu_model;
 
		if (cpu == 68000)
			m_GroupCPU.SetCurSel(0);	
		else if (cpu == 68010)
			m_GroupCPU.SetCurSel(1);
		else if (cpu == 68020)
		{
			if (currprefs.address_space_24==0)			
				m_GroupCPU.SetCurSel(2);
			else
				m_GroupCPU.SetCurSel(3);			 
		}
		else if (cpu == 68030)
		{
			m_GroupCPU.SetCurSel(4);			 
		}
		else if (cpu == 68040)
		{
			m_GroupCPU.SetCurSel(5);			 
		}
 
		if (currprefs.jports[0].id == 200 && currprefs.jports[1].id == 100) {
			m_GroupJoy.SetCurSel(0);
		} else
		if (currprefs.jports[0].id == 101 && currprefs.jports[1].id == 100) {
			m_GroupJoy.SetCurSel(1);
		} else {
			m_GroupJoy.SetCurSel(2);
		}

		if (currprefs.picasso96_nocustom == 1)
		{
			m_FasterRTG.SetCheck(true);
		}
		else
		{
			m_FasterRTG.SetCheck(false);
		}

		if (currprefs.socket_emu == 1)
		{
			m_BSDSocket.SetCheck(true);
		}
		else
		{
			m_BSDSocket.SetCheck(false);
		}

		if (currprefs.picasso96_modeflags > 0)
		{
			m_Picasso96.SetCheck(true);
		}
		else
		{
			m_Picasso96.SetCheck(false);
		}
		
		if (currprefs.floppyslots[0].dfxclick == -1)
		{
			m_DriveClick.SetCheck(true);
		}
		else
		{
			m_DriveClick.SetCheck(false);
		}

		if (currprefs.address_space_24 == 1)
		{
			m_24BitAddressing.SetCheck(true);
		}
		else
		{
			m_24BitAddressing.SetCheck(false);
		}

		if (currprefs.immediate_blits == 1)
		{
			m_ImmediateBlits.SetCheck(true);
		}
		else
		{
			m_ImmediateBlits.SetCheck(false);
		}

		if (currprefs.cpu_cycle_exact == 1)
		{
			m_CycleExact.SetCheck(true);
		}
		else
		{
			m_CycleExact.SetCheck(false);
		}

		if (currprefs.leds_on_screen == 1)
		{
			m_ShowLeds.SetCheck(true);
		}
		else
		{
			m_ShowLeds.SetCheck(false);
		}


		switch (changed_prefs.chipmem_size)
		{
			case 0x00080000:
				m_GroupChipMem.SetCurSel(0);
				break;
			case 0x100000:
				m_GroupChipMem.SetCurSel(1);
				break;
			case 0x200000:
				m_GroupChipMem.SetCurSel(2);
				break;
			case 0x400000:
				m_GroupChipMem.SetCurSel(3);
				break;
			case 0x800000:
				m_GroupChipMem.SetCurSel(4);
				break;
		}

		if (currprefs.chipset_mask == 0)
		{
			m_GroupChipset.SetCurSel(0);
		}
		else if (currprefs.chipset_mask & CSMASK_AGA)
		{
			m_GroupChipset.SetCurSel(4);
		}
		else if (currprefs.chipset_mask & (CSMASK_ECS_AGNUS | CSMASK_ECS_DENISE))
		{				 
			m_GroupChipset.SetCurSel(3);
		}
		else if (currprefs.chipset_mask & CSMASK_ECS_AGNUS)
		{				 
			m_GroupChipset.SetCurSel(1);
		}
		else if (currprefs.chipset_mask & CSMASK_ECS_DENISE)
		{				 
			m_GroupChipset.SetCurSel(2);
		}
		
		
		
		switch (changed_prefs.bogomem_size)
		{
			case 0x00000000:
				m_GroupBogoMem.SetCurSel(0);
				break;
			case 0x00080000:
				m_GroupBogoMem.SetCurSel(1);
				break;
			case 0x100000:
				m_GroupBogoMem.SetCurSel(2);
				break;
			case 0x180000:
				m_GroupBogoMem.SetCurSel(3);
				break;
			case 0x1c0000:
				m_GroupBogoMem.SetCurSel(4);
				break;
		}

		switch (changed_prefs.fastmem_size)
		{
			case 0x00000000:
				m_GroupFast.SetCurSel(0);
				break;
			case 0x100000:
				m_GroupFast.SetCurSel(1);
				break;
			case 0x200000:
				m_GroupFast.SetCurSel(2);
				break;
			case 0x400000:
				m_GroupFast.SetCurSel(3);
				break;
			case 0x800000:
				m_GroupFast.SetCurSel(4);
				break;
		}

		switch (changed_prefs.z3fastmem_size)
		{
			case 0x00:
				m_GroupZ3Fast.SetCurSel(0);
				break;
			case 0x800000:
				m_GroupZ3Fast.SetCurSel(1);
				break;
			case 0x2000000:
				m_GroupZ3Fast.SetCurSel(2);
				break;
			case 0x4000000:
				m_GroupZ3Fast.SetCurSel(3);
				break;
			case 0x8000000:
				m_GroupZ3Fast.SetCurSel(4);
				break;
		}

		switch (changed_prefs.gfxmem_size)
		{
			case 0x00:
				m_GroupRTG.SetCurSel(0);
				break;
			case 0x800000:
				m_GroupRTG.SetCurSel(1);
				break;
			case 0x2000000:
				m_GroupRTG.SetCurSel(2);
				break;
			case 0x4000000:
				m_GroupRTG.SetCurSel(3);
				break;
			case 0x8000000:
				m_GroupRTG.SetCurSel(4);
				break;
		}
		

	 
 
		bHandled = TRUE;
        return S_OK;
}