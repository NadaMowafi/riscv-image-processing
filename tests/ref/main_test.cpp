#include "image_reader.hpp"
#include "image_writer.hpp"
#include "rotation.h"
#include <iostream>

int main() {
    ImageReader reader;
    ImageWriter writer;
    ImageRotator rotator;
    Image image;

    // Read the image
    ImageStatus status = reader.readImage("barb.512 (1).pgm", image);
    if (status != ImageStatus::SUCCESS) {
        std::cerr << "Failed to read image: " << static_cast<int>(status) << std::endl;
        return 1;
    }

    std::cout << "Image read successfully. Size: "
              << image.metadata.width << "x"
              << image.metadata.height << std::endl;

    // // Rotate 90 degrees CW
    // rotator.rotate(image, RotationDirection::CW_90);
    // status = writer.writeImage("output_90CW.pgm", image);
    // if (status != ImageStatus::SUCCESS) {
    //     std::cerr << "Failed to write 90CW image: " << static_cast<int>(status) << std::endl;
    //     return 1;
    // }
    // std::cout << "90-degree CW rotated image written successfully." << std::endl;

    // Rotate 90 degrees CCW
    rotator.rotate(image, RotationDirection::CCW_90);
    status = writer.writeImage("output_90CCW.pgm", image);
    if (status != ImageStatus::SUCCESS) {
        std::cerr << "Failed to write 90CCW image: " << static_cast<int>(status) << std::endl;
        return 1;
    }
    std::cout << "90-degree CCW rotated image written successfully." << std::endl;

    // // Rotate 180 degrees
    // rotator.rotate(image, RotationDirection::ROTATE_180);
    // status = writer.writeImage("output_180.pgm", image);
    // if (status != ImageStatus::SUCCESS) {
    //     std::cerr << "Failed to write 180-degree rotated image: " << static_cast<int>(status) << std::endl;
    //     return 1;
    // }
    // std::cout << "180-degree rotated image written successfully." << std::endl;

    return 0;
}