#!/bin/sh

mkdir -p ${PREFIX}${INSTALLDIR}
rm -f ${PREFIX}${INSTALLDIR}/ueld*
cp ueld ${PREFIX}${INSTALLDIR}
ln -s -f ${INSTALLDIR}/ueld ${PREFIX}/sbin/init

cp usertool/ueldctl ${PREFIX}${INSTALLDIR}/ueldctl
ln -s -f ${INSTALLDIR}/ueldctl ${PREFIX}/sbin/ueldctl

if [ -d "${PREFIX}/etc/ueld" ]; then
	prinfo "NOTE: old /etc/ueld move to /etc/ueld.old"
	mv /etc/ueld /etc/ueld.old
fi

mkdir -p ${PREFIX}/etc/ueld
cp -n etcfiles/generic-${OS}/* ${PREFIX}/etc/ueld
cp -n etcfiles/generic/* ${PREFIX}/etc/ueld
touch ${PREFIX}/etc/ueld/*
chmod 755 ${PREFIX}/etc/ueld/*.sh
