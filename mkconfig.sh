#!/bin/sh

add_macro()
{
	local line="#define $1"

	if [ "$2" != "" ]; then
		line="$line $2"
	fi

	echo $line >> config.h
}

cat > config.h <<EOF
#ifndef __CONFIG_H_
#define __CONFIG_H_

EOF

while true ; do
	[ "$1" = "" ] && break

	case "$1" in
	--no-build-in-minit)
		add_macro CONFIG_NO_BUILD_IN_MINIT
		shift
		;;
	--term-waittime)
		add_macro CONFIG_TERM_WAITTIME $2
		shift 2
		;;
	*)
		shift
		;;
	esac
done

cat >> config.h << EOF

#endif /* __CONFIG_H_ */
EOF
