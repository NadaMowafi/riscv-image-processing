#ifndef BOXFILTER_H
#define BOXFILTER_H

#include <iostream>
#include <vector>
#include <cmath>
#include "FFT.h"
#include "Complex.h"
#include <cstdint>
using namespace std;

class BoxFilter {
    public:
    vector<vector<uint8_t>> applyBoxFilterFFT(
            const vector<vector<uint8_t>>& image, int kernelSize);
    vector<vector<uint8_t>> applyBoxFilterSlidingGrey(
            const vector<vector<uint8_t>>& inputImg, int kernelSize);
    vector<std::vector<vector<uint8_t>>> applyBoxFilterSlidingRGB(
            const vector<vector<vector<uint8_t>>>& inputImg, int kernelSize);

};
#endif // BOXFILTER_H