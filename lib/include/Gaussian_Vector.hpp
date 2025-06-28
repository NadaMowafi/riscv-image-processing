#pragma once

#include <vector>
#include <riscv_vector.h>

template <typename T>
std::vector<std::vector<T>> __riscv_applyGaussianFilterSeparable(
    const std::vector<std::vector<T>>& image,
    int kernelSize,
    double sigma);
    
std::vector<double> __riscv_generateGaussianKernel1D(int N, double sigma);
