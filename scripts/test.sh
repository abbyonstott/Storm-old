#!/bin/bash

cd ../build

if [ ! -e storm ]; then
	echo Storm must be built > /dev/stderr
	exit 1
elif [ "$(objdump --syms storm | grep debug)" = "" ]; then
	echo $0: Please compile with debug > /dev/stderr
	exit 1
fi

if [ ! -e ../tests/compiled ]; then
	mkdir ../tests/compiled
else
	rm -r ../tests/compiled/*
fi

total=0
compiled=0
executed=0

for i in $( ls ../tests | grep .storm ); do
	echo Test $((total + 1)):
	if ./storm -c ../tests/$i ../tests/compiled/${i}c 2>> compilefailed.txt; then
		compiled=$((compiled + 1))

		# execute test
		if ./storm ../tests/compiled/${i}c 2>> runfailed.txt; then
			echo $i executed successful
			executed=$((executed + 1))
		else
			echo - file $i >> runfailed.txt
			 # newline between files to avoid confusion
			echo >> runfailed.txt
			echo $i failed to execute
		fi
	else
		echo - file $i >> compilefailed.txt
		echo >> compilefailed.txt
		echo $i failed to compile
	fi

	echo
	total=$((total + 1))
done

echo $executed successful out of $compiled compiled. $'\n'$total total tests

# print errors if failed to compile
if [ $executed -ne $total ] || [ $compiled -ne $total ]; then
	echo

	# check if empty as to not print header with no actual errors
	if [ -s compilefailed.txt ]; then
		echo Compile errors:
		cat compilefailed.txt
	fi

	if [ -s runfailed.txt ]; then
		echo Runtime errors:
		cat runfailed.txt
	fi
	
	rm compilefailed.txt
	rm runfailed.txt

	cd ../scripts
	exit 1
fi

rm compilefailed.txt
rm runfailed.txt
cd ../scripts