#ifndef FFT_HPP
#define FFT_HPP

#include <vector>
#include "Complex.hpp"
#include <cstdint>

using namespace std;

class FFT {
public:
    static void fft(vector<Complex>& x, bool inverse = false);
    static void fft2D(vector<vector<Complex>>& image, bool inverse = false);
    static vector<vector<double>> zeroPad(const vector<vector<double>>& image);
    static vector<vector<uint8_t>> extractOriginalSize(const vector<vector<double>>& paddedResult,int originalRows, int originalCols);
};

#endif // FFT_HPP