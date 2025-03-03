#include "imageReader.hpp"
#include <fstream>

ImageReader::ImageReader() {}

ImageStatus ImageReader::readImage(const std::string& filePath, Image& image) {
    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        return ImageStatus::FILE_NOT_FOUND;
    }

    std::vector<uint8_t> rawData((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();

    if (rawData.empty()) {
        return ImageStatus::READ_ERROR;
    }

    ImageStatus metadataStatus = parseMetadata(rawData, image.metadata);
    if (metadataStatus != ImageStatus::SUCCESS) {
        return metadataStatus;
    }

    size_t headerSize = 15; // Example offset
    image.pixelData.assign(rawData.begin() + headerSize, rawData.end());

    return validateImage(image);
}

ImageStatus ImageReader::parseMetadata(const std::vector<uint8_t>& rawData, ImageMetadata& metadata) {
    if (rawData.size() < 15) {
        return ImageStatus::PARSE_ERROR;
    }
    
    metadata.format = "PGM"; // Example (placeholder)
    metadata.width = 256;
    metadata.height = 256;
    metadata.maxValue = 255;

    return ImageStatus::SUCCESS;
}
