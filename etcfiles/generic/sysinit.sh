#!/bin/sh

# This script will be called when system is initing.
# You can mount filesystems, start system services, etc. in this time.

# Warning: The script is only support Linux system now!

# the 'mountfs.sh' is used by no_initramfs defaultly.
[ -x '/etc/ueld/mountfs.sh' ] && . /etc/ueld/mountfs.sh

# start udevd daemon process and create device files in /dev.
echo "[sysinit.sh] starting udev..."
udevd --daemon
udevadm trigger
udevadm settle
echo "[sysinit.sh] udev is running..."

# mount /dev/shm
mkdir /dev/shm
mount -t tmpfs -o nosuid,nodev tmpfs /dev/shm

# create dir '/run/lock'
mkdir -p /run/lock

exit 0
