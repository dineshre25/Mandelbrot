/**
 * @file mandelbrot.h
 * @brief Header file for the mandelbrot application.
 * @author K. Desnos
 * @date 2026.09.15
 */

#ifndef MANDELBROT_H
#define MANDELBROT_H
#include <complex.h>

/**
 * @brief Computes the number of iterations for a given complex number c in the Mandelbrot set.
 *
 * A complex number c is in the Mandelbrot set if the sequence defined by z_{n+1} = z_n^2 + c
 * does not diverge to infinity. This function computes how many iterations it takes for the
 * sequence to diverge, up to a maximum number of iterations. As soon as the magnitude of z
 * exceeds 2, the sequence is considered to diverge.
 *
 * @param c The complex number to test.
 * @param max_iter The maximum number of iterations to perform.
 * @return The number of iterations before the sequence diverges, or max_iter if it doesn't diverge.
 */
__attribute__((no_instrument_function))
unsigned int mandelbrot(complex double c, unsigned int max_iter);

/**
 * @brief Fills an array with the Mandelbrot set values for a given region of the complex plane.
 *
 * This function computes the Mandelbrot set values for each point in a rectangular region
 * of the complex plane and stores them in an array. The region is defined by its boundaries
 * in the real and imaginary axes. The array is dynamically allocated and should be freed by
 * the caller when no longer needed.
 *
 * @param width The width of the array to fill.
 * @param height The height of the array to fill.
 * @param x_min The minimum value of the real axis.
 * @param x_max The maximum value of the real axis.
 * @param y_min The minimum value of the imaginary axis.
 * @param y_max The maximum value of the imaginary axis.
 * @param max_iter The maximum number of iterations to perform for each point.
 * @return A pointer to the array containing the Mandelbrot set values.
 */
unsigned int *fillMandelbrot(unsigned int width, unsigned int height, double x_min, double x_max, double y_min, double y_max, unsigned int max_iter);

/**
 * @brief YUV color values for complex values in the Mandelbrot set.
 */
extern const unsigned char *YUV_IN_COLOR;

/**
 * @brief Minimum YUV color values for complex values not in the Mandelbrot set.
 */
extern const unsigned char *YUV_MIN_COLOR;
/**
 * @brief Maximum YUV color values for complex values not in the Mandelbrot set, with the maximum number of iterations.
 */
extern const unsigned char *YUV_MAX_COLOR;

/**
 * @brief Converts a Mandelbrot set to a YUV format.
 *
 * This function takes a Mandelbrot set and converts it to a YUV format, which is commonly used for video encoding.
 * The conversion is based on the iteration counts of the Mandelbrot set. Colors are assigned based on whether the point is in the Mandelbrot set or not.
 * Interpolation is performed between the minimum and maximum YUV colors based on the iteration count for each point in the Mandelbrot set.
 *
 * @param mandelbrot_set A pointer to the array containing the Mandelbrot set values.
 * @param width The width of the array.
 * @param height The height of the array.
 * @param max_iter The maximum number of iterations used in the Mandelbrot set computation.
 * @return A pointer to the array containing the YUV values.
 */
unsigned char *mandelbrotToYUV(unsigned int *mandelbrot_set, unsigned int width, unsigned int height, unsigned int max_iter);

/**
 * @brief Structure representing a point in the trajectory of the Mandelbrot set exploration.
 */
typedef struct
{
  double complex center; /// The center of the region in the complex plane.
  double length;         /// The length of the region in the complex plane.
  unsigned int steps;    /// The number of steps to take to reach the next point in the trajectory.
} TrajectoryPoint;

/**
 * @brief Length of the trajectory array.
 */
extern unsigned int trajectoryLength;

/**
 * @brief Array of trajectory points for exploring the Mandelbrot set.
 */
extern TrajectoryPoint trajectory[];

#endif // MANDELBROT_H