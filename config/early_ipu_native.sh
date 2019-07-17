#!/bin/bash

for i in {1..32}
do
	/usr/bin/chown :ias /dev/media0 /dev/ipu-psys0 2>&1
	[ $? -eq 0 ] && break;
	sleep 0.05
done

