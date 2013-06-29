all: mandel_quad mandel_mpfr mandel mandel_dd mandel_dd_aa

mandel_quad: mandel_quad.c
	gcc -O3 -Wall -lquadmath mandel_quad.c -o mandel_quad

mandel_mpfr: mandel_mpfr.c
	gcc -O3 -Wall -lmpfr -lm mandel_mpfr.c -o mandel_mpfr

mandel: mandel.c
	gcc -O3 -Wall -lm mandel.c -o mandel

mandel_dd: mandel_dd.c
	gcc -O3 -Wall -lm mandel_dd.c -o mandel_dd

mandel_dd_aa: mandel_dd_aa.c
	gcc -O3 -Wall -lm mandel_dd_aa.c -o mandel_dd_aa
