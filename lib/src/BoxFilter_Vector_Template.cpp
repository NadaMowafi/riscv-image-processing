#include "BoxFilter_Vector_Template.hpp"

// Define vector traits for different data types
template<typename T>
struct VectorTraits;

template<>
struct VectorTraits<uint8_t> {
    using vec_type = vuint8m1_t;
    using wide_type = uint16_t;
    using wide_vec_type = vuint16m2_t;

    static size_t vsetvl(size_t avl) {
        return __riscv_vsetvl_e8m1(avl);
    }

    static vec_type vmv_v_x(uint8_t value, size_t vl) {
        return __riscv_vmv_v_x_u8m1(value, vl);
    }

    static wide_vec_type vmv_v_x_wide(wide_type value, size_t vl) {
        return __riscv_vmv_v_x_u16m2(value, vl);
    }

    static vec_type vle(const uint8_t* ptr, size_t vl) {
        return __riscv_vle8_v_u8m1(ptr, vl);
    }

    static void vse(uint8_t* ptr, vec_type vec, size_t vl) {
        __riscv_vse8_v_u8m1(ptr, vec, vl);
    }

    static wide_vec_type wadd_zero(vec_type a, size_t vl) {
        vec_type zero = vmv_v_x(0, vl);
        return __riscv_vwaddu_vv_u16m2(zero, a, vl);
    }

    static wide_vec_type vadd_vv(wide_vec_type a, wide_vec_type b, size_t vl) {
        return __riscv_vadd_vv_u16m2(a, b, vl);
    }

    static wide_vec_type divu(wide_vec_type a, int divisor, size_t vl) {
        return __riscv_vdivu_vx_u16m2(a, divisor, vl);
    }

    static vec_type vnclipu(wide_vec_type a, unsigned int shift, size_t vl) {
        return __riscv_vnclipu_wx_u8m1(a, shift, 0, vl);
    }
};

template<>
struct VectorTraits<uint16_t> {
    using vec_type = vuint16m1_t;
    using wide_type = uint32_t;
    using wide_vec_type = vuint32m2_t;

    static size_t vsetvl(size_t avl) {
        return __riscv_vsetvl_e16m1(avl);
    }

    static vec_type vmv_v_x(uint16_t value, size_t vl) {
        return __riscv_vmv_v_x_u16m1(value, vl);
    }

    static wide_vec_type vmv_v_x_wide(wide_type value, size_t vl) {
        return __riscv_vmv_v_x_u32m2(value, vl);
    }

    static vec_type vle(const uint16_t* ptr, size_t vl) {
        return __riscv_vle16_v_u16m1(ptr, vl);
    }

    static void vse(uint16_t* ptr, vec_type vec, size_t vl) {
        __riscv_vse16_v_u16m1(ptr, vec, vl);
    }

    static wide_vec_type wadd_zero(vec_type a, size_t vl) {
        vec_type zero = vmv_v_x(0, vl);
        return __riscv_vwaddu_vv_u32m2(zero, a, vl);
    }

    static wide_vec_type vadd_vv(wide_vec_type a, wide_vec_type b, size_t vl) {
        return __riscv_vadd_vv_u32m2(a, b, vl);
    }

    static wide_vec_type divu(wide_vec_type a, int divisor, size_t vl) {
        return __riscv_vdivu_vx_u32m2(a, divisor, vl);
    }

    static vec_type vnclipu(wide_vec_type a, unsigned int shift, size_t vl) {
        return __riscv_vnclipu_wx_u16m1(a, shift, 0, vl);
    }
};

template<>
struct VectorTraits<uint32_t> {
    using vec_type      = vuint32m1_t;
    using wide_type     = uint64_t;
    using wide_vec_type = vuint64m2_t;

    static size_t vsetvl(size_t avl) {
        return __riscv_vsetvl_e32m1(avl);
    }
    static vec_type vmv_v_x(uint32_t v, size_t vl) {
        return __riscv_vmv_v_x_u32m1(v, vl);
    }
    static wide_vec_type vmv_v_x_wide(wide_type v, size_t vl) {
        return __riscv_vmv_v_x_u64m2(v, vl);
    }
    static vec_type vle(const uint32_t* ptr, size_t vl) {
        return __riscv_vle32_v_u32m1(ptr, vl);
    }
    static void vse(uint32_t* ptr, vec_type v, size_t vl) {
        __riscv_vse32_v_u32m1(ptr, v, vl);
    }
    static wide_vec_type wadd_zero(vec_type a, size_t vl) {
        vec_type zero = vmv_v_x(0, vl);
        return __riscv_vwaddu_vv_u64m2(zero, a, vl);
    }
    static wide_vec_type vadd_vv(wide_vec_type a, wide_vec_type b, size_t vl) {
        return __riscv_vadd_vv_u64m2(a, b, vl);
    }
    static wide_vec_type divu(wide_vec_type a, int divisor, size_t vl) {
        return __riscv_vdivu_vx_u64m2(a, divisor, vl);
    }
    static vec_type vnclipu(wide_vec_type a, unsigned shift, size_t vl) {
        return __riscv_vnclipu_wx_u32m1(a, shift, 0, vl);
    }
};



