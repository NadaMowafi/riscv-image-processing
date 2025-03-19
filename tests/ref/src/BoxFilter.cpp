#include "BoxFilter.hpp"
#include "FFT.hpp"
#include "Complex.hpp"
#include <vector>
#include <iostream>
#include <stdexcept>
#include <cstdint>
#include <cmath> 



vector<vector<uint8_t>> BoxFilter::applyBoxFilterFFT(
    const vector<vector<uint8_t>>& image, int kernelSize) {
    if (image.empty() || image[0].empty()) {
        throw invalid_argument("Image is empty");
    }

    int originalRows = image.size();
    int originalCols = image[0].size();
    if (kernelSize > originalRows || kernelSize > originalCols || kernelSize % 2 == 0) {
        throw invalid_argument("Invalid kernel size");
    }
    vector<vector<double>> doubleImage(originalRows, vector<double>(originalCols, 0.0));
    for (int i = 0; i < originalRows; i++) {
        for (int j = 0; j < originalCols; j++) {
            doubleImage[i][j] = static_cast<double>(image[i][j]);
        }
    }

    vector<vector<double>> paddedImage = FFT::zeroPad(doubleImage);
    int rows = paddedImage.size();
    int cols = paddedImage[0].size();
    vector<vector<double>> kernel(rows, vector<double>(cols, 0.0));
    double normFactor = 1.0 / (kernelSize * kernelSize);
    int halfKernel = kernelSize / 2;
    for (int i = 0; i < kernelSize; i++) {
        for (int j = 0; j < kernelSize; j++) {
            int y = (i - halfKernel + rows) % rows;
            int x = (j - halfKernel + cols) % cols;
            kernel[y][x] = normFactor;
        }
    }
    vector<vector<Complex>> imageComplex(rows, vector<Complex>(cols));
    vector<vector<Complex>> kernelComplex(rows, vector<Complex>(cols));
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            imageComplex[i][j] = Complex(paddedImage[i][j], 0.0);
            kernelComplex[i][j] = Complex(kernel[i][j], 0.0);
        }
    }
    FFT::fft2D(imageComplex, false);
    FFT::fft2D(kernelComplex, false);
    vector<vector<Complex>> resultComplex(rows, vector<Complex>(cols));
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            resultComplex[i][j] = imageComplex[i][j] * kernelComplex[i][j];
        }
    }
    FFT::fft2D(resultComplex, true);
    vector<vector<double>> paddedResult(rows, vector<double>(cols));
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            paddedResult[i][j] = resultComplex[i][j].real;
        }
    }

    // Debugging: Print intermediate results
    cout << "Padded Result:" << endl;
    for (const auto& row : paddedResult) {
        for (const auto& val : row) {
            cout << val << " ";
        }
        cout << endl;
    }

    return FFT::extractOriginalSize(paddedResult, originalRows, originalCols);
}

vector<vector<uint8_t>> BoxFilter::applyBoxFilterSlidingGrey(
    const vector<vector<uint8_t>>& inputImg, int kernelSize) {
        if (inputImg.empty() || inputImg[0].empty()) {
            throw invalid_argument("Image is empty");
        }
    
        vector<vector<uint8_t>> outputImg;
        int rows = inputImg.size();  // Number of rows in the input image
        int cols = inputImg[0].size();  // Number of columns in the input image
        int border = kernelSize / 2; // Border size for padding to handle edges
          // Check if kernel size is greater than image dimensions
          if (kernelSize > rows || kernelSize > cols || kernelSize % 2 == 0) {
            throw invalid_argument("Invalid kernel size");
        }
    
        // Initialize the output image with the same size as the input
        outputImg.resize(rows, vector<uint8_t>(cols, 0));
    
        // Create a padded version of the input image to handle borders
        vector<vector<uint8_t>> padded(rows + 2 * border, vector<uint8_t>(cols + 2 * border, 0));
    
        // Copy the input image into the center of the padded image
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                padded[i + border][j + border] = inputImg[i][j];
            }
        }
    
        // Apply the horizontal box filter
        vector<vector<uint8_t>> tempImg = padded;
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                double sum = 0.0;
                for (int kj = -border; kj <= border; kj++) {
                    sum += padded[i + border][j + border + kj];
                }
                tempImg[i + border][j + border] = static_cast<uint8_t>(round(sum / kernelSize));
            }
        }
        // Apply the vertical box filter
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                double sum = 0.0;
                for (int ki = -border; ki <= border; ki++) {
                    sum += tempImg[i + border + ki][j + border];
                }
                outputImg[i][j] = static_cast<uint8_t>(round(sum / kernelSize));
            }
        }
    
        return outputImg;
    }

vector<vector<vector<uint8_t>>> BoxFilter::applyBoxFilterSlidingRGB(
    const vector<vector<vector<uint8_t>>>& inputImg, int kernelSize) {
        if (inputImg.empty() || inputImg[0].empty()) {
            throw invalid_argument("Image is empty");
        }
    
        vector<vector<vector<uint8_t>>> outputImg;
        int rows = inputImg.size();  // Number of rows in the input image
        int cols = inputImg[0].size();  // Number of columns in the input image
        int channels = inputImg[0][0].size(); // Number of color channels
        if (kernelSize > rows || kernelSize > cols || kernelSize % 2 == 0) {
            throw invalid_argument("Invalid kernel size");
        }
        int border = kernelSize / 2; // Border size for padding to handle edges

        // Initialize the output image with the same size as the input
        outputImg.resize(rows, vector<vector<uint8_t>>(cols, vector<uint8_t>(channels, 0)));

        // Create a padded version of the input image to handle borders
        vector<vector<vector<uint8_t>>> padded(rows + 2 * border, vector<vector<uint8_t>>(cols + 2 * border, vector<uint8_t>(channels, 0)));

        // Copy the input image into the center of the padded image
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                for (int c = 0; c < channels; c++) {
                    padded[i + border][j + border][c] = inputImg[i][j][c];
                }
            }
        }

        // Apply the horizontal box filter
        vector<vector<vector<uint8_t>>> tempImg = padded;
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                for (int c = 0; c < channels; c++) {
                    double sum = 0.0;
                    for (int kj = -border; kj <= border; kj++) {
                        sum += padded[i + border][j + border + kj][c];
                    }
                    tempImg[i + border][j + border][c] = static_cast<uint8_t>(round(sum / kernelSize));
                }
            }
        }

        // Apply the vertical box filter
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                for (int c = 0; c < channels; c++) {
                    double sum = 0.0;
                    for (int ki = -border; ki <= border; ki++) {
                        sum += tempImg[i + border + ki][j + border][c];
                    }
                    outputImg[i][j][c] = static_cast<uint8_t>(round(sum / kernelSize));
                }
            }
        }

        return outputImg;

                }
    