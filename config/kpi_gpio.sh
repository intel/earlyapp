#!/bin/bash

GPIO=$1
GPIODIR=/sys/class/gpio/gpio$GPIO

echo "Configuring GPIO $GPIO"

#check if the gpio is already exported
if [ ! -e "$GPIODIR" ]
then
	echo "Exporting GPIO"
	echo $GPIO > /sys/class/gpio/export
	echo "Set GPIO as output"
	echo "out" > $GPIODIR/direction
else
	echo "GPIO already exported"
fi

echo "Set value as high"
echo $2 > $GPIODIR/value
