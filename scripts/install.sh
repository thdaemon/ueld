#!/usr/bin/env bash

prinfo() {
	echo "[ INFO ]" "$*"
}

prerr() {
	echo "[ ERRO ]" "$*"
}

ckev() {
	if [ "$1" = "0" ]; then	
		return
	fi

	prerr "Fatal error. install cannot continue!"
	prerr "System ALMOST is in an inconsistent state!!!!! :-("
	exit 0
}

errquit() {
	prerr "$*"
	exit 1
}

PREFIX=''

while true ; do
	opt="$1"
	if [ "$opt" = "" ] || [ "$opt" = "--" ]; then
		shift
		break
	fi
	
	if [ "${opt:0:1}" = "-" ]; then
		shift

		case "${opt}" in
		--prefix)
			if [ "${1:0:2}" != "--" ]; then
				PREFIX="$1"
				shift
			fi
			;;
		--system)
			OS="$1"
			shift
			;;
		--uninstall)
			UNINSTALL="1"
			;;
		*)
			errquit "Unknow option ${opt}"
			;;
		esac
	else
		break
	fi
done

[ -z "${OS}" ] && errquit "--system can not be blank"

[ "${UNINSTALL}" = "1" ] && errquit "uninstall is not supported"

INSTALLDIR="/lib/ueld/ueld-`cat version`"

prinfo "install core files to ${PREFIX}${INSTALLDIR}"

mkdir -p ${PREFIX}${INSTALLDIR}
ckev $?
rm -f ${PREFIX}${INSTALLDIR}/ueld*
ckev $?
cp ueld ${PREFIX}${INSTALLDIR}
ckev $?
ln -s -f ${INSTALLDIR}/ueld ${PREFIX}/sbin/init
ckev $?

cp usertool/ueldctl ${PREFIX}${INSTALLDIR}/ueldctl
ckev $?
ln -s -f ${INSTALLDIR}/ueldctl ${PREFIX}/sbin/ueldctl
ckev $?

if [ -d "${PREFIX}/etc/ueld" ]; then
	prinfo "old ${PREFIX}/etc/ueld move to ${PREFIX}/etc/ueld.old"
	rm -rf "${PREFIX}/etc/ueld.old"
	ckev $?
	mv "${PREFIX}/etc/ueld" "${PREFIX}/etc/ueld.old"
	ckev $?
fi

prinfo "install config files to ${PREFIX}/etc/ueld"

mkdir -p ${PREFIX}/etc/ueld
ckev $?
cp -n etcfiles/generic-${OS}/* ${PREFIX}/etc/ueld
ckev $?
cp -n etcfiles/generic/* ${PREFIX}/etc/ueld
ckev $?
touch ${PREFIX}/etc/ueld/*
ckev $?
chmod 755 ${PREFIX}/etc/ueld/*.sh
ckev $?
