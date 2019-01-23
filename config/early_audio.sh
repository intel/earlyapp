#!/bin/bash

#/bin/mkdir -m 0770 -p /dev/snd
#/bin/mknod -m 0666 /dev/snd/pcmC0D0p c 116 2
#/bin/mknod -m 0666 /dev/snd/controlC0 c 116 18
#/bin/mknod -m 0666 /dev/snd/timer c 116 33
#/bin/chgrp -R audio /dev/snd

# load audio driver
modprobe snd_soc_skl
modprobe snd_soc_tdf8532
modprobe snd_soc_sst_bxt_tdf8532

# turn on codec0_out Switch
for i in {1..16}
do
	echo "EA: mixer cset $i" > /dev/kmsg
	/usr/bin/amixer -c0 cset numid=19 On > /dev/kmsg 2>&1
	[ $? -eq 0 ] && break;
	sleep 0.1
done

# early audio test
aplay -Dplughw:0,0 `dirname $0`/jingle.wav --duration 1
