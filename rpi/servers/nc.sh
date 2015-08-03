#!/bin/sh

: ${DIR:?"Need to set the DIR environment variable"}

echo "Starting USBTMC server on port 8000" 
insmod "$DIR/kernel-module/usbtmc.ko"
while true; do nc -l -p 8000 -c "$DIR/userland-scripts/talk2scope.sh" ; done
