#include "image_reader.hpp"
#include "image_writer.hpp"
#include "rotate.hpp"
#include "Flipping.hpp"
#include <iostream>
using namespace std;

int main() {
    ImageReader reader;
    ImageWriter writer;
    ImageRotator rotator;
    ImageFlipper flipper;
    Image image;

    // Read the image
    ImageStatus status = reader.readImage("barb.512.pgm", image);
    if (status != ImageStatus::SUCCESS) {
        cerr << "Failed to read image: " << static_cast<int>(status) << endl;
        return 1;
    }

    cout << "Image read successfully. Size: "
              << image.metadata.width << "x"
              << image.metadata.height << endl;

    // // Rotate 90 degrees CW
    // rotator.rotate(image, RotationDirection::CW_90);
    // status = writer.writeImage("output_90CW.pgm", image);
    // if (status != ImageStatus::SUCCESS) {
    //     cerr << "Failed to write 90CW image: " << static_cast<int>(status) << endl;
    //     return 1;
    // }
    // cout << "90-degree CW rotated image written successfully." << endl;

    // Rotate 90 degrees CCW
    rotator.rotate(image, RotationDirection::CCW_90);
    status = writer.writeImage("output_90CCW.pgm", image);
    if (status != ImageStatus::SUCCESS) {
        cerr << "Failed to write 90CCW image: " << static_cast<int>(status) << endl;
        return 1;
    }
    cout << "90-degree CCW rotated image written successfully." << endl;

    // // Rotate 180 degrees
    // rotator.rotate(image, RotationDirection::ROTATE_180);
    // status = writer.writeImage("output_180.pgm", image);
    // if (status != ImageStatus::SUCCESS) {
    //     cerr << "Failed to write 180-degree rotated image: " << static_cast<int>(status) << endl;
    //     return 1;
    // }
    // cout << "180-degree rotated image written successfully." << endl;

    //---------------------------------------FLIPPING----------------------------------------------
    // Read the image
    ImageStatus status = reader.readImage("barb.512.pgm", image);
    if (status != ImageStatus::SUCCESS) {
        cerr << "Failed to read image: " << static_cast<int>(status) << endl;
        return 1;
    }

    std::cout << "Image read successfully. Size: "
              << image.metadata.width << "x"
              << image.metadata.height << endl;

    // flipper.flip(image, FlippingDirection::VERTICAL);
    flipper.flip(image, FlippingDirection::HORIZONTAL);
    status = writer.writeImage("Flipped_Vertical.pgm", image);
    if (status != ImageStatus::SUCCESS) {
        cerr << "Failed to write image: " << static_cast<int>(status) << endl;
        return 1;
    }
    cout << "Horizontal flipped image written successfully." << endl;


    return 0;
}