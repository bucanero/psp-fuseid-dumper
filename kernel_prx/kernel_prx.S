	.set noreorder

#include "pspstub.s"

	STUB_START "kernel_prx",0x40090000,0x00020005
	STUB_FUNC  0xC2712F9D,prxNandGetScramble
	STUB_FUNC  0x06C91BFC,prxSysregGetFuseId
	STUB_END
