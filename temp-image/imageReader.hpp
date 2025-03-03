#ifndef IMAGE_READER_HPP
#define IMAGE_READER_HPP

#include "image.hpp"
#include <vector>

class ImageReader {
public:
    ImageReader();

    ImageStatus readImage(const std::string& filePath, Image& image);

private:
    ImageFormat detectFormat(const std::vector<uint8_t>& rawData);

    ImageStatus parseMetadata(const std::vector<uint8_t>& rawData, ImageMetadata& metadata);

    ImageStatus parsePGM(const std::vector<uint8_t>& rawData, Image& image);
    ImageStatus parsePNG(const std::vector<uint8_t>& rawData, Image& image);
    ImageStatus parseJPEG(const std::vector<uint8_t>& rawData, Image& image);
    ImageStatus parseBMP(const std::vector<uint8_t>& rawData, Image& image);
};

#endif // IMAGE_READER_HPP
