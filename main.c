/*
 * PSP Software Development Kit - https://github.com/pspdev
 * -----------------------------------------------------------------------
 * Licensed under the BSD license, see LICENSE in PSPSDK root for details.
 *
 * main.c - PSP FuseID dumper by Bucanero
 *
 * Based on message dialog utility sample by:
 * Copyright (c) 2005 Marcus Comstedt <marcus@mc.pp.se>
 *			 (c) 2008 InsertWittyName <tias_dp@hotmail.com>
 *
 */
#include <pspkernel.h>
#include <pspdisplay.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <pspmoduleinfo.h>
#include <psputility.h>
#include <pspgu.h>
#include <pspgum.h>
#include <psprtc.h>
#include <pspsdk.h>
#include "kernel_prx/kernel_prx.h"
#include "kernel_prx/kprx_inc.h"

/* Define the module info section */
PSP_MODULE_INFO("FuseID Dumper", 0, 1, 0);


char* stpcpy(char * dst, const char * src)
{
	return memcpy(dst, src, strlen(src));
}

/* Exit callback */
int exit_callback(int arg1, int arg2, void *common)
{
    sceKernelExitGame();
    return 0;
}

/* Callback thread */
int CallbackThread(SceSize args, void *argp)
{
    int cbid;
    cbid = sceKernelCreateCallback("Exit Callback", exit_callback, NULL);
    sceKernelRegisterExitCallback(cbid);
    sceKernelSleepThreadCB();

    return 0;
}

/* Sets up the callback thread and returns its thread id */
int SetupCallbacks(void)
{
    int thid = 0;
    thid = sceKernelCreateThread("update_thread", CallbackThread, 0x11, 0xFA0, 0, 0);
    if (thid >= 0)
    sceKernelStartThread(thid, 0, 0);
    return thid;
}

/* Graphics stuff */

static unsigned int __attribute__((aligned(16))) list[0x10000];

#define BUF_WIDTH (512)
#define SCR_WIDTH (480)
#define SCR_HEIGHT (272)
#define PIXEL_SIZE (4) /* change this if you change to another screenmode */
#define FRAME_SIZE (BUF_WIDTH * SCR_HEIGHT * PIXEL_SIZE)
#define ZBUF_SIZE (BUF_WIDTH SCR_HEIGHT * 2) /* zbuffer seems to be 16-bit? */

static void SetupGu(void)
{
    sceGuInit();

    sceGuStart(GU_DIRECT,list);
    sceGuDrawBuffer(GU_PSM_8888,(void*)0,BUF_WIDTH);
    sceGuDispBuffer(SCR_WIDTH,SCR_HEIGHT,(void*)0x88000,BUF_WIDTH);
    sceGuDepthBuffer((void*)0x110000,BUF_WIDTH);
    sceGuOffset(2048 - (SCR_WIDTH/2),2048 - (SCR_HEIGHT/2));
    sceGuViewport(2048,2048,SCR_WIDTH,SCR_HEIGHT);
    sceGuDepthRange(0xc350,0x2710);
    sceGuScissor(0,0,SCR_WIDTH,SCR_HEIGHT);
    sceGuEnable(GU_SCISSOR_TEST);
    sceGuDepthFunc(GU_GEQUAL);
    sceGuEnable(GU_DEPTH_TEST);
    sceGuFrontFace(GU_CW);
    sceGuShadeModel(GU_SMOOTH);
    sceGuEnable(GU_CULL_FACE);
    sceGuEnable(GU_CLIP_PLANES);
    sceGuFinish();
    sceGuSync(0,0);
    
    sceDisplayWaitVblankStart();
    sceGuDisplay(GU_TRUE);
}

static void DrawStuff(void)
{
    sceGuStart(GU_DIRECT, list);
    sceGuClearColor(0xFF880808);
    sceGuClearDepth(0);
    sceGuClear(GU_COLOR_BUFFER_BIT|GU_DEPTH_BUFFER_BIT);
    sceGuFinish();
    sceGuSync(0, 0);
}


/* Utility dialog functions */

pspUtilityMsgDialogParams dialog;

