#!/bin/bash
for i in {1..16}
do
    /usr/bin/chown :ias /dev/cbc-early-signals /sys/class/gpio/gpio442/value 2>&1
    [ $? -eq 0 ] && break
    sleep 0.01
done
