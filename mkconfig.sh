#!/bin/sh

add_macro()
{
	local line="#define $1"
	[ "$2" != "" ] && line="$line $2"

	echo $line >> config.h
}

cat > config.h <<EOF
#ifndef __CONFIG_H_
#define __CONFIG_H_

EOF

while true ; do
	[ "$1" = "" ] && break

	value=${1#*=}
	[ "$value" = "$1" ] && value=''

	#macro=`expr substr ${1%=*} 3 999`
	macro=${1%=*}
	macro=${macro#--}
	macro=`echo $macro | tr "a-z-" "A-Z_"`
	macro="CONFIG_$macro"
	add_macro $macro $value

	shift
done

cat >> config.h << EOF

#endif /* __CONFIG_H_ */
EOF
