#include "colors.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <gmp.h>
#include "doubledouble.h"

int main(int argc, char **argv) {
    if (argc != 6) {
        fprintf(stderr, "usage: %s width height centerx centery magnification", argv[0]);
        return 1;
    }
    DoubleDouble temp1;
    double eps = 1e-17;
    double Q1LOG2 = 1.44269504088896340735992468100189213742664595415299;
    double LOG2 = 0.69314718055994530941723212145817656807550013436026;
    unsigned int width = atoi(argv[1]);
    unsigned int height = atoi(argv[2]);
    char* image = malloc(width*height*3);
    unsigned int x, y;
    DoubleDouble centerx, centery;
    centerx = dd_new(-0.7436438870371587, -3.628952515063387E-17);
    centery = dd_new(0.13182590420531198, -1.2892807754956678E-17);
    double bailout = 4; // the distance must not be greater than 2 (4 = 2*2)
    double logLogBailout = log(log(bailout));
    DoubleDouble magn = dd_new(strtod(argv[5], NULL), 0);
    // maxiter = width * sqrt(magn);
    temp1 = dd_sqrt(magn);
    unsigned long maxiter = width * dd_get_ui(temp1);
    DoubleDouble x2, y2, x0d, y1d;
    // x0d = 4 / magn / width;
    x0d = dd_ui_div(4, magn);
    x0d = dd_div_ui(x0d, width);
    // x2 = -2 / magn + centerx;
    x2 = dd_si_div(-2, magn);
    x2 = dd_add(x2, centerx);
    // y1d = -4 / magn / width;
    y1d = dd_si_div(-4, magn);
    y1d = dd_div_ui(y1d, width);
    // y2 = 2 / magn * height / width + centery;
    y2 = dd_ui_div(2, magn);
    temp1 = dd_new(height, 0);
    temp1 = dd_div_ui(temp1, width);
    y2 = dd_mul(y2, temp1);
    y2 = dd_add(y2, centery);
    DoubleDouble px, py, zx, zy, xx, yy;
    unsigned int idx;
    unsigned int imgidx = 0;
    double hx, hy, d;
    int foundperiods = 0;
    /*fprintf(stderr, "centerx: %E\n", centerx.hi);
    fprintf(stderr, "centery: %E\n", centery.hi);
    fprintf(stderr, "magn:    %E\n", magn.hi);
    fprintf(stderr, "x2:      %E\n", x2.hi);
    fprintf(stderr, "y2:      %E\n", y2.hi);
    fprintf(stderr, "x0d:     %E\n", x0d.hi);
    fprintf(stderr, "y1d:     %E\n", y1d.hi);*/
    for (y = 0; y < height; y++) {
        for (x = 0; x < width; x++) {
            fprintf(stderr, "\r%f %%", (float)imgidx/(width*height*3)*100);
            //px = x*x0d + x2;
            px = dd_mul_ui(x0d, x);
            px = dd_add(px, x2);
            //py = y*y1d + y2;
            py = dd_mul_ui(y1d, y);
            py = dd_add(py, y2);
            // no Main bulb or Cardoid check to be faster
            zx = dd_new(px.hi, px.lo);
            zy = dd_new(py.hi, py.lo);
            unsigned long i;
            bool inside = true;
            int check = 3;
            int whenupdate = 10;
            hx = 0;
            hy = 0;
            //for (i = 1; i <= maxiter; i++) {
            for (i = 1; i <= 50000; i++) {
                //xx = zx * zx;
                xx = dd_sqr(zx);
                //yy = zy * zy;
                yy = dd_sqr(zy);
                //if (xx + yy > bailout) {
                if (xx.hi + yy.hi > bailout) {
                    inside = false;
                    break;
                }
                // iterate
                //zy = 2 * zx * zy + py;
                //zx = dd_mul_ui(zx, 2);
                //zy = dd_mul(zx, zy);
                zy = dd_add(dd_mul2(zx, zy), py);
                //zx = xx - yy + px;
                zx = dd_add(dd_sub(xx, yy), px);

                // period checking
                d = zx.hi - hx;
                if (d > 0.0 ? d < eps : d > -eps) {
                    d = zy.hi - hy;
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
                    hx = zx.hi;
                    hy = zy.hi;
                }
            }

            if (inside) {
                image[imgidx++] = 0;
                image[imgidx++] = 0;
                image[imgidx++] = 0;
            } else {
                double r = sqrt(zx.hi*zx.hi + zy.hi*zy.hi);
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
