#!/usr/bin/bash

[[ -z "$1" ]] && src="ubuntu-supported.svg" || type=$1
[[ -z "$2" ]] && type="emblems" || type=$2

out=${src%.*}

for i in 16 22 24 32 48 64 128 256
do
  inkscape -w $i -h $i $src -o $i-$type-$out.png
done
