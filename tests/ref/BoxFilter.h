#ifndef BOXFILTER_H
#define BOXFILTER_H

#include <vector> 

using namespace std; 

/**
 * Applies a Box Filter to an input image.
 * 
 * @param inputImg: Input image as a 3D vector of pixel values.
 * @param outputImg: Output image as a 3D vector of pixel values.
 * @param kernelSize: Size of the kernel .
 */
void BoxFilter(const std::vector<std::vector<std::vector<int>>>& inputImg, std::vector<std::vector<std::vector<int>>>& outputImg, int kernelSize);

#endif // BOXFILTER_H