#include "Flipping.h"

void ImageFlipper::(){



    switch (direction) {
        case ::Vertical:
            flipVertical();
            break;
        case ::Horizontal:
            flipHorizontal();
            break;
    }

    image.metadata.width = image.pixelMatrix[0].size();
    image.metadata.height = image.pixelMatrix.size();
}



void ImageFlipper::flipVertical(std::vector<std::vector<uint8_t>>& matrix) {
    size_t height = matrix.size();
    size_t width = matrix[0].size();

    std::vector<std::vector<uint8_t>> Flipped(height, std::vector<uint8_t>(width));

    for (size_t i = 0; i < height / 2; ++i) {
        for (size_t j = 0; j < width; ++j) {
            std::swap(matrix[i][j], matrix[height - 1 - i][j]);
        }
    }
    matrix = std::move(Flipped);
}

void ImageFlipper::flipHorizontal(std::vector<std::vector<uint8_t>>& matrix) {
    size_t height = matrix.size();
    size_t width = matrix[0].size();

    std::vector<std::vector<uint8_t>> Flipped(height, std::vector<uint8_t>(width));
    
    for (size_t i = 0; i < height; ++i) {
        for (size_t j = 0; j < width / 2; ++j) {
            std::swap(matrix[i][j], matrix[i][width - 1 - j]);
        }
    }
    matrix = std::move(Flipped);
}