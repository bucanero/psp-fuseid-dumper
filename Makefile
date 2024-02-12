PSPSDK = $(shell psp-config --pspsdk-path)
PSPLIBSDIR = $(PSPSDK)/..
TARGET = fuseid_dumper
OBJS = main.o
LIBS = -lpspgum -lpspgu

CFLAGS = -O2 -Wall -Wstrict-aliasing=0
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = FuseID dumper

include $(PSPSDK)/lib/build.mak
