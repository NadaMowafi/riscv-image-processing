#include "BoxFilter.h"
#include <vector>
#include <stdexcept>

using namespace std;

vector<vector<vector<int>>> BoxFilter(const vector<vector<vector<int>>> &inputImg, int kernelSize) {
     // Ensure the kernel size is odd (3x3 , 5x5, etc)
    if (kernelSize % 2 == 0) {
        throw invalid_argument("Kernel size (ksize) must be an odd number.");
    }

    vector<vector<vector<int>>> outputImg;
    int rows = inputImg.size();  // Number of rows in the input image
    int cols = inputImg[0].size();  // Number of columns in the input image
    int channels = inputImg[0][0].size(); // Number of color channels
    int border = kernelSize / 2; // Border size for padding to handle edges

    // Initialize the output image with the same size as the input
    outputImg.resize(rows, vector<vector<int>>(cols, vector<int>(channels, 0)));

    // Create a padded version of the input image to handle borders
    vector<vector<vector<int>>> padded(rows + 2 * border, vector<vector<int>>(cols + 2 * border, vector<int>(channels, 0)));

    // Copy the input image into the center of the padded image
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            for (int c = 0; c < channels; c++) {
                padded[i + border][j + border][c] = inputImg[i][j][c];
            }
        }
    }

    // Apply the horizontal box filter
    vector<vector<vector<int>>> tempImg = padded;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            for (int c = 0; c < channels; c++) {
                int sum = 0;
                for (int kj = -border; kj <= border; kj++) {
                    sum += padded[i + border][j + border + kj][c];
                }
                tempImg[i + border][j + border][c] = sum / kernelSize;
            }
        }
    }

    // Apply the vertical box filter
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            for (int c = 0; c < channels; c++) {
                int sum = 0;
                for (int ki = -border; ki <= border; ki++) {
                    sum += tempImg[i + border + ki][j + border][c];
                }
                outputImg[i][j][c] = sum / kernelSize;
            }
        }
    }

    return outputImg;
}