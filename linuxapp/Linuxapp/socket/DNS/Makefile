# Makefile
#

#CROSS_COMPILE = arm-linux-gnu-

CC = $(CROSS_COMPILE)gcc

ifdef CROSS_COMPILE
TARGET = /opt/filesystem
endif


#DEBUG = -g -O0 -Wall
DEBUG = -g -O2
CFLAGS += $(DEBUG)

PROGS = ${patsubst %.c, %, ${wildcard *.c}} 

all : $(PROGS)

install: $(PROGS)
ifdef CROSS_COMPILE
	mkdir $(TARGET)/root/net -p
	cp $(PROGS) $(TARGET)/root/net -f
endif
%.o : %.c
	$(CC)  $(CFLAGS) -c $< -o $@

.PHONY: uninstall clean dist

uninstall :
ifdef CROSS_COMPILE
	cd $(TARGET)/root/net && rm -f $(PROGS)
endif

clean : uninstall
	- rm -f $(PROGS) core *.gz

dist: clean
	tar czf ../makeru_2_dns_demo.tar.gz ../2_dns_demo	
