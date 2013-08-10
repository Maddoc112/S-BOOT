CC      = arm-hwlee-linux-gnueabi-gcc
LD      = arm-hwlee-linux-gnueabi-ld
AR      = arm-hwlee-linux-gnueabi-ar
OBJCOPY = arm-hwlee-linux-gnueabi-objcopy
OBJDUMP = arm-hwlee-linux-gnueabi-objdump

INCLUDEDIR1 	:= $(shell pwd)/include
INCLUDEDIR2 	:= $(shell pwd)/lib
CPPFLAGS   	:= -nostdinc -fno-builtin -I$(INCLUDEDIR1) -I$(INCLUDEDIR2)
CFLAGS 		:= -Wall -O2

BUILT_IN_OBJ = built-in.o

export 	CC LD AR OBJCOPY OBJDUMP INCLUDEDIR CPPFLAGS CFLAGS
export BUILT_IN_OBJ 

SUBDIRS =  start device lib app main
SUBOBJS := $(foreach n, $(SUBDIRS), $(n)/$(BUILT_IN_OBJ))

all: S-Boot.bin S-Boot.dis

S-Boot.bin: S-Boot.elf
	$(OBJCOPY) -O binary -S $^ $@

S-Boot.dis: S-Boot.elf
	$(OBJDUMP) -D -m arm  $^ > $@

S-Boot.elf: $(SUBOBJS)
	$(LD) -Tsboot.lds  -o $@ $^

$(SUBOBJS): $(SUBDIRS)

.PHONY: $(SUBDIRS)
$(SUBDIRS):
	@make -C $@ all

.PHONY : clean
clean:
	rm -f  S-Boot.dis S-Boot.bin S-Boot.elf *.o
	@ for subdir in $(SUBDIRS); do \
	    (make clean -C $$subdir); \
	done
	
