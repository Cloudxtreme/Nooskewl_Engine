#!/bin/sh

# This version runs on Windows using MSYS tools. You may need to change the
# following path... Windows has a built-in find.exe that doesn't work with
# this script.

FILES=`/c/mingw/msys/1.0/bin/find * -type f | grep -v README.txt | grep -v "^flp" | sort`

echo "Writing header..."
# the big space is a tab
du -bc $FILES | grep "	total$" | cut -f1 > $1

echo "Writing data..."
cat $FILES >> $1

echo "Writing info..."
du -b $FILES >> $1

echo "Saving uncompressed archive..."
cp $1 $1.uncompressed

echo "Compressing..."
gzip $1
mv $1.gz $1
