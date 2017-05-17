UELD_OS := unkown
include version

#ifeq "$(PLATFORM)" ""
# PLATFORM !=./systype.sh
#endif
include Make.defines.$(PLATFORM)


PREFIX :=
INSTALLDIR := /usr/lib/ueld/ueld-$(UELD_VERSION)
ETCDIR := $(PREFIX)/etc/ueld

OBJS := main.o minit.o fileio.o reboot.o tools.o respawn.o os/$(UELD_OS)/pw.o os/$(UELD_OS)/chvt.o os/$(UELD_OS)/ctrlaltdel.o os/$(UELD_OS)/proc.o os/$(UELD_OS)/mnt.o
CROSS :=
#ifeq "$(CC)" ""
# CC := gcc
#endif
STRIP := strip

CFLAG := -Wall -O2 -std=c99 $(UELD_OS_CFLAGS)

all : ueld

ueld : $(OBJS)
	@echo "  LD	$@"
	@$(CROSS)$(CC) -o ueld $(OBJS)

	@echo "  STRIP	$@"
	@$(CROSS)$(STRIP) -s $@

.c.o:
#%.o : %.c config.h
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
	cp -n etcfiles/generic/* $(ETCDIR)/
	touch $(ETCDIR)/*
	chmod 755 $(ETCDIR)/*.sh

install_no_initramfs:
	make install_ueld_executable

	mkdir -p $(ETCDIR)
	cp -n etcfiles/no_initramfs/* $(ETCDIR)/
	touch $(ETCDIR)/*
	chmod 755 $(ETCDIR)/*.sh

	make install_generic_etc_file

install:
	make install_ueld_executable
	make install_generic_etc_file

test:
	@echo "FIXME: Need a test target"

clean:
	rm -f *.o os/$(UELD_OS)/*.o config.h ueld
