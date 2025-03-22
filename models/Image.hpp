#ifndef IMAGE_HPP
#define IMAGE_HPP

#include <vector>
#include <string>
#include <cstdint>
#include "ImageStatus.hpp"
using namespace std;

enum class ImageFormat
{
    UNKNOWN,
    PGM,
    PNG,
    JPEG,
    BMP
};

struct ImageMetadata
{
    ImageFormat format = ImageFormat::UNKNOWN;
    uint32_t width = 0;
    uint32_t height = 0;
    uint32_t maxValue = 0;
};

template <typename T = uint8_t>
struct Image
{
    ImageMetadata metadata;
    vector<T> pixelData;           // Flat array (if you just want the matrix)
    vector<vector<T>> pixelMatrix; // 2D pixel array
};

template <typename T = uint8_t>
ImageStatus validateImage(const Image<T> &image);

#endif // IMAGE_HPP
