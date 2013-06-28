#!/bin/sh

start="1.0"
end="1.0e+30"
frames="14400"

magn=`python -c "from math import pow; fac = pow($end / $start, 1.0 / ($frames - 1)); print \"\\n\".join([ str($start * pow(fac, i-1)) for i in range(1, $frames + 1)])"`

centerx="-0.743643887037158704752191506114774"
centery="0.131825904205311970493132056385139"

i=0
for mag in $magn; do
	pngname=`printf "out_%05d_%s.png" $i $mag`
	if [ -s "$pngname" ]; then
		i=$((i+1))
		continue
	fi
	echo $i $mag
	fname=`printf "out_%05d_%s.ppm" $i $mag`
	/usr/bin/time -f "%e s" ./mandel_dd 320 240 $centerx $centery $mag > "$fname"
	convert "$fname" -format png "$pngname"
	rm "$fname"
	i=$((i+1))
done
