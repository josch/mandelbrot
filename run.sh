#!/bin/sh

if [ $# -ne 8 ]; then
	echo "usage: $0 startmag endmag frames method width height centerx centery"
	echo ""
	echo "   startmag  start magnification"
	echo "   endmag    end magnification"
	echo "   frames    number of frames to generate over the magnification range"
	echo "   method    name of executable to use for mandelbrot generation"
	echo "   width     width of the images"
	echo "   height    height of the images"
	echo "   centerx   x zoom coordinate"
	echo "   centery   y zoom coordinate"
	echo ""
	echo "example:"
	echo ""
	echo "   $0 1.0 1.0e+30 14400 mandel_dd_aa 1920 1080 -0.743643887037158704752191506114774 0.131825904205311970493132056385139"
	exit
fi

start="$1"
end="$2"
frames="$3"
method="$4"
width="$5"
height="$6"
centerx="$7"
centery="$8"

magn=`python -c "from math import pow; fac = pow($end / $start, 1.0 / ($frames - 1)); print \"\\n\".join([ str($start * pow(fac, i-1)) for i in range(1, $frames + 1)])"`

i=0
for mag in $magn; do
	pngname=`printf "out_%05d.png" $i`
	if [ -s "$pngname" ]; then
		i=$((i+1))
		continue
	fi
	echo $i $mag
	fname=`printf "out_%05d.ppm" $i`
	/usr/bin/time -f "%e s" "./$method" $width $height $centerx $centery $mag > "$fname"
	convert "$fname" -format png "$pngname"
	optipng -o4 -i0 "$pngname" > /dev/null 2>&1
	advpng -z4 "$pngname" > /dev/null 2>&1
	rm "$fname"
	i=$((i+1))
done
