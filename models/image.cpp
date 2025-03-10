#include "image.hpp"

ImageStatus validateImage(const Image& image) {
    if (image.pixelData.empty() || image.metadata.width == 0 || image.metadata.height == 0) {
        return ImageStatus::UNSUPPORTED_FORMAT;
    }
    return ImageStatus::SUCCESS;
}
