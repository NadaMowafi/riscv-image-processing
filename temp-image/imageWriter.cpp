#include "imageWriter.hpp"
#include <fstream>

ImageWriter::ImageWriter() {}

ImageStatus ImageWriter::writeImage(const std::string& filePath, const Image& image) {
    std::ofstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        return ImageStatus::WRITE_ERROR;
    }

    std::string header = "P5\n" + std::to_string(image.metadata.width) + " " +
                         std::to_string(image.metadata.height) + "\n" +
                         std::to_string(image.metadata.maxValue) + "\n";
    file.write(header.c_str(), header.size());
    file.write(reinterpret_cast<const char*>(image.pixelData.data()), image.pixelData.size());
    file.close();

    return ImageStatus::SUCCESS;
}
