#include "Gaussian.hpp"
#include <vector>
#include <cmath>
#include <cstdint>

//--------------------------------------------------
// 2D Gaussian Kernel (integrated version)
//--------------------------------------------------
vector<vector<double>> generateGaussianKernel(int kernelSize, double sigma) {
    vector<vector<double>> kernel(kernelSize, vector<double>(kernelSize, 0));
    double sum = 0.0;
    int half = kernelSize / 2;
    double twoSigmaSquare = 2 * sigma * sigma;
    double constant = 1.0 / (2 * M_PI * sigma * sigma);
    double step = 0.001; // integration increment

    // Loop over each pixel in the kernel
    for (int i = -half; i <= half; i++) {
        for (int j = -half; j <= half; j++) {
            double integratedValue = 0.0;
            // Integrate over the pixel area from (i - 0.5) to (i + 0.5) and (j - 0.5) to (j + 0.5)
            for (double x = i - 0.5; x < i + 0.5; x += step) {
                for (double y = j - 0.5; y < j + 0.5; y += step) {
                    integratedValue += constant * exp(-(x * x + y * y) / twoSigmaSquare) * step * step;
                }
            }
            kernel[i + half][j + half] = integratedValue;
            sum += integratedValue;
        }
    }
    // Normalize the kernel so that the sum of all elements is 1
    for (int i = 0; i < kernelSize; i++) {
        for (int j = 0; j < kernelSize; j++) {
            kernel[i][j] /= sum;
        }
    }
    return kernel;
}

//--------------------------------------------------
// 1D Gaussian Kernel (for separable convolution)
//--------------------------------------------------
vector<double> generateGaussianKernel1D(int kernelSize, double sigma) {
    int half = kernelSize / 2;
    vector<double> kernel(kernelSize, 0.0);
    double sum = 0.0;
    double twoSigmaSquare = 2 * sigma * sigma;
    double constant = 1.0 / (sqrt(2 * M_PI) * sigma);

    for (int i = -half; i <= half; i++) {
        double value = constant * exp(-(i * i) / twoSigmaSquare);
        kernel[i + half] = value;
        sum += value;
    }
    // Normalize the kernel
    for (int i = 0; i < kernelSize; i++) {
        kernel[i] /= sum;
    }
    return kernel;
}

//--------------------------------------------------
// Zero padding for uint8_t images with a given pad size
//--------------------------------------------------
vector<vector<uint8_t>> zeroPad(const vector<vector<uint8_t>>& image, int pad) {
    int rows = image.size();
    int cols = image[0].size();
    int paddedRows = rows + 2 * pad;
    int paddedCols = cols + 2 * pad;
    
    vector<vector<uint8_t>> padded(paddedRows, vector<uint8_t>(paddedCols, 0));
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            padded[i + pad][j + pad] = image[i][j];
        }
    }
    return padded;
}

//--------------------------------------------------
// 2D convolution version
//--------------------------------------------------
vector<vector<uint8_t>> applyGaussianFilter(
    const vector<vector<uint8_t>>& image,
    const vector<vector<double>>& kernel) {

    int height = image.size();
    int width = image[0].size();
    int kSize = kernel.size();
    int half = kSize / 2;

    // Pad the image with 'half' pixels on each side
    vector<vector<uint8_t>> paddedImage = zeroPad(image, half);
    
    // Create an output image with the same dimensions as the original image
    vector<vector<uint8_t>> output(height, vector<uint8_t>(width, 0));

    // Perform convolution on the padded image
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            double sum = 0.0;
            for (int m = 0; m < kSize; m++) {
                for (int n = 0; n < kSize; n++) {
                    sum += paddedImage[i + m][j + n] * kernel[m][n];
                }
            }
            output[i][j] = static_cast<uint8_t>(sum);
        }
    }
    return output;
}

//--------------------------------------------------
// Separable convolution version (optimized) using the fact that Gaussian kernel is separable
//--------------------------------------------------
vector<vector<uint8_t>> applyGaussianFilterSeparable(
    const vector<vector<uint8_t>>& image,
    int kernelSize,
    double sigma) {

    int height = image.size();
    int width = image[0].size();
    int half = kernelSize / 2;

    // Generate the 1D Gaussian kernel
    vector<double> kernel1D = generateGaussianKernel1D(kernelSize, sigma);

    // First pass: horizontal convolution.
    vector<vector<double>> intermediate(height, vector<double>(width, 0.0));
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            double sum = 0.0;
            for (int k = -half; k <= half; k++) {
                int col = j + k;
                // Zero padding: if the index is out-of-bounds, assume 0.
                if (col < 0 || col >= width) continue;
                sum += image[i][col] * kernel1D[k + half];
            }
            intermediate[i][j] = sum;
        }
    }

    // Second pass: vertical convolution.
    vector<vector<uint8_t>> output(height, vector<uint8_t>(width, 0));
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            double sum = 0.0;
            for (int k = -half; k <= half; k++) {
                int row = i + k;
                // Zero padding: if the index is out-of-bounds, assume 0.
                if (row < 0 || row >= height) continue;
                sum += intermediate[row][j] * kernel1D[k + half];
            }
            output[i][j] = static_cast<uint8_t>(sum);
        }
    }
    return output;
}