#include "FFT.h"
#include <cmath>

using namespace std;

void FFT::fft(vector<Complex>& x, bool inverse) {
    size_t n = x.size();
    if (n <= 1) return;
    
    vector<Complex> even(n / 2), odd(n / 2);
    for (size_t i = 0; i < n / 2; i++) {
        even[i] = x[i * 2];
        odd[i] = x[i * 2 + 1];
    }
    fft(even, inverse);
    fft(odd, inverse);
    double angleFactor = 2.0 * M_PI / n * (inverse ? -1.0 : 1.0);
    Complex w(1.0, 0.0);
    Complex wn(cos(angleFactor), sin(angleFactor));
    for (size_t k = 0; k < n / 2; k++) {
        Complex t = w * odd[k];
        x[k] = even[k] + t;
        x[k + n / 2] = even[k] - t;
        w = w * wn;
    }
    if (inverse) {
        for (size_t i = 0; i < n; i++) {
            x[i] *= 1.0 / n;
        }
    }
}

void FFT::fft2D(vector<vector<Complex>>& image, bool inverse) {
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

vector<vector<double>> extractOriginalSize(
    const vector<vector<double>>& paddedResult,
    int originalRows, int originalCols) {
    
    vector<vector<double>> result(originalRows, vector<double>(originalCols, 0.0));
    
    for (int i = 0; i < originalRows; i++) {
        for (int j = 0; j < originalCols; j++) {
            result[i][j] = paddedResult[i][j];
        }
    }
    
    return result;
}

vector<vector<double>> zeroPad(const vector<vector<double>>& image) {
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
