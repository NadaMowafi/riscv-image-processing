#pragma once
#include <riscv_vector.h>
#include <cstdint>
#include <cstddef>
#include <type_traits>
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

// uncomment to use these traits but it was found to be not of any use in the investegated situations
// template<>
// struct VectorTraits<uint32_t> {
//     using vec_type      = vuint32m1_t;
//     using wide_type     = uint64_t;
//     using wide_vec_type = vuint64m2_t;

//     static size_t vsetvl(size_t avl) {
//         return __riscv_vsetvl_e32m1(avl);
//     }
//     static vec_type vmv_v_x(uint32_t v, size_t vl) {
//         return __riscv_vmv_v_x_u32m1(v, vl);
//     }
//     static wide_vec_type vmv_v_x_wide(wide_type v, size_t vl) {
//         return __riscv_vmv_v_x_u64m2(v, vl);
//     }
//     static vec_type vle(const uint32_t* ptr, size_t vl) {
//         return __riscv_vle32_v_u32m1(ptr, vl);
//     }
//     static void vse(uint32_t* ptr, vec_type v, size_t vl) {
//         __riscv_vse32_v_u32m1(ptr, v, vl);
//     }
//     static wide_vec_type wadd_zero(vec_type a, size_t vl) {
//         vec_type zero = vmv_v_x(0, vl);
//         return __riscv_vwaddu_vv_u64m2(zero, a, vl);
//     }
//     static wide_vec_type vadd_vv(wide_vec_type a, wide_vec_type b, size_t vl) {
//         return __riscv_vadd_vv_u64m2(a, b, vl);
//     }
//     static wide_vec_type divu(wide_vec_type a, int divisor, size_t vl) {
//         return __riscv_vdivu_vx_u64m2(a, divisor, vl);
//     }
//     static vec_type vnclipu(wide_vec_type a, unsigned shift, size_t vl) {
//         return __riscv_vnclipu_wx_u32m1(a, shift, 0, vl);
//     }
// };