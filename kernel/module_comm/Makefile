#obj-m += hello.o
obj-m += mod_a.o
obj-m += mod_b.o
KBUILD_EXTRA_SYMBOLS += /path/to/ModuleA/Module.symvers
export KBUILD_EXTRA_SYMBOLS
#obj-m += rootkit.o
#obj-m += rickroll.o
#obj-m += excited_virus.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
