#!/bin/bash -e

PRELOAD_LIST_FILE=$DESTDIR/$1
shift

rm -f $PRELOAD_LIST_FILE
for f in $@; do
	echo $f >> $PRELOAD_LIST_FILE
done

while true; do
	found_new=n
	for _f in `cat $PRELOAD_LIST_FILE`; do
		if [ ! -e $_f ]; then
			f=$DESTDIR/$_f
		else
			f=$_f
		fi
		if ! /usr/lib64/ld-linux-x86-64.so.2 --list $f >/dev/null 2>&1; then
			continue
		fi
		for f_lib in `/usr/lib64/ld-linux-x86-64.so.2 --list $f | grep '=>' | grep -o '/usr/lib[^ ]*'`; do
			if ! grep -q "^$f_lib$" $PRELOAD_LIST_FILE; then
				found_new=y
				echo $f_lib >> $PRELOAD_LIST_FILE
			fi
		done
	done
	if [ "$found_new" == "n" ]; then
		break;
	fi
done
