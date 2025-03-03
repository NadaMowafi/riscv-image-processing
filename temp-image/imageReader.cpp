#include "imageReader.hpp"
#include <fstream>
#include <sstream>

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

    image.metadata.format = detectFormat(rawData);
    switch (image.metadata.format) {
        case ImageFormat::PGM:
            return parsePGM(rawData, image);
        case ImageFormat::PNG:
            return parsePNG(rawData, image);
        case ImageFormat::JPEG:
            return parseJPEG(rawData, image);
        case ImageFormat::BMP:
            return parseBMP(rawData, image);
        default:
            return ImageStatus::UNSUPPORTED_FORMAT;
    }
}

ImageFormat ImageReader::detectFormat(const std::vector<uint8_t>& rawData) {
    if (rawData.size() >= 2 && rawData[0] == 'P' && rawData[1] == '5') {
        return ImageFormat::PGM;
    } else if (rawData.size() >= 8 &&
               rawData[0] == 0x89 && rawData[1] == 'P' && rawData[2] == 'N' &&
               rawData[3] == 'G' && rawData[4] == 0x0D &&
               rawData[5] == 0x0A && rawData[6] == 0x1A &&
               rawData[7] == 0x0A) {
        return ImageFormat::PNG;
    } else if (rawData.size() >= 2 && rawData[0] == 0xFF && rawData[1] == 0xD8) {
        return ImageFormat::JPEG;
    } else if (rawData.size() >= 2 && rawData[0] == 'B' && rawData[1] == 'M') {
        return ImageFormat::BMP;
    }
    return ImageFormat::UNKNOWN;
}

ImageStatus ImageReader::parseMetadata(const std::vector<uint8_t>&, ImageMetadata&) {
    return ImageStatus::SUCCESS; // Reserved for shared metadata logic if needed.
}

ImageStatus ImageReader::parsePGM(const std::vector<uint8_t>& rawData, Image& image) {
    std::istringstream stream(std::string(rawData.begin(), rawData.end()));
    std::string magicNumber;
    uint32_t width, height, maxValue;

    stream >> magicNumber >> width >> height >> maxValue;
    stream.ignore(1); // Skip whitespace

    if (magicNumber != "P5") {
        return ImageStatus::PARSE_ERROR;
    }

    image.metadata.width = width;
    image.metadata.height = height;
    image.metadata.maxValue = maxValue;

    size_t headerSize = stream.tellg();
    image.pixelData.assign(rawData.begin() + headerSize, rawData.end());

    return ImageStatus::SUCCESS;
}

// Placeholder implementations for future formats
ImageStatus ImageReader::parsePNG(const std::vector<uint8_t>&, Image&) {
    return ImageStatus::UNSUPPORTED_FORMAT; // Implement later
}

ImageStatus ImageReader::parseJPEG(const std::vector<uint8_t>&, Image&) {
    return ImageStatus::UNSUPPORTED_FORMAT; // Implement later
}

ImageStatus ImageReader::parseBMP(const std::vector<uint8_t>&, Image&) {
    return ImageStatus::UNSUPPORTED_FORMAT; // Implement later
}
