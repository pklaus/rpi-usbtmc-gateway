 echo "Starting USBTMC server on port 8000 and 8001" 
 insmod /home/pi/usbtmc/usbtmc.ko
 while true; do nc.traditional -l -p 8000 -c /home/pi/usbtmc/talk2scope.sh ; done &
 while true; do nc.traditional -l -p 8001 -c "/home/pi/usbtmc/talk2scope.sh /dev/usbtmc1" ; done &
