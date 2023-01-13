obj-m += devprintk.o
.PHONY = clean all

CC = gcc
BINARY = printk

all: module binary

module:
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) modules

binary:
	$(CC) -o $(BINARY) $(BINARY).c

install: all
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) modules_install
	install $(BINARY) -t /usr/bin/ 

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) clean
	-rm $(BINARY)

