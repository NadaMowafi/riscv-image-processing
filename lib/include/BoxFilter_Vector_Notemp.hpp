#ifndef BOXFILTER_VECTOR_NOTEMP_HPP
#define BOXFILTER_VECTOR_NOTEMP_HPP

#include <vector>
#include <stdexcept>
#include <cstdint>
#include <iostream>
#include <riscv_vector.h>

using namespace std;

// Function to apply a box filter on a 2D image for uint8_t
vector<vector<uint8_t>> BoxFilter(
    const vector<vector<uint8_t>> &inputImg,
    int kernelSize);

// Function to apply a box filter on a 2D image for uint16_t
vector<vector<uint16_t>> BoxFilter(
    const vector<vector<uint16_t>> &inputImg,
    int kernelSize);

// Function to apply a box filter on a 2D image for uint32_t
vector<vector<uint32_t>> BoxFilter(
    const vector<vector<uint32_t>> &inputImg,
    int kernelSize);

// Function to pad the input image with zeros for uint8_t
vector<vector<uint8_t>> zeroPad(
    const vector<vector<uint8_t>> &inputImg,
    int kernelSize);

// Function to pad the input image with zeros for uint16_t
vector<vector<uint16_t>> zeroPad(
    const vector<vector<uint16_t>> &inputImg,
    int kernelSize);

// Function to pad the input image with zeros for uint32_t
vector<vector<uint32_t>> zeroPad(
    const vector<vector<uint32_t>> &inputImg,
    int kernelSize);

#endif // BOXFILTER_VECTOR_NOTEMP_HPP