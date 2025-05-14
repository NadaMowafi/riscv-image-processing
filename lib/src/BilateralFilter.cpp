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

vector<double> computeRangeLUT(double sigmaRange) {
    vector<double> rangeLUT(256);
    double twoSigmaRange2 = 2.0 * sigmaRange * sigmaRange;

    int i = 0;
    while (i < 256) {
        size_t vl = __riscv_vsetvl_e64m4(256 - i);
        vuint64m4_t v_idx = __riscv_vid_v_u64m4(vl); // Generate indices
        v_idx = __riscv_vadd_vx_u64m4(v_idx, i, vl); // Offset indices by i
        vfloat64m4_t v_idx_f = __riscv_vfcvt_f_x_v_f64m4(__riscv_vreinterpret_v_u64m4_i64m4(v_idx), vl);
        vfloat64m4_t v_idx2 = __riscv_vfmul_vv_f64m4(v_idx_f, v_idx_f, vl); // Square the indices
        vfloat64m4_t v_arg = __riscv_vfdiv_vf_f64m4(v_idx2, twoSigmaRange2, vl); // Divide by 2 * sigma^2
        __riscv_vse64_v_f64m4(&rangeLUT[i], v_arg, vl); // Store the intermediate results in rangeLUT
        i += vl;
    }

    // Perform scalar exp(-x) on the final results
    for (int i = 0; i < 256; ++i) {
        rangeLUT[i] = std::exp(-rangeLUT[i]);
    }

    return rangeLUT;
}

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

    vector<double> rangeLUT = computeRangeLUT(sigmaRange);

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

                    // Load pixel values
                    vuint8m2_t v_pix = __riscv_vle8_v_u8m2(&padded[r][c], vl);
                    vuint16m4_t v_pix_u16 = __riscv_vzext_vf2_u16m4(v_pix, vl); // Zero-extend to 16 bits
                    vuint32m4_t v_pix_u32 = __riscv_vreinterpret_v_u16m4_u32m4(v_pix_u16);


                    // // Convert to float64
                    vfloat32m4_t v_padded_f32 = __riscv_vfcvt_f_xu_v_f32m4(v_pix_u32, vl);
                    vfloat64m8_t v_padded = __riscv_vfwcvt_f_f_v_f64m8(v_padded_f32, vl);

                    // Convert to signed 16-bit integer
                    vint16m4_t v_pix_i16_signed = __riscv_vreinterpret_v_u16m4_i16m4(v_pix_u16);
                    vint16m4_t v_center = __riscv_vmv_v_x_i16m4(center, vl);

                    // Compute absolute differences
                    vint16m4_t v_diff = __riscv_vsub_vv_i16m4(v_pix_i16_signed, v_center, vl);
                    vbool4_t mask = __riscv_vmslt_vx_i16m4_b4(v_diff, 0, vl);
                    v_diff = __riscv_vneg_v_i16m4_m(mask, v_diff, vl);

                    // Map differences to range LUT
                    vuint16m4_t v_diff_u16 = __riscv_vreinterpret_v_i16m4_u16m4(v_diff);
                    vuint32m8_t v_idx = __riscv_vzext_vf2_u32m8(v_diff_u16, vl);

                    // Load range LUT values
                    vfloat64m8_t v_range = __riscv_vundefined_f64m8();
                    for (size_t x = 0; x < vl; ++x) {
                        uint32_t idx = reinterpret_cast<uint32_t*>(&v_idx)[x];
                        reinterpret_cast<double*>(&v_range)[x] = rangeLUT[idx];
                    }

                    // Compute spatial kernel values
                    vfloat64m8_t v_spatial = __riscv_vundefined_f64m8();
                    for (size_t x = 0; x < vl; ++x) {
                        reinterpret_cast<double*>(&v_spatial)[x] = spatialKernel[ki] * spatialKernel[kj + x];
                    }

                    // Compute weights
                    vfloat64m8_t v_weight = __riscv_vfmul_vv_f64m8(v_spatial, v_range, vl);
                    vfloat64m8_t v_weighted_padded = __riscv_vfmul_vv_f64m8(v_weight, v_padded, vl);
                    // Initialize accumulators for reduction
                    vfloat64m1_t v_acc_norm = __riscv_vfmv_s_f_f64m1(0.0, vl); // Initialize accumulator for norm
                    vfloat64m1_t v_acc_result = __riscv_vfmv_s_f_f64m1(0.0, vl); // Initialize accumulator for result
                    // Perform reduction
                    v_acc_norm = __riscv_vfredusum_vs_f64m8_f64m1(v_weight, v_acc_norm, vl);
                    v_acc_result = __riscv_vfredusum_vs_f64m8_f64m1(v_weighted_padded, v_acc_result, vl);
            
                    // Extract scalar values from accumulators
                    norm += __riscv_vfmv_f_s_f64m1_f64(v_acc_norm);
                    result += __riscv_vfmv_f_s_f64m1_f64(v_acc_result);
                                        

                    kj += vl;
                }
            }

            output[i][j] = static_cast<uint8_t>(std::round(result / norm));
        }
    }
    return output;
}