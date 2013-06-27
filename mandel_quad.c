#include "colors.h"
#include <quadmath.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

int main(int argc, char **argv) {
    if (argc != 6) {
        fprintf(stderr, "usage: %s width height centerx centery magnification", argv[0]);
        return 1;
    }
    __float128 eps = 1e-17;
    __float128 Q1LOG2, LOG2;
    Q1LOG2 = strtoflt128("1.44269504088896340735992468100189213742664595415299", NULL);
    LOG2 = strtoflt128("0.69314718055994530941723212145817656807550013436026", NULL);
    unsigned int width = atoi(argv[1]);
    unsigned int height = atoi(argv[2]);
    char* image = malloc(width*height*3);
    int x, y;
    __float128 centerx, centery;
    centerx = strtoflt128(argv[3], NULL);
    centery = strtoflt128(argv[4], NULL);
    __float128 bailout = 128;
    __float128 logLogBailout = logq(logq(bailout));
    int foundperiods = 0;
    __float128 magn = strtoflt128(argv[5], NULL);
    long maxiter = width * sqrtq(magn);
    __float128 x0d = 4 / magn / width;
    __float128 x2 = -2 / magn + centerx;
    __float128 y1d = -4 / magn / width;
    __float128 y2 = 2 / magn * height / width + centery;
    int imgidx = 0;
    for (y = 0; y < height; y++) {
        for (x = 0; x < width; x++) {
            fprintf(stderr, "\r%f %%", (float)imgidx/(width*height*3)*100);
            __float128 px = x*x0d + x2;
            __float128 py = y*y1d + y2;
            // no Main bulb or Cardoid check to be faster
            __float128 zx = px;
            __float128 zy = py;
            long i;
            // Initial maximum period to detect.
            int check = 3;
            // Maximum period doubles every iterations:
            int whenupdate = 10;
            // Period history registers.
            __float128 hx = 0;
            __float128 hy = 0;
            __float128 xx, yy;
            bool inside = true;
            for (i = 1; i <= maxiter; i++) {
                xx = zx * zx;
                yy = zy * zy;
                if (xx + yy > bailout) {
                    inside = false;
                    break;
                }
                // iterate
                zy = 2 * zx * zy + py;
                zx = xx - yy + px;

                // periodicity check
                __float128 d = zx - hx;
                if (d > 0.0 ? d < eps : d > -eps) {
                    d = zy - hy;
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
                    hx = zx;
                    hy = zy;
                }
            }

            if (inside) {
                image[imgidx++] = 0;
                image[imgidx++] = 0;
                image[imgidx++] = 0;
            } else {
                __float128 r = sqrtq(zx*zx + zy*zy);
                __float128 c = i - 1.28 + (logLogBailout - logq(logq(r))) * Q1LOG2;
                int idx = fmodq((logq(c/64+1)/LOG2+0.45), 1)*GRADIENTLENGTH + 0.5;
                image[imgidx++] = colors[idx][0];
                image[imgidx++] = colors[idx][1];
                image[imgidx++] = colors[idx][2];
            }
        }
    }
    // write out image
    printf("P6 %d %d 255\n", width, height);
    fwrite(image, 1, width*height*3, stdout);
    //fprintf(stderr, "%d periods found\n", foundperiods);

    return 0;
}
