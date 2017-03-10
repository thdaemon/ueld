#!/bin/sh

case `uname -s` in
"Linux")
	OS="linux"
	;;
"FreeBSD")
	OS="freebsd"
	;;
"SunOS")
	OS="solaris"
	;;
"Darwin")
	OS="macosx"
	;;
*)
	echo "Error: Counld NOT get the type of your operating system." >&2
	echo "Please Use 'PLATFORM' set your os type, like 'make PLATFORM=linux'" >&2
	exit 1
esac
echo $OS
exit 0
