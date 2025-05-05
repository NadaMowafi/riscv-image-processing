#ifndef IMAGE_CPP
#define IMAGE_CPP

#include "Image.hpp"

template <typename T>
ImageStatus validateImage(const Image<T> &image)
{
    if (image.pixelData.empty() || image.metadata.width == 0 || image.metadata.height == 0)
    {
        return ImageStatus::UNSUPPORTED_FORMAT;
    }
    return ImageStatus::SUCCESS;
}
template ImageStatus validateImage<uint8_t>(const Image<uint8_t> &image);
template ImageStatus validateImage<uint16_t>(const Image<uint16_t> &image);
template ImageStatus validateImage<uint32_t>(const Image<uint32_t> &image);

#endif // IMAGE_CPP
