TARGET = eboot
OBJS = main.o ../kernel_prx/kernel_prx.S

INCDIR = 
CFLAGS = -O2 -G0 -Wall -Wno-trigraphs
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS) -c

PSP_FW_VERSION = 300

EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = FuseID dumper

LIBDIR = 
LIBS = 
LDFLAGS = -L.

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak
