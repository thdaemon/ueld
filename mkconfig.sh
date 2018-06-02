#!/bin/sh

is_int_or_float()
{
	local rval=0

	echo $1 | grep "[^\.0-9]" > /dev/null 2>&1 && rval=1
	echo $1 | grep "^.*\..*\..*$" > /dev/null 2>&1 && rval=1
	return ${rval}
}

add_macro()
{
	local line="#define $1"
	if [ "$2" != "" ]; then
		if is_int_or_float $2; then
			line="$line $2"
		else
			line="$line \"$2\""
		fi
	fi

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

add_macro PROJECT_VERSION "`cat version`"

cat >> config.h << EOF

#endif /* __CONFIG_H_ */
EOF
