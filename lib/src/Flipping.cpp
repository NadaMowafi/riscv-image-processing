#ifndef FLIPPING_CPP
#define FLIPPING_CPP

#include "Flipping.hpp"
#include <vector>
#include <iostream>
#include <riscv_vector.h>


template class ImageFlipper<uint8_t>;
// template class ImageFlipper<uint16_t>;
// template class ImageFlipper<uint32_t>;
// template class ImageFlipper<uint64_t>;

template <>
vuint8m1_t ImageFlipper<uint8_t>::create_reverse_index(size_t vl) {
    vector<uint8_t> idx(vl);
    for (size_t i = 0; i < vl; ++i)
        idx[i] = static_cast<uint8_t>(vl - 1 - i);
    return __riscv_vle8_v_u8m1(idx.data(), vl);
}


template <typename T>
void ImageFlipper<T>::__riscv_flip(Image<T> &image, FlippingDirection direction)
{

    if (image.pixelMatrix.empty())
    {
        throw FlipError("Pixel matrix is empty, cannot Flip image.");
    }
    switch (direction)
    {
    case FlippingDirection::VERTICAL:
        __riscv_flipVertical(image.pixelMatrix);
        break;
    case FlippingDirection::HORIZONTAL:
        __riscv_flipHorizontal(image.pixelMatrix);
        break;
    default:
        throw FlipError("Unsupported Flip direction.");
    }

    image.metadata.width = image.pixelMatrix[0].size();
    image.metadata.height = image.pixelMatrix.size();
}

template <>
void ImageFlipper<uint8_t>::__riscv_flipVertical(vector<vector<uint8_t>> &matrix)
{
    size_t height = matrix.size();
    size_t width = matrix[0].size();
    size_t half = height / 2;
    size_t counter =0;
    for (size_t i = 0; i < half; ++i) {
        auto& top = matrix[i];
        auto& bottom = matrix[height - 1 - i];
        size_t j = 0;

        while (j < width) {
            
            size_t vl = __riscv_vsetvl_e8m1(width - j);
            if (vl < 16) 
            {
                cout<<"vl is less than 16 and it is equals:"<< vl <<endl;
            }
           

            vuint8m1_t topVec = __riscv_vle8_v_u8m1(&top[j], vl);
            vuint8m1_t bottomVec = __riscv_vle8_v_u8m1(&bottom[j], vl);

            __riscv_vse8_v_u8m1(&top[j], bottomVec, vl);
            __riscv_vse8_v_u8m1(&bottom[j], topVec, vl);
            j += vl;
            if (i==0)
            {counter++;}
        }
    }
    cout<<"counter is equals:"<< counter <<endl;
}

template <>
void ImageFlipper<uint8_t>::__riscv_flipHorizontal(vector<vector<uint8_t>> &matrix)
{
    size_t height = matrix.size();
    size_t width = matrix[0].size();
    for (size_t i = 0; i < height; ++i) {
        auto& row = matrix[i];
        size_t left = 0;
        size_t right = width - 1;

        while (left < right) {
            size_t vl = __riscv_vsetvl_e8m1((right - left + 1) / 2);
            vuint8m1_t leftVec = __riscv_vle8_v_u8m1(&row[left], vl);
            vuint8m1_t rightVec = __riscv_vle8_v_u8m1(&row[right - vl + 1], vl);

            vuint8m1_t revIdx = create_reverse_index(vl);
            leftVec = __riscv_vrgather_vv_u8m1(leftVec, revIdx, vl);
            rightVec = __riscv_vrgather_vv_u8m1(rightVec, revIdx, vl);

            __riscv_vse8_v_u8m1(&row[left], rightVec, vl);
            __riscv_vse8_v_u8m1(&row[right - vl + 1], leftVec, vl);

            left += vl;
            right -= vl;
        }
    }

}

#endif // FLIPPING_CPP

          
