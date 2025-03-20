/* rotate.cpp */
#include "rotate.hpp"

void ImageRotator::rotate(Image& image, RotationDirection direction) {
    if (image.metadata.width == 0 || image.metadata.height == 0) {
        throw RotationError("Invalid image dimensions for rotation.");
    }

    if (image.pixelMatrix.empty()) {
        throw RotationError("Pixel matrix is empty, cannot rotate image.");
    }

    switch (direction) {
        case RotationDirection::CW_90:
            rotate90CW(image.pixelMatrix);
            break;
        case RotationDirection::CCW_90:
            rotate90CCW(image.pixelMatrix);
            break;
        case RotationDirection::ROTATE_180:
            rotate180(image.pixelMatrix);
            break;
        default:
            throw RotationError("Unsupported rotation direction.");
    }

    image.metadata.width = image.pixelMatrix[0].size();
    image.metadata.height = image.pixelMatrix.size();
}

void ImageRotator::rotate90CW(vector<vector<uint8_t>>& matrix) {
    size_t newHeight = matrix[0].size();
    size_t newWidth = matrix.size();
    vector<vector<uint8_t>> rotated(newHeight, vector<uint8_t>(newWidth));

    for (size_t i = 0; i < newWidth; ++i) {
        for (size_t j = 0; j < newHeight; ++j) {
            rotated[j][newWidth - 1 - i] = matrix[i][j];
        }
    }

    matrix = move(rotated);
}

void ImageRotator::rotate90CCW(vector<vector<uint8_t>>& matrix) {
    size_t newHeight = matrix[0].size();
    size_t newWidth = matrix.size();
    vector<vector<uint8_t>> rotated(newHeight, vector<uint8_t>(newWidth));

    for (size_t i = 0; i < newWidth; ++i) {
        for (size_t j = 0; j < newHeight; ++j) {
            rotated[newHeight - 1 - j][i] = matrix[i][j];
        }
    }

    matrix = move(rotated);
}

void ImageRotator::rotate180(vector<vector<uint8_t>>& matrix) {
    size_t row = matrix.size();
    size_t col = matrix[0].size();
    for (size_t i = 0; i < row / 2; i++) {
        for (size_t j = 0; j < col; j++) {
            if (i == row - i - 1 && j >= (col - i) / 2) break;
            swap(matrix[i][j], matrix[row - i - 1][col - j - 1]);
        }
    }
}
