#ifndef BOXFILTER_HPP
#define BOXFILTER_HPP

#include <iostream>
#include <vector>
#include <cmath>
#include "FFT.hpp"
#include "Complex.hpp"
#include <cstdint>
using namespace std;

template <typename T = uint8_t>
class BoxFilter
{
public:
    static vector<vector<T>> applyBoxFilterFFT(
        const vector<vector<T>> &image, int kernelSize);
    static vector<vector<T>> applyBoxFilterSlidingGrey(
        const vector<vector<T>> &inputImg, int kernelSize);
    static vector<vector<vector<T>>> applyBoxFilterSlidingRGB(
        const vector<vector<vector<T>>> &inputImg, int kernelSize);
};
#endif // BOXFILTER_HPP
