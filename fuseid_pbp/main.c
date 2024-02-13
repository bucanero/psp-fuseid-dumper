#include <pspsdk.h>
#include <pspkernel.h>
#include <pspctrl.h>
#include <pspdisplay.h>
#include <pspge.h>
#include <psprtc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define VER_MAJOR 1
#define VER_MINOR 0

PSP_MODULE_INFO("pspIdent", 0, VER_MAJOR, VER_MINOR);
PSP_MAIN_THREAD_ATTR(0);
PSP_HEAP_SIZE_KB(1024);

#include "../kernel_prx/kernel_prx.h"
#include "../kernel_prx/kprx_inc.h"

#define printf pspDebugScreenPrintf

#define color(x) pspDebugScreenSetTextColor(x)
#define RED 0xff0000ff
#define BLUE 0xffff0000
#define GREEN 0xff00ff00
#define LGRAY 0xffd3d3d3
#define WHITE 0xffffffff
#define YELLOW 0xff00ffff
#define ORANGE 0xff007fff

char* stpcpy(char * dst, const char * src)
{
	return memcpy(dst, src, strlen(src));
}

int main(int argc, char*argv[])
{
	FILE* fp;

	pspDebugScreenInit();
	pspDebugScreenClear();
	color(BLUE);
	printf("\n FuseID dumper v%i.%i by Bucanero\n\n", VER_MAJOR, VER_MINOR);

	fp = fopen("kernel.prx", "wb");
	fwrite(kernel_prx, sizeof(kernel_prx), 1, fp);
	fclose(fp);

	SceUID mod = pspSdkLoadStartModule("kernel.prx", PSP_MEMORY_PARTITION_KERNEL);
	if (mod < 0) {
		int firmware = sceKernelDevkitVersion();
		color(ORANGE); printf(" *"); color(WHITE);
		printf(" %-10s %x.%x%x (0x%08x)\n\n", "Firmware", firmware >> 24,
				(firmware >> 16) & 0xff, (firmware >> 8) & 0xff, firmware);

		color(ORANGE);
		printf(" Error: pspSdkLoadStartModule() returned 0x%08x\n", mod);
		printf(" The program will automatically quit after 8 seconds...\n");

		sceKernelDelayThread(8*1000*1000);
		sceKernelExitGame();
	}

	SceCtrlData pad;
	sceCtrlSetSamplingCycle(0);
	sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);

	long long fuseid = prxSysregGetFuseId();

	color(GREEN); printf(" *"); color(WHITE);
	printf(" %-10s 0x%llX\n", "Fuse ID", fuseid);
	color(GREEN); printf(" *"); color(WHITE);
	printf(" %-10s 0x%08lX\n", "Fuse 90", (u32)(fuseid & 0xFFFFFFFF));
	color(GREEN); printf(" *"); color(WHITE);
	printf(" %-10s 0x%08lX\n", "Fuse 94", (u32)(fuseid >> 32));
	printf("\n");

	sceKernelDelayThread(1*1000*1000);

	printf(" Press X or O to quit the program\n\n");
	for(;;) {
		sceCtrlReadBufferPositive(&pad, 1);
		if (pad.Buttons & (PSP_CTRL_CROSS | PSP_CTRL_CIRCLE)) {

			printf(" Saving FUSEID.BIN ...\n\n");

			fp = fopen("FUSEID.BIN", "wb");
			fwrite(&fuseid, 1, 8, fp);
			fclose(fp);

			color(ORANGE);
			printf(" The program will automatically quit in 3 seconds...\n");

			sceKernelDelayThread(3*1000*1000);
			break;
		}
		sceDisplayWaitVblank();
	}

	sceKernelExitGame();
	return 0;
}
