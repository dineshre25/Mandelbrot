/**
 * @file mandelbrot.c
 * @brief Implementation of the mandelbrot application.
 * @author K. Desnos
 * @date 2026.09.15
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "mandelbrot.h"

const unsigned char *YUV_IN_COLOR = (const unsigned char[]){0, 128, 128};   // YUV representation of black
const unsigned char *YUV_MIN_COLOR = (const unsigned char[]){22, 228, 111}; // YUV representation of black
const unsigned char *YUV_MAX_COLOR = (const unsigned char[]){72, 84, 255};  // YUV representation of Red

/**
 * @brief Length of the trajectory array.
 */
unsigned int trajectoryLength = 5;

/**
 * @brief Array of trajectory points for exploring the Mandelbrot set.
 */
TrajectoryPoint trajectory[] = {
    {0.0 + 0.0 * I, 4.0, 100},
    {-0.5 + 0.0 * I, 2.0, 50},
    {-0.75 + 0.1 * I, 1.0, 50},
    {-1.00 + 0.25 * I, 0.5, 50},
    {-1.00 + 0.3 * I, 0.25, 100}};

unsigned int mandelbrot(complex double c, unsigned int max_iter)
{
    /*complex double z = 0.0 + 0.0 * I;
    unsigned int iter = 0;

    while (cabs(z) <= 2.0 && iter < max_iter)
    {
        z = z * z + c;
        iter++;
    }

    return iter;*/

    // réécriture de la fonction sans les nombres complexes
    double zR = 0.0;
    double zI = 0.0;
    double cR = creal(c);
    double cI = cimag(c);
    unsigned int iter = 0;

    while (sqrt(zR * zR + zI * zI) <= 2.0 && iter < max_iter)
    {
        double temp = zR * zR - zI * zI + cR;
        zI = 2.0 * zR * zI + cI;
        zR = temp;
        iter++;
    }

    return iter;
}

unsigned int *fillMandelbrot(unsigned int width, unsigned int height, double x_min, double x_max, double y_min, double y_max, unsigned int max_iter)
{

    // Allocate memory for the Mandelbrot set array
    unsigned int *mandelbrot_set = (unsigned int *)malloc(width * height * sizeof(unsigned int));
    if (mandelbrot_set == NULL)
    {
        fprintf(stderr, "Memory allocation failed for mandelbrot_set\n");
        return NULL;
    }

    // Check that the precision of the complex number is sufficient for the given range
    if (x_min + (x_max - x_min) * 1.0 / (width - 1) == x_min || y_min + (y_max - y_min) * 1.0 / (height - 1) == y_min)
    {
        fprintf(stderr, "Range is too small for sufficient precision\n");
        free(mandelbrot_set);
        return NULL;
    }

    #pragma omp parallel num_threads(6) shared(height, width, x_max, x_min, y_max, y_min) // machine de TP avec 6 coeurs
    {
        double x_temp = (x_max - x_min) / (width - 1);
        // Fill the Mandelbrot set array
        #pragma omp for schedule(dynamic)
        for (unsigned int j = 0; j < height; j++)
        {
            double y = y_min + (y_max - y_min) * j / (height - 1); // Sortie de la boucle for car dépend que de j
            for (unsigned int i = 0; i < width; i++)
            {
                double x = x_min + x_temp * i;
                complex double c = x + y * I;
                mandelbrot_set[j * width + i] = mandelbrot(c, max_iter);
            }
        }
    }

    return mandelbrot_set;
}

unsigned char *mandelbrotToYUV(unsigned int *mandelbrot_set, unsigned int width, unsigned int height, unsigned int max_iter)
{

    // Allocate memory for the YUV image
    unsigned char *yuv_image = (unsigned char *)malloc(width * height * 3 * sizeof(unsigned char));
    if (yuv_image == NULL)
    {
        fprintf(stderr, "Memory allocation failed for yuv_image\n");
        return NULL;
    }

    // Create separate pointers for Y, U, and V components
    unsigned char *y = yuv_image;
    unsigned char *u = yuv_image + width * height;
    unsigned char *v = yuv_image + (unsigned int)(1.5 * width * height);

    // Fill the YUV image based on the Mandelbrot set values
    // Scan the pixels in 2x2 blocks to compute U and V values

    #pragma omp parallel num_threads(6) shared(height, width, mandelbrot_set, max_iter) // machine de TP avec 6 coeurs
    {
        #pragma omp for schedule(static)
        for (unsigned int j = 0; j < height; j += 2)
        {
            for (unsigned int i = 0; i < width; i += 2)
            {
                unsigned int u_val = 0;
                unsigned int v_val = 0;
                // Process each pixel in the 2x2 block
                // Accumulate U and V values for the 2x2 block
                for (unsigned int jj = 0; jj < 2; jj++)
                {
                    unsigned int idx_temp = (j + jj) * width + i;
                    for (unsigned int ii = 0; ii < 2; ii++)
                    {
                        unsigned int idx = idx_temp + ii;
                        unsigned int iter = mandelbrot_set[idx];
                        if (iter >= max_iter)
                        {
                            // Point is in the Mandelbrot set, use YUV_IN_COLOR
                            y[idx] = YUV_IN_COLOR[0];
                            u_val += YUV_IN_COLOR[1];
                            v_val += YUV_IN_COLOR[2];
                        }
                        else
                        {
                            // Point is not in the Mandelbrot set, interpolate between YUV_MIN_COLOR and YUV_MAX_COLOR
                            double t = sqrt((double)iter / max_iter);
                            y[idx] = (unsigned char)((1 - t) * YUV_MIN_COLOR[0] + t * YUV_MAX_COLOR[0]);
                            u_val += (unsigned char)((1 - t) * YUV_MIN_COLOR[1] + t * YUV_MAX_COLOR[1]);
                            v_val += (unsigned char)((1 - t) * YUV_MIN_COLOR[2] + t * YUV_MAX_COLOR[2]);
                        }
                    }
                }

                // Average the U and V values for the 2x2 block
                u[(j / 2) * (width / 2) + (i / 2)] = u_val / 4;
                v[(j / 2) * (width / 2) + (i / 2)] = v_val / 4;
            }
        }
    }

    return yuv_image;
}