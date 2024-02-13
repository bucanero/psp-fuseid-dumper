PSPSDK = $(shell psp-config --pspsdk-path)
PSPLIBSDIR = $(PSPSDK)/..
TARGET = fuseid_dumper
OBJS = main.o kernel_prx/kernel_prx.S
LIBS = -lpspgum -lpspgu

CFLAGS = -O2 -G0 -Wall -Wstrict-aliasing=0
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = FuseID dumper

include $(PSPSDK)/lib/build.mak
