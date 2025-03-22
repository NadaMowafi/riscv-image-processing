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

#endif // IMAGE_CPP
