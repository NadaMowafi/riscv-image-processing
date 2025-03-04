#include "image_reader.hpp"
#include "image_writer.hpp"
#include <iostream>

int main() {
    ImageReader reader;
    ImageWriter writer;
    Image image;

    ImageStatus status = reader.readImage("3things.256.pgm", image);
    if (status != ImageStatus::SUCCESS) {
        std::cerr << "Failed to read image: " << static_cast<int>(status) << std::endl;
        return 1;
    }

    std::cout << "Image read successfully. Size: "
              << image.metadata.width << "x"
              << image.metadata.height << std::endl;

    status = writer.writeImage("output.pgm", image);
    if (status != ImageStatus::SUCCESS) {
        std::cerr << "Failed to write image: " << static_cast<int>(status) << std::endl;
        return 1;
    }

    std::cout << "Image written successfully." << std::endl;
    return 0;
}
