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
    mpfr_t temp1;
    mpfr_init2(temp1, 300);
    double eps = 1e-17;
    double Q1LOG2 = 1.44269504088896340735992468100189213742664595415299;
    double LOG2 = 0.69314718055994530941723212145817656807550013436026;
    unsigned int width = atoi(argv[1]);
    unsigned int height = atoi(argv[2]);
    char* image = malloc(width*height*3);
    unsigned int x, y;
    mpfr_t centerx, centery;
    mpfr_init2(centerx, 300);
    mpfr_init2(centery, 300);
    mpfr_set_str(centerx, argv[3], 10, MPFR_RNDN);
    mpfr_set_str(centery, argv[4], 10, MPFR_RNDN);
    double bailout = 128; // with a smaller value there are lines on magn=1
    double logLogBailout = log(log(bailout));
    mpfr_t magn;
    mpfr_init2(magn, 300);
    mpfr_set_str(magn, argv[5], 10, MPFR_RNDN);
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
    mpfr_t px, py, zx, zy, xx, yy;
    mpfr_init2(px, 300);
    mpfr_init2(py, 300);
    mpfr_init2(zx, 300);
    mpfr_init2(zy, 300);
    mpfr_init2(xx, 300);
    mpfr_init2(yy, 300);
    unsigned int idx;
    unsigned int imgidx = 0;
    double hx, hy, d;
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
            hx = 0;
            hy = 0;
            double zxd, zyd;
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
                zxd = mpfr_get_d(zx, MPFR_RNDN);
                d = zxd - hx;
                if (d > 0.0 ? d < eps : d > -eps) {
                    zyd = mpfr_get_d(zy, MPFR_RNDN);
                    d = zyd - hy;
                    if (d > 0.0 ? d < eps : d > -eps) {
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
                    zxd = mpfr_get_d(zx, MPFR_RNDN);
                    zyd = mpfr_get_d(zy, MPFR_RNDN);
                    hx = zxd;
                    hy = zyd;
                }
            }

            if (inside) {
                image[imgidx++] = 0;
                image[imgidx++] = 0;
                image[imgidx++] = 0;
            } else {
                zxd = mpfr_get_d(zx, MPFR_RNDN);
                zyd = mpfr_get_d(zy, MPFR_RNDN);
                double r = sqrt(zxd*zxd + zyd*zyd);
                double c = i - 1.28 + (logLogBailout - log(log(r))) * Q1LOG2;
                idx = fmod((log(c/64+1)/LOG2+0.45), 1)*GRADIENTLENGTH + 0.5;
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
