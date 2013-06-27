#!/bin/sh

start="1.0"
end="1.0e+32"
frames="100"

magn=`python -c "from math import pow; fac = pow($end / $start, 1.0 / ($frames - 1)); print \"\\n\".join([ str($start * pow(fac, i-1)) for i in range(1, $frames + 1)])"`

centerx="-0.743643887037158704752191506114774"
centery="0.131825904205311970493132056385139"

i=0
for mag in $magn; do
	echo $mag
	fname=`printf "out_%04d_%s.ppm" $i $mag`
	./a.out 320 240 $centerx $centery $mag > "$fname"
	i=$((i+1))
done
