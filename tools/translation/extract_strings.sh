#!/bin/sh

if [ "$1" == "" ] ; then
	echo "Usage: extract_strings.sh <English.utf8> <directories.txt> <objname>";
	exit;
fi

cat $1 | sed -e 's/^[0-9]\+://' > __tmp__.txt

for f in `cat $2` ; do
	for g in $f/* ; do
		./print_strings.exe < $g >> __tmp__.txt
	done
done

sort __tmp__.txt | uniq > __tmp2__.txt
rm -f __tmp__.txt
rm -f __final__.txt

cat $1 | sed -e 's/^[0-9]\+://' | sort > __a__.txt

if [ -f __a__.txt ] ; then
	comm -3  __a__.txt __tmp2__.txt | sed -e 's/\t//' > __tmp3__.txt
else
	cp __tmp2__.txt __tmp3__.txt
fi

rm -f __a__.txt
rm -f __tmp2__.txt

num_existing=`wc -l $1 | cut -f1 -d' '`
count="0"

while read -r line
do
	n=$((num_existing + count))
	count=$((count+1))
	echo "$n:$line" >> __final__.txt
done < __tmp3__.txt

cat __final__.txt >> $1

rm -f __final__.txt
rm -f __tmp3__.txt

cat $1 | sed -e 's/^[0-9]\+://' > __all__.txt

for f in `cat $2` ; do
	for g in $f/* ; do
		./substring.exe $3 __all__.txt $g
		dos2unix < $g > __tmp__.txt
		mv __tmp__.txt $g
	done
done

rm -f __all__.txt