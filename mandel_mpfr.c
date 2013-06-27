#include "colors.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <gmp.h>
#include <mpfr.h>

int main(int argc, char **argv) {
    if (argc != 6) {
        fprintf(stderr, "usage: %s width height centerx centery magnification", argv[0]);
        return 1;
    }
    mpfr_t temp1, temp2;
    mpfr_init2(temp1, 300);
    mpfr_init2(temp2, 300);
    long double eps = 1e-17;
    mpfr_t Q1LOG2, LOG2;
    mpfr_init_set_str(Q1LOG2, "1.44269504088896340735992468100189213742664595415299", 10, MPFR_RNDN);
    mpfr_init_set_str(LOG2, "0.69314718055994530941723212145817656807550013436026", 10, MPFR_RNDN);
    unsigned int width = atoi(argv[1]);
    unsigned int height = atoi(argv[2]);
    char* image = malloc(width*height*3);
    unsigned int x, y;
    mpfr_t centerx, centery;
    mpfr_init2(centerx, 300);
    mpfr_init2(centery, 300);
    mpfr_set_str(centerx, argv[3], 10, MPFR_RNDN);
    mpfr_set_str(centery, argv[4], 10, MPFR_RNDN);
    /*mpfr_set_d(centerx, -0.7436438870371587, MPFR_RNDN);
    mpfr_add_d(centerx, centerx, -3.628952515063387E-17, MPFR_RNDN);
    mpfr_set_d(centery, 0.13182590420531198, MPFR_RNDN);
    mpfr_add_d(centery, centery, -1.2892807754956678E-17, MPFR_RNDN);
    mpfr_out_str(stderr, 10, 0, centerx, MPFR_RNDN);
    fputs("\n", stderr);
    mpfr_out_str(stderr, 10, 0, centery, MPFR_RNDN);
    fputs("\n", stderr);*/
    double bailout = 4; // the distance must not be greater than 2 (4 = 2*2)
    mpfr_t logLogBailout;
    mpfr_init2(logLogBailout, 300);
    // logLogBailout = log(log(bailout));
    mpfr_set_d(logLogBailout, bailout, MPFR_RNDN);
    mpfr_log(logLogBailout, logLogBailout, MPFR_RNDN);
    mpfr_log(logLogBailout, logLogBailout, MPFR_RNDN);
    mpfr_t magn;
    mpfr_init_set_str(magn, argv[5], 10, MPFR_RNDN);
    // maxiter = width * sqrt(magn);
    mpfr_sqrt(temp1, magn, MPFR_RNDN);
    unsigned long maxiter = width * mpfr_get_ui(temp1, MPFR_RNDN);
    mpfr_t x2, y2, x0d, y1d;
    mpfr_init2(x2, 300);
    mpfr_init2(y2, 300);
    mpfr_init2(x0d, 300);
    mpfr_init2(y1d, 300);
    // x0d = 4 / magn / width;
    mpfr_ui_div(x0d, 4, magn, MPFR_RNDN);
    mpfr_div_ui(x0d, x0d, width, MPFR_RNDN);
    // x2 = -2 / magn + centerx;
    mpfr_si_div(x2, -2, magn, MPFR_RNDN);
    mpfr_add(x2, x2, centerx, MPFR_RNDN);
    // y1d = -4 / magn / width;
    mpfr_si_div(y1d, -4, magn, MPFR_RNDN);
    mpfr_div_ui(y1d, y1d, width, MPFR_RNDN);
    // y2 = 2 / magn * height / width + centery;
    mpfr_ui_div(y2, 2, magn, MPFR_RNDN);
    mpfr_set_ui(temp1, height, MPFR_RNDN);
    mpfr_div_ui(temp1, temp1, width, MPFR_RNDN);
    mpfr_mul(y2, y2, temp1, MPFR_RNDN);
    mpfr_add(y2, y2, centery, MPFR_RNDN);
    mpfr_t px, py, zx, zy, xx, yy, r, c;
    mpfr_init2(px, 300);
    mpfr_init2(py, 300);
    mpfr_init2(zx, 300);
    mpfr_init2(zy, 300);
    mpfr_init2(xx, 300);
    mpfr_init2(yy, 300);
    mpfr_init2(r, 300);
    mpfr_init2(c, 300);
    int idx;
    int imgidx = 0;
    mpfr_t hx, hy, d;
    mpfr_init2(hx, 300);
    mpfr_init2(hy, 300);
    mpfr_init2(d, 300);
    int foundperiods = 0;
    for (y = 0; y < height; y++) {
        for (x = 0; x < width; x++) {
            fprintf(stderr, "\r%f %%", (float)imgidx/(width*height*3)*100);
            //px = x*x0d + x2;
            mpfr_mul_ui(px, x0d, x, MPFR_RNDN);
            mpfr_add(px, px, x2, MPFR_RNDN);
            //py = y*y1d + y2;
            mpfr_mul_ui(py, y1d, y, MPFR_RNDN);
            mpfr_add(py, py, y2, MPFR_RNDN);
            // no Main bulb or Cardoid check to be faster
            //zx = px;
            mpfr_set(zx, px, MPFR_RNDN);
            //zy = py;
            mpfr_set(zy, py, MPFR_RNDN);
            unsigned long i;
            bool inside = true;
            int check = 3;
            int whenupdate = 10;
            mpfr_set_d(hx, 0, MPFR_RNDN);
            mpfr_set_d(hy, 0, MPFR_RNDN);
            for (i = 1; i <= maxiter; i++) {
            //for (i = 1; i <= 50000; i++) {
                //xx = zx * zx;
                mpfr_mul(xx, zx, zx, MPFR_RNDN);
                //yy = zy * zy;
                mpfr_mul(yy, zy, zy, MPFR_RNDN);
                //if (xx + yy > bailout) {
                mpfr_add(temp1, xx, yy, MPFR_RNDN);
                if (mpfr_cmp_ld(temp1, bailout) > 0) {
                    inside = false;
                    break;
                }
                // iterate
                //zy = 2 * zx * zy + py;
                mpfr_mul_ui(zx, zx, 2, MPFR_RNDN);
                mpfr_mul(zy, zx, zy, MPFR_RNDN);
                mpfr_add(zy, zy, py, MPFR_RNDN);
                //zx = xx - yy + px;
                mpfr_sub(zx, xx, yy, MPFR_RNDN);
                mpfr_add(zx, zx, px, MPFR_RNDN);

                // period checking
                // d = zx - hx;
                mpfr_sub(d, zx, hx, MPFR_RNDN);
                mpfr_abs(d, d, MPFR_RNDN);
                if (mpfr_cmp_d(d, eps) < 0) {
                    // d = zy - hy;
                    mpfr_sub(d, zy, hy, MPFR_RNDN);
                    mpfr_abs(d, d, MPFR_RNDN);
                    if (mpfr_cmp_d(d, eps) < 0) {
                        // Period found.
                        foundperiods++;
                        break;
                    }
                }
                if ((i & check) == 0) {
                    if (--whenupdate == 0) {
                        whenupdate = 10;
                        check <<= 1;
                        check++;
                    }
                    // period = 0;
                    mpfr_set(hx, zx, MPFR_RNDN);
                    mpfr_set(hy, zy, MPFR_RNDN);
                }
            }

            if (inside) {
                image[imgidx++] = 0;
                image[imgidx++] = 0;
                image[imgidx++] = 0;
            } else {
                //r = sqrtl(zx*zx + zy*zy);
                mpfr_mul(temp1, zx, zx, MPFR_RNDN);
                mpfr_mul(temp2, zy, zy, MPFR_RNDN);
                mpfr_add(temp1, temp1, temp2, MPFR_RNDN);
                mpfr_sqrt(r, temp1, MPFR_RNDN);
                //c = i - 1.28 + (logLogBailout - logl(logl(r))) * Q1LOG2;
                mpfr_log(r, r, MPFR_RNDN);
                mpfr_log(r, r, MPFR_RNDN);
                mpfr_sub(c, logLogBailout, r, MPFR_RNDN);
                mpfr_mul(c, c, Q1LOG2, MPFR_RNDN);
                mpfr_add_d(c, c, i - 1.28, MPFR_RNDN);
                //idx = fmodl((logl(c/64+1)/LOG2+0.45), 1)*GRADIENTLENGTH + 0.5;
                mpfr_div_ui(c, c, 64, MPFR_RNDN);
                mpfr_add_ui(c, c, 1, MPFR_RNDN);
                mpfr_log(c, c, MPFR_RNDN);
                mpfr_div(c, c, LOG2, MPFR_RNDN);
                mpfr_add_d(c, c, 0.45, MPFR_RNDN);
                mpfr_frac(c, c, MPFR_RNDN);
                mpfr_mul_d(c, c, GRADIENTLENGTH, MPFR_RNDN);
                idx = mpfr_get_ui(c, MPFR_RNDN);
                image[imgidx++] = colors[idx][0];
                image[imgidx++] = colors[idx][1];
                image[imgidx++] = colors[idx][2];
            }
        }
    }
    // write out image
    printf("P6 %d %d 255\n", width, height);
    fwrite(image, 1, width*height*3, stdout);
    //fprintf(stderr, "\r%d found periods", foundperiods);
    fprintf(stderr, "\n");

    return 0;
}
