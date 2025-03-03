#include "BoxFilter.h"
#include "FFT.h"
#include "Complex.h"
#include <vector>

std::vector<std::vector<double>> BoxFilter::applyBoxFilterFFT(
    const std::vector<std::vector<double>>& image, int kernelSize) {
    int originalRows = image.size();
    int originalCols = image[0].size();
    std::vector<std::vector<double>> paddedImage = FFT::zeroPad(image);
    int rows = paddedImage.size();
    int cols = paddedImage[0].size();
    std::vector<std::vector<double>> kernel(rows, std::vector<double>(cols, 0.0));
    double normFactor = 1.0 / (kernelSize * kernelSize);
    int halfKernel = kernelSize / 2;
    for (int i = 0; i < kernelSize; i++) {
        for (int j = 0; j < kernelSize; j++) {
            int y = (i - halfKernel + rows) % rows;
            int x = (j - halfKernel + cols) % cols;
            kernel[y][x] = normFactor;
        }
    }
    std::vector<std::vector<Complex>> imageComplex(rows, std::vector<Complex>(cols));
    std::vector<std::vector<Complex>> kernelComplex(rows, std::vector<Complex>(cols));
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            imageComplex[i][j] = Complex(paddedImage[i][j], 0.0);
            kernelComplex[i][j] = Complex(kernel[i][j], 0.0);
        }
    }
    FFT::fft2D(imageComplex, false);
    FFT::fft2D(kernelComplex, false);
    std::vector<std::vector<Complex>> resultComplex(rows, std::vector<Complex>(cols));
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            resultComplex[i][j] = imageComplex[i][j] * kernelComplex[i][j];
        }
    }
    FFT::fft2D(resultComplex, true);
    std::vector<std::vector<double>> paddedResult(rows, std::vector<double>(cols));
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            paddedResult[i][j] = resultComplex[i][j].real;
        }
    }
    return FFT::extractOriginalSize(paddedResult, originalRows, originalCols);
}
