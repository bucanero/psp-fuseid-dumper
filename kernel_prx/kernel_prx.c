#include <pspsdk.h>
#include <pspkernel.h> // sceKernelUtilsSha1Digest()
#include <pspsyscon.h>
#include <pspsysreg.h>
#include <pspidstorage.h> // sceIdStorageLookup()
#include <pspsysmem_kernel.h> // sceKernelGetModel()

PSP_MODULE_INFO("kernel_prx", 0x1006, 1, 5);
PSP_MAIN_THREAD_ATTR(0);

int module_start(SceSize args, void*argp) {
//	Kprintf("pspident @ built: %s %s", __DATE__, __TIME__);
	return 0;
}

long long prxNandGetScramble(void) {
	int k1 = pspSdkSetK1(0);
	u64 value;
	u32 fuse90, fuse94;
	fuse90 = *(vu32*)(0xBC100090);
	fuse94 = *(vu32*)(0xBC100094);

	value = ((u64)fuse94 << 32) | (u64)fuse90;
	pspSdkSetK1(k1);
	return value;
}

long long sceSysreg_driver_4F46EEDE(void);
long long prxSysregGetFuseId(void) {
	int k1 = pspSdkSetK1(0);
	long long fi = sceSysreg_driver_4F46EEDE();
	pspSdkSetK1(k1);
	return fi;
}

int module_stop(void) {
	return 0;
}