static void ConfigureDialog(pspUtilityMsgDialogParams *dialog, size_t dialog_size)
{
    memset(dialog, 0, dialog_size);

    dialog->base.size = dialog_size;
    sceUtilityGetSystemParamInt(PSP_SYSTEMPARAM_ID_INT_LANGUAGE, &dialog->base.language); // Prompt language
    sceUtilityGetSystemParamInt(PSP_SYSTEMPARAM_ID_INT_UNKNOWN, &dialog->base.buttonSwap); // X/O button swap

    dialog->base.graphicsThread = 0x11;
    dialog->base.accessThread = 0x13;
    dialog->base.fontThread = 0x12;
    dialog->base.soundThread = 0x10;
}

static void ShowMessageDialog(const char *message)
{
    ConfigureDialog(&dialog, sizeof(dialog));
    dialog.mode = PSP_UTILITY_MSGDIALOG_MODE_TEXT;
    dialog.options = PSP_UTILITY_MSGDIALOG_OPTION_TEXT;
    strcpy(dialog.message, message);

    sceUtilityMsgDialogInitStart(&dialog);

    for(;;) {
        DrawStuff();

        switch(sceUtilityMsgDialogGetStatus()) {

        case 2:
            sceUtilityMsgDialogUpdate(1);
            break;
            
        case 3:
            sceUtilityMsgDialogShutdownStart();
            break;
            
        case 0:
            return;
            
        }

        sceDisplayWaitVblankStart();
        sceGuSwapBuffers();
    }
}

/*
  sceRtcCompareTick kernel exploit by davee, implementation by CelesteBlue
  https://github.com/PSP-Archive/LibPspExploit/blob/main/kernel_read.c
*/

// input: 4-byte-aligned kernel address to a 64-bit integer
// return *addr >= value;
static int is_ge_u64(uint32_t addr, uint32_t *value)
{
    return (int)sceRtcCompareTick((uint64_t *)value, (uint64_t *)addr) <= 0;
}

// input: 4-byte-aligned kernel address
// return *addr
uint64_t pspXploitKernelRead64(uint32_t addr)
{
    uint32_t value[2] = {0, 0};
    uint32_t res[2] = {0, 0};
    int bit_idx = 0;
    for (; bit_idx < 32; bit_idx++) {
        value[1] = res[1] | (1 << (31 - bit_idx));
        if (is_ge_u64(addr, value))
            res[1] = value[1];
    }
    value[1] = res[1];
    bit_idx = 0;
    for (; bit_idx < 32; bit_idx++) {
        value[0] = res[0] | (1 << (31 - bit_idx));
        if (is_ge_u64(addr, value))
            res[0] = value[0];
    }
    return *(uint64_t*)res;
}

/* main routine */
int main(int argc, char *argv[])
{
    char string[256];
    uint64_t fuse;
    FILE* fp;

    SetupCallbacks();
    SetupGu();
    
    fuse = pspXploitKernelRead64(0xBC100090);
    
    // if Kernel read exploit fails, try kernel.prx (e.g. Adrenaline)
    if (fuse == 0xFFFFFFFFFFFFFFFF)
    {
        fp = fopen("kernel.prx", "wb");
        fwrite(kernel_prx, sizeof(kernel_prx), 1, fp);
        fclose(fp);

        if (pspSdkLoadStartModule("kernel.prx", PSP_MEMORY_PARTITION_KERNEL) < 0)
        {
            ShowMessageDialog("Error: pspSdkLoadStartModule()");
            sceKernelExitGame();
        }
        else fuse = prxSysregGetFuseId();
        
        sceIoRemove("kernel.prx");
    }

    if ((fp = fopen("FUSEID.BIN", "wb")) != NULL)
    {
        fwrite(&fuse, sizeof(uint64_t), 1, fp);
        fclose(fp);
    }

    snprintf(string, sizeof(string), "Fuse ID successfully dumped!\nFuse 90: 0x%08lX\nFuse 94: 0x%08lX", (uint32_t)(fuse & 0xFFFFFFFF), (uint32_t)(fuse >> 32));

    ShowMessageDialog(string);

    sceKernelExitGame();
    return 0;
}
