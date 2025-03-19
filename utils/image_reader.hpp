#ifndef IMAGE_READER_HPP
#define IMAGE_READER_HPP

#include "image.hpp"
#include <vector>
using namespace std;

class ImageReader {
public:
    ImageReader();

    ImageStatus readImage(const string& filePath, Image& image);

private:
    ImageFormat detectFormat(const vector<uint8_t>& rawData);

    ImageStatus parseMetadata(const vector<uint8_t>& rawData, ImageMetadata& metadata);

    ImageStatus parsePGM(const vector<uint8_t>& rawData, Image& image);
    ImageStatus parsePNG(const vector<uint8_t>& rawData, Image& image);
    ImageStatus parseJPEG(const vector<uint8_t>& rawData, Image& image);
    ImageStatus parseBMP(const vector<uint8_t>& rawData, Image& image);
};

#endif // IMAGE_READER_HPP
