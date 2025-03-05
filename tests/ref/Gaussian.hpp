#ifndef GAUSSIANFILTER_H
#define GAUSSIANFILTER_H

#include <vector>
#include <cstdint>

// Generates a normalized 2D Gaussian kernel.
// The kernel is represented as a 2D vector of doubles.
// The kernel size must be an odd number.
std::vector<std::vector<double>> generateGaussianKernel(int kernelSize, double sigma);

// Applies a Gaussian filter (convolution) to the input image.
// The image is represented as a 2D vector of uint8_t (grayscale values).
std::vector<std::vector<uint8_t>> applyGaussianFilter(
    const std::vector<std::vector<uint8_t>>& image,
    const std::vector<std::vector<double>>& kernel);

// Padding the image for convolution
std::vector<std::vector<uint8_t>> zeroPad(const std::vector<std::vector<uint8_t>>& image, int padSize);

// Generates a 1D Gaussian kernel.
std::vector<double> generateGaussianKernel1D(int kernelSize, double sigma);

// Applies a separable Gaussian filter to the input image.
std::vector<std::vector<uint8_t>> applyGaussianFilterSeparable(
    const std::vector<std::vector<uint8_t>>& image, int kernelSize, double sigma);

#endif // GAUSSIANFILTER_H