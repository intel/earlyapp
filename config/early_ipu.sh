#!/bin/bash

for i in {1..32}
do
	/usr/bin/chown :ias /dev/intel_stream27 /dev/intel_pipeline /dev/ipu-psys0 2>&1
	[ $? -eq 0 ] && break;
	sleep 0.05
done

