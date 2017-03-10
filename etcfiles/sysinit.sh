#!/bin/sh

# This script will be called when system is initing.
# You call mount filesystems, start system services, etc. in this time.

# Example: start udevd daemon process and create device files in /dev.

echo "[sysinit.sh] starting udev..."
#setsid udevd > /dev/null 2>&1 &
udevd --daemon
udevadm trigger
udevadm settle
echo "[sysinit.sh] udev is running..."

exit 0
