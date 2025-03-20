#ifndef BOXFILTER_HPP
#define BOXFILTER_HPP

#include <iostream>
#include <vector>
#include <cmath>
#include "FFT.hpp"
#include "Complex.hpp"
#include <cstdint>
using namespace std;

class BoxFilter {
    public:
    vector<vector<uint8_t>> applyBoxFilterFFT(
            const vector<vector<uint8_t>>& image, int kernelSize);
    vector<vector<uint8_t>> applyBoxFilterSlidingGrey(
            const vector<vector<uint8_t>>& inputImg, int kernelSize);
    vector<vector<vector<uint8_t>>> applyBoxFilterSlidingRGB(
            const vector<vector<vector<uint8_t>>>& inputImg, int kernelSize);

};
#endif // BOXFILTER_HPP