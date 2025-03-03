#ifndef BOXFILTER_H
#define BOXFILTER_H

#include <iostream>
#include <vector>
#include <cmath>
#include "FFT.h"
#include "Complex.h"
class BoxFilter {
    std::vector<std::vector<double>> applyBoxFilterFFT(
        const std::vector<std::vector<double>>& image, int kernelSize);
};
#endif // BOXFILTER_H