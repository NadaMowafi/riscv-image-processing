#ifndef IMAGE_READER_HPP
#define IMAGE_READER_HPP

#include "Image.hpp"
#include <vector>
using namespace std;

template <typename T = uint8_t>
class ImageReader
{
public:
    ImageReader();

    ImageStatus readImage(const string &filePath, Image<T> &image);

private:
    ImageFormat detectFormat(const vector<T> &rawData);

    ImageStatus parseMetadata(const vector<T> &rawData, ImageMetadata &metadata);

    ImageStatus parsePGM(const vector<T> &rawData, Image<T> &image);
    ImageStatus parsePNG(const vector<T> &rawData, Image<T> &image);
    ImageStatus parseJPEG(const vector<T> &rawData, Image<T> &image);
    ImageStatus parseBMP(const vector<T> &rawData, Image<T> &image);
};

#endif // IMAGE_READER_HPP
