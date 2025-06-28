#include "Gaussian_Vector.hpp"
#include <cmath>
#include <type_traits>

std::vector<double> __riscv_generateGaussianKernel1D(int N, double sigma) {
    if ((N & 1) == 0) ++N;
    int half       = N >> 1;
    double twoSig2 = 2.0 * sigma * sigma;

    std::vector<double> arg(N), kernel(N);

    int i = 0;
    while (i < N) {
        size_t vl = __riscv_vsetvl_e64m4(N - i);
        vuint64m4_t vidx = __riscv_vid_v_u64m4(vl);
        vidx = __riscv_vadd_vx_u64m4(vidx, (uint64_t)i, vl);
        vfloat64m4_t v_i = __riscv_vfcvt_f_x_v_f64m4(__riscv_vreinterpret_v_u64m4_i64m4(vidx), vl);
        vfloat64m4_t v_x  = __riscv_vfsub_vf_f64m4(v_i, (double)half, vl);
        vfloat64m4_t v_x2 = __riscv_vfmul_vv_f64m4(v_x, v_x, vl);
        vfloat64m4_t v_arg= __riscv_vfdiv_vf_f64m4(v_x2, twoSig2, vl);
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

template <typename T>
static std::vector<std::vector<T>> __riscv_zeroPadImage(const std::vector<std::vector<T>>& image, int padSize) {
    if (image.empty() || image[0].empty()) return {};

    int H = image.size(), W = image[0].size();
    int PH = H + 2 * padSize, PW = W + 2 * padSize;
    std::vector<std::vector<T>> padded(PH, std::vector<T>(PW, 0));

    for (int i = 0; i < H; ++i) {
        for (int j = 0; j < W; j += __riscv_vsetvl_e64m4(W - j)) {
            size_t vl = __riscv_vsetvl_e64m4(W - j);
            if constexpr (std::is_same_v<T, uint8_t>) {
                vuint8m4_t v = __riscv_vle8_v_u8m4(&image[i][j], vl);
                __riscv_vse8_v_u8m4(&padded[i+padSize][j+padSize], v, vl);
            } else if constexpr (std::is_same_v<T, uint16_t>) {
                vuint16m4_t v = __riscv_vle16_v_u16m4(&image[i][j], vl);
                __riscv_vse16_v_u16m4(&padded[i+padSize][j+padSize], v, vl);
            } else if constexpr (std::is_same_v<T, uint32_t>) {
                vuint32m4_t v = __riscv_vle32_v_u32m4(&image[i][j], vl);
                __riscv_vse32_v_u32m4(&padded[i+padSize][j+padSize], v, vl);
            } else if constexpr (std::is_same_v<T, uint64_t>) {
                vuint64m4_t v = __riscv_vle64_v_u64m4(&image[i][j], vl);
                __riscv_vse64_v_u64m4(&padded[i+padSize][j+padSize], v, vl);
            } else {
                for (size_t k = 0; k < vl; ++k)
                    padded[i+padSize][j+padSize+k] = image[i][j+k];
            }
        }
    }
    return padded;
}

template <typename T>
std::vector<std::vector<T>> __riscv_applyGaussianFilterSeparable(
    const std::vector<std::vector<T>>& image,
    int kernelSize,
    double sigma)
{
    if (image.empty() || image[0].empty()) return {};

    int H = image.size(), W = image[0].size();
    if (kernelSize <= 0) kernelSize = (static_cast<int>(std::ceil(sigma * 6)) | 1);
    int half = kernelSize / 2;

    auto kernel1D = __riscv_generateGaussianKernel1D(kernelSize, sigma);
    auto padded = __riscv_zeroPadImage(image, half);
    int PH = padded.size(), PW = padded[0].size();

    std::vector<std::vector<double>> interm(PH, std::vector<double>(PW, 0.0));
    for (int i = 0; i < PH; ++i) {
        for (int j = half; j < PW - half; j += __riscv_vsetvl_e64m4(PW - half - j)) {
            size_t vl = __riscv_vsetvl_e64m4(PW - half - j);
            vfloat64m4_t vsum = __riscv_vfmv_v_f_f64m4(0.0, vl);

            for (int k = 0; k < kernelSize; ++k) {
                int col = j - half + k;
                alignas(8) double tmp[vl];
                for (size_t l = 0; l < vl; ++l)
                    tmp[l] = static_cast<double>(padded[i][col + l]);
                vfloat64m4_t vimg = __riscv_vle64_v_f64m4(tmp, vl);
                vsum = __riscv_vfadd_vv_f64m4(
                    vsum,
                    __riscv_vfmul_vf_f64m4(vimg, kernel1D[k], vl),
                    vl
                );
            }
            alignas(8) double out_tmp[vl];
            __riscv_vse64_v_f64m4(out_tmp, vsum, vl);
            for (size_t l = 0; l < vl; ++l)
                interm[i][j + l] = out_tmp[l];
        }
    }

    std::vector<std::vector<T>> output(H, std::vector<T>(W, 0));
    for (int j = half; j < PW - half; ++j) {
        for (int i = half; i < PH - half; i += __riscv_vsetvl_e64m4(PH - half - i)) {
            size_t vl = __riscv_vsetvl_e64m4(PH - half - i);
            vfloat64m4_t vsum = __riscv_vfmv_v_f_f64m4(0.0, vl);

            for (int k = 0; k < kernelSize; ++k) {
                int row = i - half + k;
                alignas(8) double tmp[vl];
                for (size_t l = 0; l < vl; ++l)
                    tmp[l] = interm[row + l][j];
                vfloat64m4_t vimg = __riscv_vle64_v_f64m4(tmp, vl);
                vsum = __riscv_vfadd_vv_f64m4(
                    vsum,
                    __riscv_vfmul_vf_f64m4(vimg, kernel1D[k], vl),
                    vl
                );
            }
            alignas(8) double out_tmp[vl];
            __riscv_vse64_v_f64m4(out_tmp, vsum, vl);
            for (size_t l = 0; l < vl && (i - half + l) < H; ++l)
                output[i - half + l][j - half] = static_cast<T>(out_tmp[l]);
        }
    }
    return output;
}

// Explicit instantiation for uint8_t and uint16_t if needed:
template std::vector<std::vector<uint8_t>> __riscv_applyGaussianFilterSeparable(
    const std::vector<std::vector<uint8_t>>&, int, double);
template std::vector<std::vector<uint16_t>> __riscv_applyGaussianFilterSeparable(
    const std::vector<std::vector<uint16_t>>&, int, double);
