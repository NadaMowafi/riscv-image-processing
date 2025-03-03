#ifndef IMAGE_WRITER_HPP
#define IMAGE_WRITER_HPP

#include "image.hpp"

class ImageWriter {
public:
    ImageWriter();

    ImageStatus writeImage(const std::string& filePath, const Image& image);
};

#endif // IMAGE_WRITER_HPP
