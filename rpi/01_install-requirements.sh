#!/bin/bash

if [[ $EUID -ne 0 ]]; then
   echo "This script must be run as root" 1>&2
   exit 1
fi

pacman -Sy
for package in wget base-devel vim openbsd-netcat socat
do
  echo -e "\ninstalling $package\n"
  pacman -S --needed --noconfirm $package
done

pacman -S --needed --noconfirm linux-raspberrypi-headers
if [ $? -ne 0 ]; then
  echo -e "\n--------------------"
  echo -e "You're not on a Raspberry Pi? Install the matching kernel header package yourself..."
  echo -e "To make that easier, listed below are the installed packages whose name start with linux."
  echo -e "If your kernel is just 'linux' then the headers package is linux-headers."
  echo -e "--------------------\n"
  pacman -Q | grep -e "^linux"
fi


