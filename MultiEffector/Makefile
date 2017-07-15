
#-------------------------------------------------------------------------------
# Variable
#-------------------------------------------------------------------------------
######## Target ########
TARGET			= effector

TARGET_EXE		= $(TARGET)
TARGET_SYM		= $(TARGET).sym
TARGET_DUMP		= $(TARGET).dump
TARGET_BIN		= $(TARGET).bin
TARGET_SREC		= $(TARGET).srec

######## Source ########
ASMS			=

SRCS			=
SRCS			+= effector.c
SRCS			+= effdata.c
SRCS			+= effmng.c
SRCS			+= shell.c
SRCS			+= sounddev.c
SRCS			+= through.c
SRCS			+= sma.c

OBJS			= $(ASMS:.S=.o) $(SRCS:.c=.o)
DPNS			= $(ASMS:.S=.d) $(SRCS:.c=.d)

######## Directory ########
INCLUDE_DIRS	= .
SRC_DIRS		= .

######## Command ########
#PREFIX			= arm-linux-gnueabihf-
PREFIX			=

GCC				= $(PREFIX)gcc
AR				= $(PREFIX)ar
LD				= $(PREFIX)ld
NM				= $(PREFIX)nm
OBJCOPY			= $(PREFIX)objcopy
OBJDUMP			= $(PREFIX)objdump
SIZE			= $(PREFIX)size

CFLAGS			=
#CFLAGS			+= $(addprefix -I, $(INCLUDE_DIRS))
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

all: exe

exe: $(TARGET_EXE)

sym: $(TARGET_SYM)

dump: $(TARGET_DUMP)

bin: $(TARGET_BIN)

srec: $(TARGET_SREC)

clean:
	$(RM) $(TARGET_EXE)
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

######## Create execution file ########
$(TARGET_EXE): $(OBJS)
	$(GCC) $(CFLAGS) $(LDFLAGS) -o $(TARGET_EXE) $(OBJS)

######## Create symbol file ########
$(TARGET_SYM): $(TARGET_EXE)
	$(NM) $^ > $@

######## Create dump file ########
$(TARGET_DUMP): $(TARGET_EXE)
	$(OBJDUMP) -a -h -D -z $^ > $@

######## Create binary ########
$(TARGET_BIN): $(TARGET_EXE)
	$(OBJCOPY) -O binary $^ $@

######## Create S-record ########
$(TARGET_SREC): $(TARGET_EXE)
	$(OBJCOPY) -O srec $^ $@

######## Create Intel HEX ########
$(TARGET_HEX): $(TARGET)
	$(OBJCOPY) -O ihex $^ $@

######## Include ########
-include $(DPNS)

