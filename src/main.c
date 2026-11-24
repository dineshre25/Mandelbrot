/**
 * @file main.c
 * @brief Main entry point for the mandelbrot application.
 * @author K. Desnos
 * @date 2026.09.15
 */

#include <stdio.h>
#include <stdlib.h>

#include "md5.h"
#include "clock.h"
#include "yuvDisplay.h"
#include "mandelbrot.h"

#define MAX_ITER 75

int stopThreads = 0;

int main(int argc, char *argv[])
{
    // Initialize display
    yuvDisplayInit(0, DISPLAY_W, DISPLAY_H);

    // Cumulative time and steps
    double cumulativeTime = 0;
    unsigned int cumulativeSteps = 0;

    unsigned int originIndex = 0;
    unsigned int destinationIndex = 1;
    while (!stopThreads)
    {
        // Get current origin and destination
        TrajectoryPoint *origin = &trajectory[originIndex];
        TrajectoryPoint *destination = &trajectory[destinationIndex];

        // Reset timing for this waypoint segment.
        startTiming(0);

        // Print the mandelbrot for the given waypoints
        unsigned int step;
        for (step = 0; step < destination->steps && !stopThreads; step++)
        {

            // Progression to next waypoint
            double ratio = (double)step / (double)destination->steps;

            // Compute current mandelbrot settings
            complex double center = (destination->center - origin->center) * ratio + origin->center;
            double delta = (destination->length - origin->length) * ratio + origin->length;
            double xMin = creal(center) - delta / 2;
            double xMax = creal(center) + delta / 2;
            double yMin = cimag(center) - delta / 2;
            double yMax = cimag(center) + delta / 2;

            // Compute mandelbrot
            unsigned int *mandelbrot_set = fillMandelbrot(DISPLAY_H, DISPLAY_W, xMin, xMax, yMin, yMax, MAX_ITER);

            // Compute MD5
            MD5_Update(DISPLAY_H * DISPLAY_W * sizeof(unsigned int), mandelbrot_set);

            // Display mandelbrot
            unsigned char *yuv_image = mandelbrotToYUV(mandelbrot_set, DISPLAY_H, DISPLAY_W, MAX_ITER);
            free(mandelbrot_set);

            yuvDisplay(0, yuv_image, yuv_image + DISPLAY_H * DISPLAY_W, yuv_image + (unsigned int)(1.5 * DISPLAY_H * DISPLAY_W));
            free(yuv_image);
        }

        // Display clock
        unsigned int time = stopTiming(0);
        double elapsedSeconds = time / 1000000.0;
        cumulativeTime += elapsedSeconds;
        cumulativeSteps += step;
        double fps = step / elapsedSeconds;
        double cumulativeFps = cumulativeSteps / cumulativeTime;
        printf("FPS: %.2f (global: %.2f)\n", fps, cumulativeFps);

        // Increment trajectory
        originIndex = (originIndex + 1) % trajectoryLength;
        destinationIndex = (destinationIndex + 1) % trajectoryLength;
    }

    // Finalize display
    yuvFinalize(0);

    return 0;
}