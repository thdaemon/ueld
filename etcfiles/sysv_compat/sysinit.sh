#!/bin/sh

# This script will be called when system is initing.
# You can mount filesystems, start system services, etc. in this time.

# This is compat-sysvinit sysinit.sh

runlevel=5

/etc/init.d/rc S
/etc/init.d/rc ${runlevel}

[ -x /etc/ueld/u_sysinit.sh ] && /etc/ueld/u_sysinit.sh

exit 0
