#include "ImageReader.hpp"
#include "ImageWriter.hpp"
#include "BoxFilter_Vector_Template.hpp"
#include <iostream>
#include <vector>

using namespace std;

int main() {
    // Define readers and writers for each data type
    ImageReader<uint8_t> reader8;
    ImageWriter<uint8_t> writer8;

    ImageReader<uint16_t> reader16;
    ImageWriter<uint16_t> writer16;

    ImageReader<uint32_t> reader32;
    ImageWriter<uint32_t> writer32;

    // Define images for each data type
    Image<uint8_t> image8;
    Image<uint16_t> image16;
    Image<uint32_t> image32;

    // Read the image for uint8_t
    ImageStatus status = reader8.readImage("barb.512.pgm", image8);
    if (status != ImageStatus::SUCCESS) {
        cerr << "Failed to read uint8_t image: " << static_cast<int>(status) << endl;
        return 1;
    }
    cout << "uint8_t Image read successfully. Size: "
         << image8.metadata.width << "x" << image8.metadata.height << endl;

    // Read the image for uint16_t
    status = reader16.readImage("barb.512.pgm", image16);
    if (status != ImageStatus::SUCCESS) {
        cerr << "Failed to read uint16_t image: " << static_cast<int>(status) << endl;
        return 1;
    }
    cout << "uint16_t Image read successfully. Size: "
         << image16.metadata.width << "x" << image16.metadata.height << endl;

    // Read the image for uint32_t
    status = reader32.readImage("barb.512.pgm", image32);
    if (status != ImageStatus::SUCCESS) {
        cerr << "Failed to read uint32_t image: " << static_cast<int>(status) << endl;
        return 1;
    }
    cout << "uint32_t Image read successfully. Size: "
         << image32.metadata.width << "x" << image32.metadata.height << endl;

    // Define kernel size
    int kernelSize = 3;

    // Apply BoxFilter for uint8_t
    cout << "Applying BoxFilter for uint8_t..." << endl;
    vector<vector<uint8_t>> result8 = BoxFilter(image8.pixelMatrix, kernelSize);

    // Write the result for uint8_t
    image8.pixelMatrix = result8;
    status = writer8.writeImage("output_uint8.pgm", image8);
    if (status != ImageStatus::SUCCESS) {
        cerr << "Failed to write uint8_t image: " << static_cast<int>(status) << endl;
        return 1;
    }
    cout << "uint8_t Image written successfully to output_uint8.pgm" << endl;

    // Apply BoxFilter for uint16_t
    cout << "Applying BoxFilter for uint16_t..." << endl;
    vector<vector<uint16_t>> result16 = BoxFilter(image16.pixelMatrix, kernelSize);

    // Write the result for uint16_t
    image16.pixelMatrix = result16;
    status = writer16.writeImage("output_uint16.pgm", image16);
    if (status != ImageStatus::SUCCESS) {
        cerr << "Failed to write uint16_t image: " << static_cast<int>(status) << endl;
        return 1;
    }
    cout << "uint16_t Image written successfully to output_uint16.pgm" << endl;

    // Apply BoxFilter for uint32_t
    cout << "Applying BoxFilter for uint32_t..." << endl;
    vector<vector<uint32_t>> result32 = BoxFilter(image32.pixelMatrix, kernelSize);

    // Write the result for uint32_t
    image32.pixelMatrix = result32;
    status = writer32.writeImage("output_uint32.pgm", image32);
    if (status != ImageStatus::SUCCESS) {
        cerr << "Failed to write uint32_t image: " << static_cast<int>(status) << endl;
        return 1;
    }
    cout << "uint32_t Image written successfully to output_uint32.pgm" << endl;

    return 0;
}