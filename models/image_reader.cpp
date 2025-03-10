#include "image_reader.hpp"
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
        return ImageStatus::FILE_READ_ERROR;
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
    std::string line;
    std::string magicNumber;
    uint32_t width = 0, height = 0, maxValue = 0;

    // Read magic number
    std::getline(stream, line);
    if (line != "P5") {
        return ImageStatus::PARSE_ERROR;
    }
    magicNumber = line;

    // Read width, height, and maxValue while skipping comments
    while (std::getline(stream, line)) {
        if (line.empty() || line[0] == '#') continue;
        std::istringstream dimensions(line);
        dimensions >> width >> height;
        if (width > 0 && height > 0) break;
    }
    while (std::getline(stream, line)) {
        if (line.empty() || line[0] == '#') continue;
        std::istringstream maxValStream(line);
        maxValStream >> maxValue;
        if (maxValue > 0) break;
    }

    if (width == 0 || height == 0 || maxValue == 0) {
        return ImageStatus::PARSE_ERROR;
    }

    image.metadata.width = width;
    image.metadata.height = height;
    image.metadata.maxValue = maxValue;

    // Get position of pixel data after header
    size_t headerSize = stream.tellg();
    if (headerSize == -1) {
        return ImageStatus::PARSE_ERROR;
    }

    if (rawData.size() < headerSize + width * height) {
        return ImageStatus::FILE_READ_ERROR;
    }

    image.pixelData.assign(rawData.begin() + headerSize, rawData.begin() + headerSize + width * height);

    // Fill pixelMatrix
    image.pixelMatrix.resize(height, std::vector<uint8_t>(width));
    for (uint32_t i = 0; i < height; ++i) {
        for (uint32_t j = 0; j < width; ++j) {
            image.pixelMatrix[i][j] = image.pixelData[i * width + j];
        }
    }

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
