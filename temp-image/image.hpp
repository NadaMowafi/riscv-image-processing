#ifndef IMAGE_HPP
#define IMAGE_HPP

#include <vector>
#include <string>
#include <cstdint>

// Here we can put the enums that mazen told us about ....
enum class ImageStatus {
    SUCCESS,
    FILE_NOT_FOUND,
    INVALID_FORMAT,
    READ_ERROR,
    WRITE_ERROR,
    PARSE_ERROR
};

struct ImageMetadata {
    std::string format;
    uint32_t width;
    uint32_t height;
    uint16_t maxValue;
};

struct Image {
    ImageMetadata metadata;
    std::vector<uint8_t> pixelData;
};

ImageStatus validateImage(const Image& image);

#endif // IMAGE_HPP
