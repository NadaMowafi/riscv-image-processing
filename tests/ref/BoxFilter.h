#ifndef BOXFILTER_H
#define BOXFILTER_H

#include <vector>
#include <cstdint>

using namespace std;

/**
 * Applies a Box Filter to an input image.
 * 
 * @param inputImg: Input image as a 2D vector of pixel values.
 * @param kernelSize: Size of the kernel.
 * @return Output image as a 2D vector of pixel values.
 */
vector<vector<uint8_t>> BoxFilter(const vector<vector<uint8_t>> &inputImg, int kernelSize);

#endif // BOXFILTER_H