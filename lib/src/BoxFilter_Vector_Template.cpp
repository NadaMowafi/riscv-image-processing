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
        // narrow from 64→32 bits
        return __riscv_vnclipu_wx_u32m1(a, shift, 0, vl);
    }
};


// Templated zeroPad function
template<typename T>
vector<vector<T>> zeroPad(
    const vector<vector<T>> &inputImg,
    int kernelSize)
{
    int rows = inputImg.size();
    int cols = inputImg[0].size();
    int border = kernelSize / 2;

    vector<vector<T>> padded(
        rows + 2 * border,
        vector<T>(cols + 2 * border, 0)
    );

    for (int i = 0; i < rows; ++i) {
        int j = 0;
        while (j < cols) {
            size_t vl = VectorTraits<T>::vsetvl(cols - j);
            typename VectorTraits<T>::vec_type vec = VectorTraits<T>::vle(&inputImg[i][j], vl);
            VectorTraits<T>::vse(&padded[i + border][j + border], vec, vl);
            j += vl;
        }
    }

    return padded;
}

// Templated BoxFilter function
template<typename T>
vector<vector<T>> BoxFilter(
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
    vector<vector<T>> padded = zeroPad(inputImg, kernelSize);
    vector<vector<T>> tempImg = padded;

    // --- Horizontal pass ---
    for (int i = 0; i < rows; ++i) {
        int y = i + border;
        int j = 0;
        while (j < cols) {
            size_t vl = VectorTraits<T>::vsetvl(cols - j);
            typename VectorTraits<T>::vec_type zero = VectorTraits<T>::vmv_v_x(0, vl);
            typename VectorTraits<T>::wide_vec_type vsum = VectorTraits<T>::vmv_v_x_wide(0, vl);

            for (int k = -border; k <= border; ++k) {
                const T* ptr = &padded[y][j + border + k];
                typename VectorTraits<T>::vec_type v = VectorTraits<T>::vle(ptr, vl);
                typename VectorTraits<T>::wide_vec_type v_wide = VectorTraits<T>::wadd_zero(v, vl);
                vsum = VectorTraits<T>::vadd_vv(vsum, v_wide, vl);
            }

            typename VectorTraits<T>::wide_vec_type vavg = VectorTraits<T>::divu(vsum, kernelSize, vl);
            typename VectorTraits<T>::vec_type vavg_narrow = VectorTraits<T>::vnclipu(vavg, 0, vl);
            VectorTraits<T>::vse(&tempImg[y][j + border], vavg_narrow, vl);

            j += vl;
        }
    }

    // --- Vertical pass ---
    for (int i = 0; i < rows; ++i) {
        int y0 = i + border;
        int j = 0;
        while (j < cols) {
            size_t vl = VectorTraits<T>::vsetvl(cols - j);
            typename VectorTraits<T>::vec_type zero = VectorTraits<T>::vmv_v_x(0, vl);
            typename VectorTraits<T>::wide_vec_type vsum = VectorTraits<T>::vmv_v_x_wide(0, vl);

            for (int k = -border; k <= border; ++k) {
                const T* ptr = &tempImg[y0 + k][j + border];
                typename VectorTraits<T>::vec_type v = VectorTraits<T>::vle(ptr, vl);
                typename VectorTraits<T>::wide_vec_type v_wide = VectorTraits<T>::wadd_zero(v, vl);
                vsum = VectorTraits<T>::vadd_vv(vsum, v_wide, vl);
            }

            typename VectorTraits<T>::wide_vec_type vavg = VectorTraits<T>::divu(vsum, kernelSize, vl);
            typename VectorTraits<T>::vec_type vavg_narrow = VectorTraits<T>::vnclipu(vavg, 0, vl);
            VectorTraits<T>::vse(&outputImg[i][j], vavg_narrow, vl);

            j += vl;
        }
    }

    return outputImg;
}

template vector<vector<uint8_t>> zeroPad<uint8_t>(
    const vector<vector<uint8_t>>&,
    int);
template vector<vector<uint8_t>> BoxFilter<uint8_t>(
    const vector<vector<uint8_t>>&,
    int);
template vector<vector<uint16_t>> zeroPad<uint16_t>(
        const vector<vector<uint16_t>>&,
        int);
template vector<vector<uint16_t>> BoxFilter<uint16_t>(
        const vector<vector<uint16_t>>&,
        int);
template vector<vector<uint32_t>> zeroPad<uint32_t>(
    const vector<vector<uint32_t>>&,
    int);
template vector<vector<uint32_t>> BoxFilter<uint32_t>(
    const vector<vector<uint32_t>>&,
    int);