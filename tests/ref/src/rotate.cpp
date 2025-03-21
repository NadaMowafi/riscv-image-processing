#ifndef ROTATE_CPP
#define ROTATE_CPP

/* Rotate.cpp */
#include "Rotate.hpp"

template class ImageRotator<uint8_t>;
template class ImageRotator<uint16_t>;
template class ImageRotator<uint32_t>;
template class ImageRotator<uint64_t>;

template <typename T>
void ImageRotator<T>::rotate(Image<T> &image, RotationDirection direction)
{
    if (image.metadata.width == 0 || image.metadata.height == 0)
    {
        throw RotationError("Invalid image dimensions for rotation.");
    }

    if (image.pixelMatrix.empty())
    {
        throw RotationError("Pixel matrix is empty, cannot rotate image.");
    }

    switch (direction)
    {
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

template <typename T>
void ImageRotator<T>::rotate90CW(vector<vector<T>> &matrix)
{
    size_t newHeight = matrix[0].size();
    size_t newWidth = matrix.size();
    vector<vector<T>> rotated(newHeight, vector<T>(newWidth));

    for (size_t i = 0; i < newWidth; ++i)
    {
        for (size_t j = 0; j < newHeight; ++j)
        {
            rotated[j][newWidth - 1 - i] = matrix[i][j];
        }
    }

    matrix = move(rotated);
}

template <typename T>
void ImageRotator<T>::rotate90CCW(vector<vector<T>> &matrix)
{
    size_t newHeight = matrix[0].size();
    size_t newWidth = matrix.size();
    vector<vector<T>> rotated(newHeight, vector<T>(newWidth));

    for (size_t i = 0; i < newWidth; ++i)
    {
        for (size_t j = 0; j < newHeight; ++j)
        {
            rotated[newHeight - 1 - j][i] = matrix[i][j];
        }
    }

    matrix = move(rotated);
}

template <typename T>
void ImageRotator<T>::rotate180(vector<vector<T>> &matrix)
{
    size_t row = matrix.size();
    size_t col = matrix[0].size();
    for (size_t i = 0; i < row / 2; i++)
    {
        for (size_t j = 0; j < col; j++)
        {
            if (i == row - i - 1 && j >= (col - i) / 2)
                break;
            swap(matrix[i][j], matrix[row - i - 1][col - j - 1]);
        }
    }
}

#endif // ROTATE_CPP
