#!/bin/sh
# Usage: ./talk2scope.sh /dev/usbtmc0 < scpi_commands > output_file

: ${DIR:?"Need to set the DIR environment variable"}

if [ -z $1 ]; then DEV=/dev/usbtmc0 ; else DEV=$1 ; fi
while read line; do
    if [ -e ${DEV} ] ; then
	sleep 0.01
	echo $line > ${DEV}
	sleep 0.01
	if [[ $line == *\?* ]] ; then
	    "$DIR/userland/cat2" < ${DEV}
	fi
    else
	echo "Device ${DEV} does not exist"
    fi
done
