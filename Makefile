CC = cc
CFLAGS = -O2 -lbluetooth -g
CFLAGS += -Wall -pedantic -Wextra -Wshadow -Wpointer-arith -Wcast-qual \
	  -Wstrict-prototypes -Wmissing-prototypes
CC_COMPILE = $(CC) $(CFLAGS) -c

OBJFILES := libliveview.o libliveview.o msg.o img.o

all: liveview

.PHONY: liveview

liveview : $(OBJFILES)
	$(AR) rcs libliveview.a $(OBJFILES)

.PHONY: demo
demo: liveview
	$(MAKE) -C demo

.PHONY: run
run: demo
	cd demo ; ./a.out

.PHONY: run_loop
run_loop: demo
	cd demo ; while [ 1 ]; do ./a.out; done

%.o: %.c
	$(CC_COMPILE) -o $@ $<

clean:
	$(MAKE) -C demo clean
	@echo cleaning directory
	@rm -f *.o *.a a.out

indent:
	@indent -linux *.c *.h
