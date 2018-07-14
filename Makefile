UELD_OS := unkown
UELD_VERSION := $(shell cat version)

ifeq "$(PLATFORM)" ""
 PLATFORM := $(shell ./systype.sh)
endif
include Make.defines.$(PLATFORM)


PREFIX :=
INSTALLDIR := /lib/ueld/ueld-$(UELD_VERSION)
ETCDIR := $(PREFIX)/etc/ueld

OBJS := main.o minit.o fileio.o reboot.o tools.o respawn.o log.o os/$(UELD_OS)/pw.o os/$(UELD_OS)/chvt.o os/$(UELD_OS)/ctrlaltdel.o os/$(UELD_OS)/proc.o os/$(UELD_OS)/mnt.o os/$(UELD_OS)/klog.o
CROSS :=
ifeq "$(CC)" ""
 CC := gcc
endif
STRIP := strip

CFLAG := -Wall -O2 -std=c99 -Wno-unused-result $(UELD_OS_CFLAGS)

all : ueld

ueld : $(OBJS)
	@echo "  LD	$@"
	@$(CROSS)$(CC) -o ueld $(OBJS)

	@echo "  STRIP	$@"
	@$(CROSS)$(STRIP) -s $@

%.o : %.c config.h
	@echo "  CC	$@"
	@$(CROSS)$(CC) -c -o $@ $(CFLAG) $<

config.h:
	@echo "Please use ./mkconfig.sh to generate a config header."
	@false

install_ueld_executable : ueld
	mkdir -p $(PREFIX)$(INSTALLDIR)
	rm -f $(PREFIX)$(INSTALLDIR)/ueld*
	cp ueld $(PREFIX)$(INSTALLDIR)
	ln -s -f $(INSTALLDIR)/ueld $(PREFIX)/sbin/init

	cp usertool/ueldctl $(PREFIX)$(INSTALLDIR)/ueldctl
	ln -s -f $(INSTALLDIR)/ueldctl $(PREFIX)/sbin/ueldctl

install_generic_etc_file:
	mkdir -p $(ETCDIR)
	cp -n etcfiles/generic-$(UELD_OS)/* $(ETCDIR)/
	cp etcfiles/generic/* $(ETCDIR)/
	touch $(ETCDIR)/*
	chmod 755 $(ETCDIR)/*.sh

install_no_initramfs:
	@$(MAKE) install_ueld_executable

	mkdir -p $(ETCDIR)
	cp -n etcfiles/no_initramfs/* $(ETCDIR)/
	touch $(ETCDIR)/*
	chmod 755 $(ETCDIR)/*.sh

	@$(MAKE) install_generic_etc_file

install_sysv_compat:
	@$(MAKE) install_ueld_executable

	mkdir -p $(ETCDIR)
	cp -n etcfiles/sysv_compat/* $(ETCDIR)/
	touch $(ETCDIR)/*
	chmod 755 $(ETCDIR)/*

	@$(MAKE) install_generic_etc_file

install:
	@$(MAKE) install_ueld_executable
	@$(MAKE) install_generic_etc_file

defconfig:
	./mkconfig.sh --no-build-in-minit --enable-log --enable-log-file --enable-log-syslog --enable-os-klog

test:
	@echo "FIXME: Need a test target"

clean:
	rm -f *.o os/$(UELD_OS)/*.o config.h ueld
