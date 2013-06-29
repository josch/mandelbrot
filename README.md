Introduction
============

Calculating Mandelbrot using different ways. It tries to imitate the Mandelbrot
calculation by Zom-B but be faster in doing so.

This project started by trying to reproduce this zoom sequence but in higher
resolution:

https://en.wikipedia.org/wiki/File:Mandelbrot_sequence_new.gif

The generator used for this version is available here:

https://github.com/josch/mandelbrot_zomb

Overview
========

mandel:
 - straight forward "long double" based mandelbrot
 - this is obviously the fastest implementation but has limited precision

mandel_mpfr:
 - mandelbrot using the arbitrary precision mpfr library

mandel_quad:
 - mandelbrot using gcc libquad

mandel_dd:
 - mandelbrot using custom double double math
 - this is the fastest implementation given double double precision

mandel_dd_aa:
 - with antialiasing
 - it is more than three times faster than the Zom-B version (see below)


How to compile
==============

Compile by running `make`.

How to execute
==============

Each of the tools takes as arguments the width, height, coordinates and
magnification. Output is in PPM format on standard output.

	$ ./mandel_dd_aa 1920 1080 0.0 0.0 1.0 > out.ppm

You can compress the ppm by using:

	$ convert out.ppm -format png out.png

And even compress it further by doing:

	$ optipng -o4 -i0 out.png
	$ advpng -z4 out.png

= How to generate zoom animation =

The shell script (with a small python helper) `run.sh` will generate and
execute a list of commands to create a zooming sequence of images.

You can create a video of the resulting sequence using:

	$ avconv -i out_%05d.png out.mp4

Or a gif animation by using:

	$ convert -delay 20 -loop 0 out_*.png out.gif

Benchmark
=========

Benchmark against mandelbrot computation by Zom-B:

at zoom coordinates:
 - x = -0.743643887037158704752191506114774
 - y = 0.131825904205311970493132056385139

1 to 1e32 in 100 steps at 320x240 and no AA took
 - mandel_dd   1:28 h
 - mandel_zomb 2:45 h

1.0e17 at 1920x1080 with 5xAA took
 - mandel_dd_aa 2:38 h
 - mandel_zomb  8:40 h
