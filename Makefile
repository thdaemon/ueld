





UELD_OS := unkown
include version


PREFIX :=
INSTALLDIR := /usr/local/sbin/ueld/ueld-$(UELD_VERSION)
ETCDIR := $(PREFIX)/etc/ueld

OBJS := main.o minit.o readline.o reboot.o tools.o restarts.o os/$(UELD_OS)/pw.o
CROSS :=
CC := gcc
STRIP := strip

CFLAG := -Wall -O2 -std=c99 $(UELD_C_DEF)

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
	touch $(ETCDIR)/sysinit.sh $(ETCDIR)/sysloaded.sh $(ETCDIR)/syshalt.sh $(ETCDIR)/ueld.conf $(ETCDIR)/restarts.list
	chmod 755 $(ETCDIR)/*.sh

	cp usertool/ueldctl $(PREFIX)$(INSTALLDIR)/ueldctl
	ln -s -f $(PREFIX)$(INSTALLDIR)/ueldctl /sbin/ueldctl

test:
	@echo "FIX ME: Need a test target"

clean:
	rm -f *.o os/$(UELD_OS)/*.o ueld
