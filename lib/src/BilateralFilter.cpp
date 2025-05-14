#include <cmath>
#include <cstdint>
#include <vector>
#include <type_traits>
#include <riscv_vector.h>
#include "BilateralFilter.hpp"  


using std::vector;

// Generate normalized 1D Gaussian kernel using RVV
std::vector<double> __riscv_generateGaussianKernel1D(int N, double sigma) {
    if ((N & 1) == 0) ++N;
    int half = N >> 1;
    double twoSig2 = 2.0 * sigma * sigma;

    std::vector<double> arg(N), kernel(N);

    int i = 0;
    while (i < N) {
        size_t vl = __riscv_vsetvl_e64m4(N - i);
        vuint64m4_t vidx = __riscv_vid_v_u64m4(vl);
        vidx = __riscv_vadd_vx_u64m4(vidx, (uint64_t)i, vl);
        vfloat64m4_t v_i = __riscv_vfcvt_f_x_v_f64m4(__riscv_vreinterpret_v_u64m4_i64m4(vidx), vl);
        vfloat64m4_t v_x = __riscv_vfsub_vf_f64m4(v_i, (double)half, vl);
        vfloat64m4_t v_x2 = __riscv_vfmul_vv_f64m4(v_x, v_x, vl);
        vfloat64m4_t v_arg = __riscv_vfdiv_vf_f64m4(v_x2, twoSig2, vl);
        __riscv_vse64_v_f64m4(&arg[i], v_arg, vl);
        i += vl;
    }

    double sum = 0.0;
    for (int j = 0; j < N; ++j) {
        kernel[j] = std::exp(-arg[j]);
        sum += kernel[j];
    }

    i = 0;
    while (i < N) {
        size_t vl = __riscv_vsetvl_e64m4(N - i);
        vfloat64m4_t v_kernel = __riscv_vle64_v_f64m4(&kernel[i], vl);
        v_kernel = __riscv_vfdiv_vf_f64m4(v_kernel, sum, vl);
        __riscv_vse64_v_f64m4(&kernel[i], v_kernel, vl);
        i += vl;
    }

    return kernel;
}

// Pad image using RVV
template <typename T>
static vector<vector<T>> __riscv_zeroPadImage(const vector<vector<T>>& image, int padSize) {
    if (image.empty() || image[0].empty()) return {};

    int H = image.size(), W = image[0].size();
    int PH = H + 2 * padSize, PW = W + 2 * padSize;
    vector<vector<T>> padded(PH, vector<T>(PW, 0));

    for (int i = 0; i < H; ++i) {
        for (int j = 0; j < W; j += __riscv_vsetvl_e64m4(W - j)) {
            size_t vl = __riscv_vsetvl_e64m4(W - j);
            if constexpr (std::is_same_v<T, uint8_t>) {
                vuint8m4_t v = __riscv_vle8_v_u8m4(&image[i][j], vl);
                __riscv_vse8_v_u8m4(&padded[i + padSize][j + padSize], v, vl);
            } else if constexpr (std::is_same_v<T, uint16_t>) {
                vuint16m4_t v = __riscv_vle16_v_u16m4(&image[i][j], vl);
                __riscv_vse16_v_u16m4(&padded[i + padSize][j + padSize], v, vl);
            } else if constexpr (std::is_same_v<T, uint32_t>) {
                vuint32m4_t v = __riscv_vle32_v_u32m4(&image[i][j], vl);
                __riscv_vse32_v_u32m4(&padded[i + padSize][j + padSize], v, vl);
            } else if constexpr (std::is_same_v<T, uint64_t>) {
                vuint64m4_t v = __riscv_vle64_v_u64m4(&image[i][j], vl);
                __riscv_vse64_v_u64m4(&padded[i + padSize][j + padSize], v, vl);
            } else {
                for (size_t k = 0; k < vl; ++k)
                    padded[i + padSize][j + padSize + k] = image[i][j + k];
            }
        }
    }

    return padded;
}

// Bilateral Filter using RVV for grayscale uint8_t images
vector<vector<uint8_t>> bilateralFilterRVV(
    const vector<vector<uint8_t>>& image,
    int kernelSize,
    double sigmaSpatial,
    double sigmaRange
) {
    if (image.empty() || image[0].empty()) return {};

    int H = image.size(), W = image[0].size();
    int halfK = kernelSize / 2;

    vector<double> spatialKernel = __riscv_generateGaussianKernel1D(kernelSize, sigmaSpatial);
    vector<vector<uint8_t>> padded = __riscv_zeroPadImage(image, halfK);
    vector<vector<uint8_t>> output(H, vector<uint8_t>(W, 0));

    vector<double> rangeLUT(256);
    double twoSigmaRange2 = 2.0 * sigmaRange * sigmaRange;
    for (int i = 0; i < 256; ++i)
        rangeLUT[i] = std::exp(- (i * i) / twoSigmaRange2);

    for (int i = 0; i < H; ++i) {
        for (int j = 0; j < W; ++j) {
            double norm = 0.0, result = 0.0;
            uint8_t center = padded[i + halfK][j + halfK];

            for (int ki = 0; ki < kernelSize; ++ki) {
                int r = i + ki;
                int kj = 0;

                while (kj < kernelSize) {
                    size_t vl = __riscv_vsetvl_e8m4(kernelSize - kj);
                    int c = j + kj;

                    
                    vuint8m2_t v_pix = __riscv_vle8_v_u8m2(&padded[r][c], vl); 
                    vuint16m4_t v_pix_i16 = __riscv_vzext_vf2_u16m4(v_pix, vl);
                    vint16m4_t v_pix_i16_signed = __riscv_vreinterpret_v_u16m4_i16m4(v_pix_i16); 
                    vint16m4_t v_center = __riscv_vmv_v_x_i16m4(center, vl);
                    vint16m4_t v_diff = __riscv_vsub_vv_i16m4(v_pix_i16_signed, v_center, vl);
                    vbool4_t mask = __riscv_vmslt_vx_i16m4_b4(v_diff, 0, vl);
                    v_diff = __riscv_vneg_v_i16m4_m(mask, v_diff, vl); 

                    vuint16m4_t v_diff_u16 = __riscv_vreinterpret_v_i16m4_u16m4(v_diff);
                    vuint32m8_t v_idx = __riscv_vzext_vf2_u32m8(v_diff_u16, vl);
                    vint32m8_t v_idx_signed = __riscv_vreinterpret_v_u32m8_i32m8(v_idx); 



                    alignas(64) uint32_t indices[vl];
                    __riscv_vse32_v_i32m8(reinterpret_cast<int*>(indices), v_idx_signed, vl);

                    alignas(64) double v_range[vl];
                    for (size_t x = 0; x < vl; ++x)
                        v_range[x] = rangeLUT[indices[x]];

                    alignas(64) double v_spatial[vl];
                    for (size_t x = 0; x < vl; ++x)
                        v_spatial[x] = spatialKernel[ki] * spatialKernel[kj + x];

                    for (size_t x = 0; x < vl; ++x) {
                        double weight = v_spatial[x] * v_range[x];
                        norm += weight;
                        result += weight * padded[r][c + x];
                    }

                    kj += vl;
                }
            }

            output[i][j] = static_cast<uint8_t>(std::round(result / norm));
        }
    }

    return output;
}