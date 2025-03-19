#ifndef IMAGE_WRITER_HPP
#define IMAGE_WRITER_HPP

#include "image.hpp"
#include <string>
using namespace std;

class ImageWriter {
public:
    ImageWriter();

    ImageStatus writeImage(const string& filePath, const Image& image);

private:
    ImageStatus writePGM(const string& filePath, const Image& image);
    ImageStatus writePNG(const string& filePath, const Image& image);
    ImageStatus writeJPEG(const string& filePath, const Image& image);
    ImageStatus writeBMP(const string& filePath, const Image& image);
};

#endif // IMAGE_WRITER_HPP
