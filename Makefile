all:
	gcc -O3 -Wall -lquadmath mandel_quad.c -o mandel_quad
	gcc -O3 -Wall -lmpfr -lm mandel_mpfr.c -o mandel_mpfr
	gcc -O3 -Wall -lm mandel.c -o mandel
	gcc -O3 -Wall -lm mandel_dd.c -o mandel_dd
