#ifndef IMAGE_WRITER_HPP
#define IMAGE_WRITER_HPP

#include "Image.hpp"
#include <string>
using namespace std;

template <typename T = uint8_t>
class ImageWriter
{
public:
    ImageWriter();

    ImageStatus writeImage(const string &filePath, const Image<T> &image);

private:
    ImageStatus writePGM(const string &filePath, const Image<T> &image);
    ImageStatus writePNG(const string &filePath, const Image<T> &image);
    ImageStatus writeJPEG(const string &filePath, const Image<T> &image);
    ImageStatus writeBMP(const string &filePath, const Image<T> &image);
};

#endif // IMAGE_WRITER_HPP
