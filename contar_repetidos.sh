#!/bin/sh

r=0
arch=$1

for i in `cat $arch`; do
  if [ `grep $i $arch | wc -l` -gt 1 ]; then
    r=`expr $r + 1`
    echo "Repetido: " $i
  fi
done

echo "Total " $r
