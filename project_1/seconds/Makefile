# "obj-m" means we want to compile this into a module
# the "*.o" corresponds to the "*.c" file you want to compile
obj-m += seconds.o

# "shell uname -r" will get your kernel version automatically 
KDIR := /lib/modules/$(shell uname -r)/build

# current working directory
PWD := $(shell pwd)

# default rule
all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

# clean rule
clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean