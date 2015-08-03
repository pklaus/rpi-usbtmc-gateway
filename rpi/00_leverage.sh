#!/bin/bash

if [[ $EUID -ne 0 ]]; then
   echo "This script must be run as root" 1>&2
   exit 1
fi

pacman -Sy
pacman -S --needed git
cd /opt
git clone https://github.com/pklaus/rpi-usbtmc-gateway.git usbtmc
cd usbtmc

echo -e "\nnow run:\n"

echo "./01_install-requirements.sh"
echo "./02_compile.sh"
echo "./03_setup-and-install.sh"


