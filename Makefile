all:
	gcc -Ofast -Wall -lquadmath mandel_quad.c -o mandel_quad
	gcc -Ofast -Wall -lmpfr mandel_mpfr.c -o mandel_mpfr
	gcc -Ofast -Wall -lm mandel.c -o mandel
