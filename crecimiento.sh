#!/bin/sh
for i in `seq 1 100`; do
  $1 -f colecciones/coleccion_$i.csv >> info.dat
done
