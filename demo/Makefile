CC = cc
CFLAGS = -O2 -lbluetooth -g
CFLAGS += -I.. -L.. -lliveview
CFLAGS += -Wall -pedantic -Wextra -Wshadow -Wpointer-arith -Wcast-qual \
	  -Wstrict-prototypes -Wmissing-prototypes
CC_COMPILE = $(CC) $(CFLAGS) -c

all: demo 

demo: main.c
	$(CC) $^ ${CFLAGS} -lliveview

clean:
	@echo cleaning directory
	@rm -f *.o a.out

indent:
	@indent -linux *.c *.h
