#include "image_writer.hpp"
#include <fstream>

ImageWriter::ImageWriter() {}

ImageStatus ImageWriter::writeImage(const std::string& filePath, const Image& image) {
    switch (image.metadata.format) {
        case ImageFormat::PGM:
            return writePGM(filePath, image);
        case ImageFormat::PNG:
            return writePNG(filePath, image);
        case ImageFormat::JPEG:
            return writeJPEG(filePath, image);
        case ImageFormat::BMP:
            return writeBMP(filePath, image);
        default:
            return ImageStatus::UNSUPPORTED_FORMAT;
    }
}

ImageStatus ImageWriter::writePGM(const std::string& filePath, const Image& image) {
    std::ofstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        return ImageStatus::FILE_WRITE_ERROR;
    }

    // Write PGM header
    file << "P5\n";
    file << image.metadata.width << " " << image.metadata.height << "\n";
    file << image.metadata.maxValue << "\n";

    // Write pixel data from pixelMatrix
    for (const auto& row : image.pixelMatrix) {
        file.write(reinterpret_cast<const char*>(row.data()), row.size());
    }

    file.close();
    if (!file) {
        return ImageStatus::FILE_WRITE_ERROR;
    }

    return ImageStatus::SUCCESS;
}

ImageStatus ImageWriter::writePNG(const std::string&, const Image&) {
    return ImageStatus::UNSUPPORTED_FORMAT; // To be implemented later
}

ImageStatus ImageWriter::writeJPEG(const std::string&, const Image&) {
    return ImageStatus::UNSUPPORTED_FORMAT; // To be implemented later
}

ImageStatus ImageWriter::writeBMP(const std::string&, const Image&) {
    return ImageStatus::UNSUPPORTED_FORMAT; // To be implemented later
}
