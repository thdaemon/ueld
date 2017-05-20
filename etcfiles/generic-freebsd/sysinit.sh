#!/bin/sh

# This script will be called when system is initing.
# You can mount filesystems, start system services, etc. in this time.

mount -o rw /

[ -x '/etc/ueld/mountfs.sh' ] && . /etc/ueld/mountfs.sh

exit 0
