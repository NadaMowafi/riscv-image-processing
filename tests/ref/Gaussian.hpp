#ifndef GAUSSIANFILTER_H
#define GAUSSIANFILTER_H

#include <vector>

// Generates a normalized 2D Gaussian kernel.
// The kernel is represented as a 2D vector of doubles.
// The kernel size must be an odd number.
std::vector<std::vector<double>> generateGaussianKernel(int kernelSize,
    double sigma);

// Applies a Gaussian filter (convolution) to the input image.
// The image is represented as a 2D vector of ints (grayscale values).
std::vector<std::vector<int>> applyGaussianFilter(
    const std::vector<std::vector<int>>& image,
    const std::vector<std::vector<double>>& kernel);
//Padding the image for convolution
std::vector<std::vector<int>> zeroPad(const std::vector<std::vector<int>>& image , int padSize);
std::vector<double> generateGaussianKernel1D(int kernelSize, double sigma);
std::vector<std::vector<int>> applyGaussianFilterSeparable(
    const std::vector<std::vector<int>>& image, int kernelSize, double sigma);
#endif // GAUSSIANFILTER_H