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
vector<vector<vector<int>>> BoxFilter(const vector<vector<vector<int>>> &inputImg, int kernelSize);

#endif // BOXFILTER_H