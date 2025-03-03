#ifndef IMAGE_READER_HPP
#define IMAGE_READER_HPP

#include "image.hpp"

class ImageReader {
public:
    ImageReader();

    ImageStatus readImage(const std::string& filePath, Image& image);

private:
    ImageStatus parseMetadata(const std::vector<uint8_t>& rawData, ImageMetadata& metadata);
};

#endif // IMAGE_READER_HPP