// Templated zeroPad function
template <typename T>
static vector<vector<T>> __riscv_zeroPadImage(const vector<vector<T>>& image, int padSize) {
    if (image.empty() || image[0].empty()) return {};

    int H = image.size(), W = image[0].size();
    int PH = H + 2 * padSize, PW = W + 2 * padSize;
    vector<vector<T>> padded(PH, vector<T>(PW, 0));

    for (int i = 0; i < H; ++i) {
        for (int j = 0; j < W; j += __riscv_vsetvl_e64m1(W - j)) {
            size_t vl = __riscv_vsetvl_e64m1(W - j);
            // load/store based on T
            // solution for 8, 16, 32, 64 bit types that made the compiler happy :)
            // this is a bit of a hack, but it works
            if constexpr (std::is_same_v<T, uint8_t>) {
                vuint8m1_t v = __riscv_vle8_v_u8m1(&image[i][j], vl);
                __riscv_vse8_v_u8m1(&padded[i+padSize][j+padSize], v, vl);
            } else if constexpr (std::is_same_v<T, uint16_t>) {
                vuint16m1_t v = __riscv_vle16_v_u16m1(&image[i][j], vl);
                __riscv_vse16_v_u16m1(&padded[i+padSize][j+padSize], v, vl);
            } else if constexpr (std::is_same_v<T, uint32_t>) {
                vuint32m1_t v = __riscv_vle32_v_u32m1(&image[i][j], vl);
                __riscv_vse32_v_u32m1(&padded[i+padSize][j+padSize], v, vl);
            } else if constexpr (std::is_same_v<T, uint64_t>) {
                vuint64m1_t v = __riscv_vle64_v_u64m1(&image[i][j], vl);
                __riscv_vse64_v_u64m1(&padded[i+padSize][j+padSize], v, vl);
            } else {
                // fallback scalar copy
                for (size_t k = 0; k < vl; ++k)
                    padded[i+padSize][j+padSize+k] = image[i][j+k];
            }
        }
    }
    return padded;
}


// Templated BoxFilter function
template<typename T>
vector<vector<T>> __riscv_BoxFilter(
    const vector<vector<T>> &inputImg,
    int kernelSize)
{
    if (inputImg.empty() || inputImg[0].empty()) {
        throw invalid_argument("Image is empty");
    }

    int rows = inputImg.size();
    int cols = inputImg[0].size();
    int border = kernelSize / 2;

    if (kernelSize > rows || kernelSize > cols || (kernelSize % 2) == 0) {
        throw invalid_argument("Invalid kernel size");
    }

    // Output, padded input, and temporary buffer
    vector<vector<T>> outputImg(rows, vector<T>(cols, 0));
    vector<vector<T>> padded = __riscv_zeroPadImage(inputImg, kernelSize);
    vector<vector<T>> tempImg = padded;

// --- Horizontal pass (1×K) ---
for (int i = 0; i < rows; ++i) {
    int y = i + border;
    int j = 0;
    while (j < cols) {
        // set vl once per chunk
        size_t vl = VectorTraits<T>::vsetvl(cols - j);

        auto zero     = VectorTraits<T>::vmv_v_x(   0, vl);
        auto vsum     = VectorTraits<T>::vmv_v_x_wide(0, vl);

        // accumulate the K horizontal taps
        for (int k = -border; k <= border; ++k) {
            const T* ptr = &padded[y][j + border + k];
            auto v       = VectorTraits<T>::vle(ptr, vl);
            auto vwide   = VectorTraits<T>::wadd_zero(v, vl);
            vsum         = VectorTraits<T>::vadd_vv(vsum, vwide, vl);
        }

        // divide, narrow, store
        auto vavg        = VectorTraits<T>::divu(vsum, kernelSize, vl);
        auto vavg_narrow = VectorTraits<T>::vnclipu(vavg, 0, vl);
        VectorTraits<T>::vse(&tempImg[y][j + border], vavg_narrow, vl);

        j += vl;
    }
}

// --- Vertical pass (K×1) ---
for (int i = 0; i < rows; ++i) {
    int y0 = i + border;
    int j  = 0;
    while (j < cols) {
        // 1) set vl once per chunk
        size_t vl = VectorTraits<T>::vsetvl(cols - j);

        // 2) hoist zero-vector & initial sum
        auto zero = VectorTraits<T>::vmv_v_x(   0, vl);
        auto vsum = VectorTraits<T>::vmv_v_x_wide(0, vl);

        // 3) accumulate the K vertical taps
        for (int k = -border; k <= border; ++k) {
            const T* ptr = &tempImg[y0 + k][j + border];
            auto v       = VectorTraits<T>::vle(ptr, vl);
            auto vwide   = VectorTraits<T>::wadd_zero(v, vl);
            vsum         = VectorTraits<T>::vadd_vv(vsum, vwide, vl);
        }

        // 4) divide, narrow, store
        auto vavg        = VectorTraits<T>::divu(vsum, kernelSize, vl);
        auto vavg_narrow = VectorTraits<T>::vnclipu(vavg, 0, vl);
        VectorTraits<T>::vse(&outputImg[i][j], vavg_narrow, vl);

        j += vl;
    }
}

    return outputImg;
}

template vector<vector<uint8_t>> __riscv_zeroPadImage<uint8_t>(
    const vector<vector<uint8_t>>&,
    int);
template vector<vector<uint8_t>> __riscv_BoxFilter<uint8_t>(
    const vector<vector<uint8_t>>&,
    int);
template vector<vector<uint16_t>> __riscv_zeroPadImage<uint16_t>(
        const vector<vector<uint16_t>>&,
        int);
template vector<vector<uint16_t>> __riscv_BoxFilter<uint16_t>(
        const vector<vector<uint16_t>>&,
        int);
template vector<vector<uint32_t>> __riscv_zeroPadImage<uint32_t>(
    const vector<vector<uint32_t>>&,
    int);
template vector<vector<uint32_t>> __riscv_BoxFilter<uint32_t>(
    const vector<vector<uint32_t>>&,
    int);
