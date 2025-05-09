#ifndef ROTATE_CPP
#define ROTATE_CPP

/* Rotate.cpp */
#include "Rotate.hpp"
#include "Flipping.hpp"
#include <riscv_vector.h>
#include <stdexcept>



template class ImageRotator<uint8_t>;
// template class ImageRotator<uint16_t>;
// template class ImageRotator<uint32_t>;
// template class ImageRotator<uint64_t>;

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
        __riscv_rotate90CW(image.pixelMatrix);
        break;
    case RotationDirection::CCW_90:
        __riscv_rotate90CCW(image.pixelMatrix);
        break;
    case RotationDirection::ROTATE_180:
        __riscv_rotate180(image.pixelMatrix);
        break;
    default:
        throw RotationError("Unsupported rotation direction.");
    }

    image.metadata.width = image.pixelMatrix[0].size();
    image.metadata.height = image.pixelMatrix.size();
}

vuint8m1_t create_reverse_index(size_t vl) {
    std::vector<uint8_t> idx(vl);
    for (size_t i = 0; i < vl; ++i)
        idx[i] = static_cast<uint8_t>(vl - 1 - i);
    return __riscv_vle8_v_u8m1(idx.data(), vl);
}

template <>
void ImageRotator<uint8_t>::__riscv_rotate90CW(std::vector<std::vector<uint8_t>>& matrix) {
    size_t height = matrix.size();
    size_t width = matrix[0].size();
    std::vector<std::vector<uint8_t>> rotated(width, std::vector<uint8_t>(height));

    for (size_t col = 0; col < width; ++col) {
        size_t row = 0;
        while (row < height) {
            size_t vl = __riscv_vsetvl_e8m1(height - row); 
            std::vector<uint8_t> temp(vl);
            for (size_t i = 0; i < vl; ++i) {
                temp[i] = matrix[row + i][col];
            }
            vuint8m1_t vdata = __riscv_vle8_v_u8m1(temp.data(), vl);
            // Extract elements from the RISC-V vector into a standard array
            uint8_t extracted[vl];
            __riscv_vse8_v_u8m1(extracted, vdata, vl);
            for (size_t i = 0; i < vl; ++i) {
                rotated[col][height - 1 - (row + i)] = extracted[i];
            }
            row += vl;
        }
    }
    matrix = std::move(rotated);
}

template<>
void ImageRotator<uint8_t>::__riscv_rotate90CCW(std::vector<std::vector<uint8_t>>& matrix) {
    size_t height = matrix.size();
    size_t width = matrix[0].size();
    std::vector<std::vector<uint8_t>> rotated(width, std::vector<uint8_t>(height));

    for (size_t col = 0; col < width; ++col) {
        size_t row = 0;
        while (row < height) {
            size_t vl = __riscv_vsetvl_e8m1(height - row); 
            std::vector<uint8_t> temp(vl);
            for (size_t i = 0; i < vl; ++i) {
                temp[i] = matrix[row + i][col];
            }
            vuint8m1_t vdata = __riscv_vle8_v_u8m1(temp.data(), vl);
            uint8_t extracted[vl];
            __riscv_vse8_v_u8m1(extracted, vdata, vl);
            for (size_t i = 0; i < vl; ++i) {
                rotated[width - 1 - col][row + i] = extracted[i];
            }
            row += vl;
        }
    }
    matrix = std::move(rotated);
}

template <>
void ImageRotator<uint8_t>::__riscv_rotate180(vector<vector<uint8_t>> &matrix)
{
    size_t rows = matrix.size();
    size_t cols = matrix[0].size();
     for (size_t i=0;i<(rows+1)/2;i++)
     {
        size_t j =0;
       while (j<cols)
       {
       size_t vl = __riscv_vsetvl_e8m1(cols - j);

       vuint8m1_t topVec = __riscv_vle8_v_u8m1(&matrix[i][j], vl);
       vuint8m1_t bottomVec = __riscv_vle8_v_u8m1(&matrix[rows-1-i][cols-j-vl], vl);

         vuint8m1_t revIdx = create_reverse_index(vl);
         vuint8m1_t revTop = __riscv_vrgather_vv_u8m1(topVec, revIdx, vl);
         vuint8m1_t revBottom = __riscv_vrgather_vv_u8m1(bottomVec, revIdx, vl);
       __riscv_vse8_v_u8m1(&matrix[i][j],revBottom,vl);
       __riscv_vse8_v_u8m1(&matrix[rows-1-i][cols-j-vl],revTop ,vl);

       j+=vl;
       }
     }    
}

#endif // ROTATE_CPP