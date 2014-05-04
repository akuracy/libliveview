CC = cc
CFLAGS = -O2 -lbluetooth -g
CFLAGS += -I.. -L.. -lliveview
CFLAGS += -lpthread -D_REENTRANT
CFLAGS += -Wall -pedantic -Wextra -Wshadow -Wpointer-arith -Wcast-qual \
	  -Wstrict-prototypes -Wmissing-prototypes
CC_COMPILE = $(CC) $(CFLAGS) -c

all: demo 

.PHONY: demo
demo: main.c
	$(CC) $^ ${CFLAGS} -lliveview

.PHONY: run
run: demo
	./a.out

.PHONY: run_loop
run_loop: demo
	while [ 1 ]; do ./a.out; done

clean:
	@echo cleaning directory
	@rm -f *.o a.out

indent:
	@indent -linux *.c *.h
