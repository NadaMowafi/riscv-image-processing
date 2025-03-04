#ifndef IMAGE_HPP
#define IMAGE_HPP

#include <vector>
#include <string>
#include <cstdint>
#include "../../utils/image_status.h"

enum class ImageFormat {
    UNKNOWN,
    PGM,
    PNG,
    JPEG,
    BMP
};

struct ImageMetadata {
    ImageFormat format = ImageFormat::UNKNOWN;
    uint32_t width = 0;
    uint32_t height = 0;
    uint32_t maxValue = 0;
};

struct Image {
    ImageMetadata metadata;
    std::vector<uint8_t> pixelData;
};

ImageStatus validateImage(const Image& image);

#endif // IMAGE_HPP
