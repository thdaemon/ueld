#!/bin/sh

#When system halt, we should turn off all swap and dettach all loop device.
#Need NOT umount filesystems, ueld will do it!

swapoff -a
for i in `ls /dev/loop*`
do
	losetup -d $i 2>/dev/null
done

exit 0
