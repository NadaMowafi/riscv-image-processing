#ifndef FFT_CPP
#define FFT_CPP

#include "FFT.hpp"
#include <cmath>
#include <cstdint>

template class FFT<uint8_t>;
template class FFT<uint16_t>;
template class FFT<uint32_t>;
template class FFT<uint64_t>;

template <typename T>
void FFT<T>::fft(vector<Complex>& x, bool inverse) {
    size_t n = x.size();
    if (n <= 1) return;

    size_t j = 0;
    for (size_t i = 1; i < n; i++) {
        size_t bit = n >> 1;
        while (j >= bit) {
            j -= bit;
            bit >>= 1;
        }
        j += bit;
        if (i < j) swap(x[i], x[j]);
    }

    for (size_t len = 2; len <= n; len *= 2) {
        double angle = 2 * M_PI / len * (inverse ? -1 : 1);
        Complex wn(cos(angle), sin(angle)); 
        for (size_t i = 0; i < n; i += len) {
            Complex w(1, 0);
            for (size_t j = 0; j < len / 2; j++) {
                Complex t = w * x[i + j + len / 2];
                Complex u = x[i + j];
                x[i + j] = u + t;
                x[i + j + len / 2] = u - t;
                w = w * wn; 
            }
        }
    }
    if (inverse) {
        for (size_t i = 0; i < n; i++) {
            x[i] *= (1.0 / n);
        }
    }
}

template <typename T>
void FFT<T>::fft2D(vector<vector<Complex>>& image, bool inverse) {
    int rows = image.size();
    int cols = image[0].size();
    for (int i = 0; i < rows; i++) {
        fft(image[i], inverse);
    }
    for (int j = 0; j < cols; j++) {
        vector<Complex> col(rows);
        for (int i = 0; i < rows; i++) {
            col[i] = image[i][j];
        }
        fft(col, inverse);
        for (int i = 0; i < rows; i++) {
            image[i][j] = col[i];
        }
    }
}

template <typename T>
vector<vector<T>> FFT<T>::extractOriginalSize(const vector<vector<double>>& paddedResult, int originalRows, int originalCols) {
    
    vector<vector<T>> result(originalRows, vector<T>(originalCols, 0));
    
    for (int i = 0; i < originalRows; i++) {
        for (int j = 0; j < originalCols; j++) {
            result[i][j] = static_cast<T>(round(paddedResult[i][j]));
        }
    }
    
    return result;
}

template <typename T>
vector<vector<double>> FFT<T>::zeroPad(const vector<vector<double>>& image) {
    int rows = image.size();
    int cols = image[0].size();
    
    int paddedRows = pow(2, ceil(log2(rows)));
    int paddedCols = pow(2, ceil(log2(cols)));
    
    vector<vector<double>> padded(paddedRows, vector<double>(paddedCols, 0.0));
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            padded[i][j] = image[i][j];
        }
    }

    return padded;
}

#endif // FFT_CPP
