#!/bin/sh

: ${DIR:?"Need to set the DIR environment variable"}

echo "Starting USBTMC server on port 5025" 
insmod "$DIR/kernel-module/usbtmc.ko"
while true; do nc -l -p 5025 -c "$DIR/userland/talk2scope.sh" ; done
