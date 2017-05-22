#!/bin/sh

# This script will be called when user power-off the machine.
# When system halt, we should turn off all swap.
# Need NOT sync and umount filesystems, ueld or kernel will do it!

swapoff -a

exit 0
