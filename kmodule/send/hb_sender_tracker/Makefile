EXTRA_CFLAGS := -I./

obj-m += hb_sender_tracker.o 
hb_sender_tracker-objs := debugfs.o kretprobe.o netfilter.o main.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
