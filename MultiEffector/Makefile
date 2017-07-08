
#-------------------------------------------------------------------------------
# Variable
#-------------------------------------------------------------------------------
######## Target ########
TARGET			= effector

#TARGET_AXF		= $(TARGET).axf
TARGET_AXF		= $(TARGET)
TARGET_SYM		= $(TARGET).sym
TARGET_DUMP		= $(TARGET).dump
TARGET_BIN		= $(TARGET).bin
TARGET_SREC		= $(TARGET).srec

######## Source ########
ASMS			=

SRCS			=
SRCS			+= effector.c

OBJS			= $(ASMS:.S=.o) $(SRCS:.c=.o)
DPNS			= $(ASMS:.S=.d) $(SRCS:.c=.d)

######## Directory ########
INCLUDE_DIRS	= .
SRC_DIRS		= .

######## Command ########
PREFIX			= arm-linux-gnueabihf-

GCC				= $(PREFIX)gcc
AR				= $(PREFIX)ar
LD				= $(PREFIX)ld
NM				= $(PREFIX)nm
OBJCOPY			= $(PREFIX)objcopy
OBJDUMP			= $(PREFIX)objdump
SIZE			= $(PREFIX)size

CFLAGS			=
CFLAGS			+= $(addprefix -I, $(INCLUDE_DIRS))
CFLAGS			+= -lpthread
CFLAGS			+= -lasound

ASFLAGS			=

LDFLAGS			=

RM				= rm -f

#-------------------------------------------------------------------------------
# Special Variable
#-------------------------------------------------------------------------------

vpath %.h	= $(INCLUDE_DIRS)
vpath %.S	= $(SRC_DIRS)
vpath %.c	= $(SRC_DIRS)

.PHONY: all map sym dump bin srec clean
.SUFFIXES: .h .S .c .d .o

#-------------------------------------------------------------------------------
# Command
#-------------------------------------------------------------------------------

all: axf

axf: $(TARGET_AXF)

sym: $(TARGET_SYM)

dump: $(TARGET_DUMP)

bin: $(TARGET_BIN)

srec: $(TARGET_SREC)

clean:
	$(RM) $(TARGET_AXF)
	$(RM) $(TARGET_SYM)
	$(RM) $(TARGET_DUMP)
	$(RM) $(TARGET_BIN)
	$(RM) $(TARGET_SREC)
	$(RM) $(OBJS)
	$(RM) $(DPNS)

#-------------------------------------------------------------------------------
# Dependence
#-------------------------------------------------------------------------------
######## Asm file -> Depend file ########
.S.d:
	$(GCC) -MM -MD $<

######## Asm file -> Object file ########
.S.o:
	$(GCC) -c $(ASFLAGS) $(CFLAGS) $<

######## C file -> Depend file ########
.c.d:
	$(GCC) -MM -MD $(CFLAGS) $<

######## C file -> Object file ########
.c.o:
	$(GCC) -c $(CFLAGS) $<

######## C file -> Asm file ########
.c.S:
	$(GCC) -S $(CFLAGS) $<

######## Create AXF and map file ########
$(TARGET_AXF): $(OBJS)
	$(GCC) $(CFLAGS) $(LDFLAGS) -o $(TARGET_AXF) $(OBJS)

######## Create symbol file ########
$(TARGET_SYM): $(TARGET_AXF)
	$(NM) $^ > $@

######## Create dump file ########
$(TARGET_DUMP): $(TARGET_AXF)
	$(OBJDUMP) -a -h -D -z $^ > $@

######## Create binary ########
$(TARGET_BIN): $(TARGET_AXF)
	$(OBJCOPY) -O binary $^ $@

######## Create S-record ########
$(TARGET_SREC): $(TARGET_AXF)
	$(OBJCOPY) -O srec $^ $@

######## Create Intel HEX ########
$(TARGET_HEX): $(TARGET)
	$(OBJCOPY) -O ihex $^ $@

######## Include ########
-include $(DPNS)

