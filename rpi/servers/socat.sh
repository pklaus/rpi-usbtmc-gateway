#!/bin/sh

: ${DIR:?"Need to set the DIR environment variable"}

echo "Starting USBTMC server on port 5025"

insmod "$DIR/kernel-module/usbtmc.ko"

#socat tcp-listen:5025,fork,reuseaddr,crnl EXEC:"$DIR/userland-scripts/talk2scope.sh",su-d=pi,pty,echo=0
socat tcp-listen:5025,fork,reuseaddr,crnl EXEC:"$DIR/userland-scripts/talk2scope.sh",pty,echo=0

