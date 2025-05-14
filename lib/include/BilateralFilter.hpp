#ifndef BILATERAL_FILTER_HPP
#define BILATERAL_FILTER_HPP

#include <vector>
#include <cstdint>
#include <riscv_vector.h>
#include <cmath>

std::vector<std::vector<uint8_t>> bilateralFilterRVV(
    const std::vector<std::vector<uint8_t>>& image,
    int kernelSize,
    double sigmaSpatial,
    double sigmaRange
);

// Optional utilities if needed elsewhere
std::vector<double> __riscv_generateGaussianKernel1D(int N, double sigma);

template <typename T>
std::vector<std::vector<T>> __riscv_zeroPadImage(
    const std::vector<std::vector<T>>& image,
    int padSize
);

#endif // BILATERAL_FILTER_RVV_HPP