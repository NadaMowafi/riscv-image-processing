#ifndef FLIPPING_CPP
#define FLIPPING_CPP

#include "Flipping.hpp"
#include <vector>

template class ImageFlipper<uint8_t>;
template class ImageFlipper<uint16_t>;
template class ImageFlipper<uint32_t>;
template class ImageFlipper<uint64_t>;

template <typename T>
void ImageFlipper<T>::flip(Image<T> &image, FlippingDirection direction)
{

    if (image.pixelMatrix.empty())
    {
        throw FlipError("Pixel matrix is empty, cannot Flip image.");
    }
    switch (direction)
    {
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

template <typename T>
void ImageFlipper<T>::flipVertical(vector<vector<T>> &matrix)
{
    size_t height = matrix.size();
    size_t width = matrix[0].size();

    for (size_t i = 0; i < height / 2; ++i)
    {
        for (size_t j = 0; j < width; ++j)
        {
            swap(matrix[i][j], matrix[height - 1 - i][j]);
        }
    }
}

template <typename T>
void ImageFlipper<T>::flipHorizontal(vector<vector<T>> &matrix)
{
    size_t height = matrix.size();
    size_t width = matrix[0].size();

    for (size_t i = 0; i < height; ++i)
    {
        for (size_t j = 0; j < width / 2; ++j)
        {
            swap(matrix[i][j], matrix[i][width - 1 - j]);
        }
    }
}

#endif // FLIPPING_CPP
