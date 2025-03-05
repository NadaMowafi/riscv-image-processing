#include "BoxFilter.h"
#include <vector>
#include <stdexcept>
#include <cstdint>

using namespace std;

vector<vector<uint8_t>> BoxFilter(const vector<vector<uint8_t>> &inputImg, int kernelSize) {
    // Ensure the kernel size is odd (3x3 , 5x5, etc)
    if (kernelSize % 2 == 0) {
        throw invalid_argument("Kernel size (ksize) must be an odd number.");
    }

    vector<vector<uint8_t>> outputImg;
    int rows = inputImg.size();  // Number of rows in the input image
    int cols = inputImg[0].size();  // Number of columns in the input image
    int border = kernelSize / 2; // Border size for padding to handle edges

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
            int sum = 0;
            for (int kj = -border; kj <= border; kj++) {
                sum += padded[i + border][j + border + kj];
            }
            tempImg[i + border][j + border] = sum / kernelSize;
        }
    }

    // Apply the vertical box filter
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            int sum = 0;
            for (int ki = -border; ki <= border; ki++) {
                sum += tempImg[i + border + ki][j + border];
            }
            outputImg[i][j] = sum / kernelSize;
        }
    }

    return outputImg;
}