#obj-m += hello.o
#obj-m += filtericmp.o
#obj-m += rootkit.o
#obj-m += rickroll.o
#obj-m += excited_virus.o
#obj-m += hello-packet.o
#obj-m += mmap/mymmap.o
#obj-m += thread/kthread.o
#obj-m += ioctl/ioctl_basic.o
#obj-m += excited_virus.o
#obj-m += lock/mutex_sample.o
#obj-m += lock/spin_sample_interrupt.o
#obj-m += lock/sample_refcnt.o
#obj-m += netfilter/first_sample.o
#obj-m += netfilter/firewall.o
#obj-m += socket/udpsend.o
#obj-m += notify_chain/test_chain.o
#obj-m += notify_chain/regchain.o
#obj-m += notify_chain/notify.o
#obj-m += nf_conntrack/helper.o
#obj-m += netfilter/hooktest.o
obj-m += netfilter/skbudp.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
