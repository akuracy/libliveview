CC = cc
CFLAGS = -O2 -lbluetooth
CFLAGS += -Wall -pedantic -Wextra -Wshadow -Wpointer-arith -Wcast-qual \
	  -Wstrict-prototypes -Wmissing-prototypes
CC_COMPILE = $(CC) $(CFLAGS) -c

OBJFILES := libliveview.h libliveview.c

all: liveview

liveview : $(OBJFILES)
	ar rcs libliveview.a $(OBJFILES)

test: liveview
	gcc test.c libliveview.c ${CFLAGS}

%.o: %.c
	$(CC_COMPILE) -o $@ $<

clean:
	@echo cleaning directory
	@rm -f *.o *.a a.out

indent:
	@indent -linux *.c *.h
