#!/bin/bash
: '
    test.sh - Run tests and report results
    Copyright (C) 2020 Ethan Onstott

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
'
if [ ! -e ../tests/compiled ]; then
	mkdir ../tests/compiled
else
	rm -r ../tests/compiled/*
fi

cd ..
cmake .
make -j$(nproc)
total=0
compiled=0
executed=0

cd scripts

for i in $( ls ../tests | grep .storm ); do
	# compile test
	if ../storm -c ../tests/$i ../tests/compiled/${i}c; then
		echo $i compiled successfully
		compiled=$((compiled + 1))

		# execute test
		# don't output program text if not error
		if ../storm ../tests/compiled/${i}c > /dev/null; then
			echo $i executed successful
			executed=$((executed + 1))
		elif [ $1 = "--nofail" ]; then
			echo $i failed to execute
			exit 0
		fi
	elif [[ $1 = "--nofail" ]]; then
		echo $i failed to compile
		exit 1
	fi
	total=$((total + 1))
done

echo $executed successful out of $compiled compiled. $'\n'$total total tests