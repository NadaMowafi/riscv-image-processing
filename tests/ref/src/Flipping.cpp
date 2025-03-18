#include "Flipping.hpp"
#include <vector>


void ImageFlipper::flip(Image&image, FlippingDirection direction){

    if (image.pixelMatrix.empty()) {
        throw FlipError("Pixel matrix is empty, cannot Flip image.");
    }
    switch (direction) {
        case FlippingDirection::VERTICAL:
            flipVertical(image.pixelMatrix);
            break;
        case FlippingDirection::HORIZONTAL:
            flipHorizontal(image.pixelMatrix);
            break;
        default:
            throw FlipError("Unsupported Flip direction.");
    }

    image.metadata.width = image.pixelMatrix[0].size();
    image.metadata.height = image.pixelMatrix.size();
}



void ImageFlipper::flipVertical(std::vector<std::vector<uint8_t>>& matrix) {
    size_t height = matrix.size();
    size_t width = matrix[0].size();


    for (size_t i = 0; i < height / 2; ++i) {
        for (size_t j = 0; j < width; ++j) {
            std::swap(matrix[i][j], matrix[height - 1 - i][j]);
        }
    }
    
}

void ImageFlipper::flipHorizontal(std::vector<std::vector<uint8_t>>& matrix) {
    size_t height = matrix.size();
    size_t width = matrix[0].size();

    
    for (size_t i = 0; i < height; ++i) {
        for (size_t j = 0; j < width / 2; ++j) {
            std::swap(matrix[i][j], matrix[i][width - 1 - j]);
        }
    }
    
}
