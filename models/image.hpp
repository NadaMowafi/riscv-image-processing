#ifndef IMAGE_HPP
#define IMAGE_HPP

#include <vector>
#include <string>
#include <cstdint>
#include "image_status.hpp"
using namespace std;

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
    vector<uint8_t> pixelData; // Flat array (if you just want the matrix)
    vector<vector<uint8_t>> pixelMatrix; // 2D pixel array
};


ImageStatus validateImage(const Image& image);

#endif // IMAGE_HPP
