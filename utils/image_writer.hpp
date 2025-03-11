#ifndef IMAGE_WRITER_HPP
#define IMAGE_WRITER_HPP

#include "image.hpp"
#include <string>

class ImageWriter {
public:
    ImageWriter();

    ImageStatus writeImage(const std::string& filePath, const Image& image);

private:
    ImageStatus writePGM(const std::string& filePath, const Image& image);
    ImageStatus writePNG(const std::string& filePath, const Image& image);
    ImageStatus writeJPEG(const std::string& filePath, const Image& image);
    ImageStatus writeBMP(const std::string& filePath, const Image& image);
};

#endif // IMAGE_WRITER_HPP
