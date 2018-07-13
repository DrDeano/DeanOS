CC = gcc
CFLAGS = -m32 -masm=intel -Wall -Wextra -std=c11 -O2 -c -ffreestanding -Iinclude -I../kernel/include -D__is_libk

AR = ar
ARFLAGS = -rcs

all: clean libk.a

FREEOBJS=\
$(ARCH_FREEOBJS) \
stdio.o \
stdlib.o \
string.o \
time.o
 
HOSTEDOBJS=\
$(ARCH_HOSTEDOBJS) \
 
OBJS=\
$(FREEOBJS) \
$(HOSTEDOBJS) \

%.o: %.c
	$(CC) $< -o $@ $(CFLAGS)
	
libk.a: $(OBJS)
	$(AR) $(ARFLAGS) $@ $^

clean:
	rm -f *.o
	rm -f *.d
	rm -f libk.a