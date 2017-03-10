





UELD_OS := unkown
include version

ifeq "$(PLATFORM)" ""
 PLATFORM := $(shell ./systype.sh)
endif
include Make.defines.$(PLATFORM)


PREFIX :=
INSTALLDIR := /usr/local/sbin/ueld/ueld-$(UELD_VERSION)
ETCDIR := $(PREFIX)/etc/ueld

OBJS := main.o minit.o readline.o reboot.o tools.o restarts.o os/$(UELD_OS)/pw.o os/$(UELD_OS)/chvt.o os/$(UELD_OS)/ctrlaltdel.o
CROSS :=
CC := gcc
STRIP := strip

CFLAG := -Wall -O2 -std=c99 $(UELD_OS_CFLAGS)

all : ueld

ueld : $(OBJS)
	@echo "  LD	$@"
	@$(CROSS)$(CC) -o ueld $(OBJS)

	@echo "  STRIP	$@"
	@$(CROSS)$(STRIP) -s $@

%.o : %.c
	@echo "  CC	$@"
	@$(CROSS)$(CC) -c -o $@ $(CFLAG) $<

install : ueld
	mkdir -p $(PREFIX)$(INSTALLDIR)
	rm -f $(PREFIX)$(INSTALLDIR)/ueld*
	cp ueld $(PREFIX)$(INSTALLDIR)

	ln -s -f $(PREFIX)$(INSTALLDIR)/ueld /sbin/init

	mkdir -p $(ETCDIR)
	cp -n etcfiles/* $(ETCDIR)/
	touch $(ETCDIR)/*
	chmod 755 $(ETCDIR)/*.sh

	cp usertool/ueldctl $(PREFIX)$(INSTALLDIR)/ueldctl
	ln -s -f $(PREFIX)$(INSTALLDIR)/ueldctl /sbin/ueldctl

test:
	@echo "FIX ME: Need a test target"

clean:
	rm -f *.o os/$(UELD_OS)/*.o ueld
