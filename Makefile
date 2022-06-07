obj-m +=  module_pi.o
module_pi-src := src/module_pi.c
module_pi-objs := $(module_pi-src:.c=.o) src/libGPIO.o 

#foo-objs by default is foo.o, in order to add objects its is necessary to include it again

KVERSION = $(shell uname -r)

all:
	make -C /lib/modules/$(KVERSION)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(KVERSION)/build M=$(PWD) clean