#!/bin/bash

if [[ $EUID -ne 0 ]]; then
   echo "This script must be run as root" 1>&2
   exit 1
fi

prev_wd="$(pwd)"

# go to script dir...
cd "$( dirname "${BASH_SOURCE[0]}" )"

echo -e "\n---------------"
echo "Creating the usbtmc group"
groupadd usbtmc
echo -e "Add all users you want to access the /dev/usbtmcX files to this group:\n  usermod -a -G usbtmc your_username"
echo -e "---------------\n"

echo -e "\n---------------"
echo "Setting up the udev rules"
cp udev-rules/98-usbtmc.rules /etc/udev/rules.d/
udevadm control --reload-rules
echo -e "---------------\n"

echo -e "\n---------------"
echo "Setting up the startup service"
cp system-startup/usbtmc.service /etc/systemd/system/
systemctl daemon-reload
echo -e "\nSet up the systemd service: usbtmc"
echo "You can start it with:"
echo " systemctl start usbtmc"
echo "To check its status, run:"
echo " systemctl start usbtmc"
echo "To enable it at system startup, run:"
echo " systemctl enable usbtmc"
echo -e "---------------\n"


cd "$prev_wd"
