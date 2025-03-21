#ifndef GAUSSIANFILTER_HPP
#define GAUSSIANFILTER_HPP

#include <vector>
#include <cstdint>
using namespace std;

// Generates a normalized 2D Gaussian kernel.
// The kernel is represented as a 2D vector of doubles.
// The kernel size must be an odd number.
vector<vector<double>> generateGaussianKernel(int kernelSize, double sigma);

// Applies a Gaussian filter (convolution) to the input image.
// The image is represented as a 2D vector of uint8_t (grayscale values).
template <typename T = uint8_t>
vector<vector<T>> applyGaussianFilter(
    const vector<vector<T>> &image,
    const vector<vector<double>> &kernel);

// Padding the image for convolution
template <typename T = uint8_t>
vector<vector<T>> zeroPad(const vector<vector<T>> &image, int padSize);

// Generates a 1D Gaussian kernel.
vector<double> generateGaussianKernel1D(int kernelSize, double sigma);

// Applies a separable Gaussian filter to the input image.
template <typename T = uint8_t>
vector<vector<T>> applyGaussianFilterSeparable(
    const vector<vector<T>> &image, int kernelSize, double sigma);

#endif // GAUSSIANFILTER_H
