#include "image_reader.hpp"
#include "image_writer.hpp"
#include "BoxFilter.h"
#include "Gaussian.hpp"
#include "FFT.h"
#include <iostream>
#include <vector>
#include <cstdint>

using namespace std;

int main() {
    ImageReader reader;
    ImageWriter writer;
    Image image;
    BoxFilter BoxFilter;
    int kernelSize = 5;
    // -------------------Apply BoxFilter FFT----------------------- 
    ImageStatus status = reader.readImage("barb.512.pgm", image);
    if (status != ImageStatus::SUCCESS) {
        std::cerr << "Failed to read image: " << static_cast<int>(status) << std::endl;
        return 1;
    }
    std::cout << "Image read successfully. Size: "
              << image.metadata.width << "x"
              << image.metadata.height << std::endl;

    
    std::vector<std::vector<uint8_t>> filteredMatrix = BoxFilter.applyBoxFilterFFT(image.pixelMatrix, kernelSize);
    image.pixelMatrix = filteredMatrix;
    
    status = writer.writeImage("barb.512blur_fft.pgm", image);
    if (status != ImageStatus::SUCCESS) {
        std::cerr << "Failed to write FFT filtered image: " << static_cast<int>(status) << std::endl;
        return 1;
    }
    cout << "FFT Filtered image written successfully." << endl;

    // -------------------Apply BoxFilter Sliding-----------------------
    status = reader.readImage("barb.512.pgm", image);
    if (status != ImageStatus::SUCCESS) {
        std::cerr << "Failed to read image: " << static_cast<int>(status) << std::endl;
        return 1;
    }
    
    vector<vector<uint8_t>> filteredMatrixSliding = BoxFilter.applyBoxFilterSlidingGrey(image.pixelMatrix, kernelSize);
    image.pixelMatrix = filteredMatrixSliding;

    status = writer.writeImage("barb.512blur_sliding.pgm", image);
    if (status != ImageStatus::SUCCESS) {
        std::cerr << "Failed to write Sliding Window filtered image: " << static_cast<int>(status) << std::endl;
        return 1;
    }

    cout << "Sliding Window filtered image written successfully." << std::endl;

    // -------------------Apply Gaussian Filter-----------------------
    status = reader.readImage("barb.512.pgm", image);
    if (status != ImageStatus::SUCCESS) {
        std::cerr << "Failed to read image: " << static_cast<int>(status) << std::endl;
        return 1;
    }
    double sigma = 3;
    vector<vector<double>> gaussianKernel = generateGaussianKernel(kernelSize, sigma);
    vector<vector<uint8_t>> filteredMatrixGaussian = applyGaussianFilter(image.pixelMatrix, gaussianKernel);

    image.pixelMatrix = filteredMatrixGaussian;

    status = writer.writeImage("barb.512blur_gaussian.pgm", image);
    if (status != ImageStatus::SUCCESS) {
        std::cerr << "Failed to write Gaussian filtered image: " << static_cast<int>(status) << std::endl;
        return 1;
    }

    cout << "Gaussian filtered image written successfully." << endl;

    return 0;
}