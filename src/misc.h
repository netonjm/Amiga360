/*
 * PUAE - The Un*x Amiga Emulator
 *
 * Misc
 *
 * Copyright 2010-2011 Mustafa TUFAN
 */

extern int ispressed (int key);

extern int D3D_goodenough (void);
extern int DirectDraw_CurrentRefreshRate (void);
extern int DirectDraw_GetVerticalBlankStatus (void);
extern double getcurrentvblankrate (void);
extern void setid (struct uae_input_device *uid, int i, int slot, int sub, int port, int evt);
