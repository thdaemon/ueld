#!/bin/sh

#Example: start udevd daemon process and create device files in /dev.
setsid udevd > /dev/null 2>&1 &
echo "udev is running..."
udevadm trigger
udevadm settle
echo "udev is ok..."

exit 0
