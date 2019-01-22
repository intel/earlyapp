#!/bin/bash
#set -x

grep "mem=4096" /proc/cmdline > /dev/null || exit
addr=0x100000000

cd /sys/devices/system/memory

function online_memsec()
{
	addr=$1
	echo $addr > probe 
	let index=addr/0x8000000
	echo online > memory$index/state 
}

# mem section is 256MB each, and the physical addr range
# is from 0x100000000 - 0x280000000 (4G - 10G)
for i in `seq 1 48`
do
	online_memsec $addr 
	let addr=addr+0x8000000
done

wait



