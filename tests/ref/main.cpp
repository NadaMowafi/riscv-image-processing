#include <iostream>
#include <vector>
#include "Gaussian.hpp"

int main() {
    // Example: a simple 5x5 grayscale image (values between 0 and 255)
    std::vector<std::vector<int>> image = {
        { 10,  50,  80,  50, 10 },
        { 50, 150, 200, 150, 50 },
        { 80, 200, 255, 200, 80 },
        { 50, 150, 200, 150, 50 },
        { 10,  50,  80,  50, 10 }
    };

    int kernelSize = 5; // Must be odd.
    double sigma = 1.0; // Standard deviation.

    // Generate the Gaussian kernel.
    std::vector<std::vector<double>> kernel = generateGaussianKernel(kernelSize, sigma);
    std::cout << "Kernel: " << std::endl;
    for (const auto& row : kernel) {
        for (const auto& pixel : row) {
            std::cout << pixel << "\t";
        }
        std::cout << std::endl;
    }
    // Apply the Gaussian filter to the image.
    std::vector<std::vector<int>> blurredImage = applyGaussianFilterSeparable(image, kernelSize, sigma );

    // Display the blurred image.
    std::cout << "Blurred Image:" << std::endl;
    for (const auto& row : blurredImage) {
        for (const auto& pixel : row) {
            std::cout << pixel << "\t";
        }
        std::cout << std::endl;
    }

    return 0;
}
