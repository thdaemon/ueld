#!/bin/sh

# When ueld has runed in a system which is not have initramfs (or initrd), ueld need mount some filesystems by itself.

# Warning: The script is only support Linux system now!

mount -t proc proc /proc
mount -t sysfs sys /sys
mount -t devtmpfs udev /dev
mkdir -p /dev/pts
mount -t devpts -o nosuid,noexec,gid=5,mode=620 devpts /dev/pts
mount -t tmpfs -o nosuid,noexec,mode=755,size=83024k tmpfs /run
