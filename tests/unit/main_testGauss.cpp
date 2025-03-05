#include "image_reader.hpp"
#include "image_writer.hpp"
#include <iostream>
#include <vector>
#include "Gaussian.hpp"

using namespace std;

int main() {
    ImageReader reader;
    ImageWriter writer;
    Image image;

    ImageStatus status = reader.readImage("barb.512.pgm", image);
    if (status != ImageStatus::SUCCESS) {
        std::cerr << "Failed to read image: " << static_cast<int>(status) << std::endl;
        return 1;
    }

    std::cout << "Image read successfully. Size: "
              << image.metadata.width << "x"
              << image.metadata.height << std::endl;

    // Apply BoxFilter
    int kernelSize = 5;
    double sigma = 5;
   vector<vector<double>> KGaussian = generateGaussianKernel(kernelSize, sigma);
    vector<vector<uint8_t>> filteredMatrix = applyGaussianFilter(image.pixelMatrix, KGaussian);
    // Update the pixelMatrix with the filteredMatrix
    image.pixelMatrix = filteredMatrix;

    status = writer.writeImage("barb.512blurGaussian.pgm", image);
    if (status != ImageStatus::SUCCESS) {
        std::cerr << "Failed to write image: " << static_cast<int>(status) << std::endl;
        return 1;
    }

    std::cout << "Image written successfully." << std::endl;
    return 0;
}