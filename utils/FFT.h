#ifndef FFT_H
#define FFT_H

#include <vector>
#include "Complex.h"

using namespace std;

class FFT {
public:
    static void fft(vector<Complex>& x, bool inverse = false);
    static void fft2D(vector<vector<Complex>>& image, bool inverse = false);
    static vector<vector<double>> zeroPad(const vector<vector<double>>& image);
    static vector<vector<double>> extractOriginalSize(const vector<vector<double>>& paddedResult,int originalRows, int originalCols);
};

#endif // FFT_H