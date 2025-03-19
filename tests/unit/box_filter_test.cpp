#include <gtest/gtest.h>
#include "BoxFilter.hpp"
#include "FFT.hpp"
#include <vector>
#include <stdexcept>
#include <cstdint>


using namespace std;


TEST(BoxFilterTest, ApplyBoxFilterSuccess) {
    vector<vector<uint8_t>> image = {
        {1, 2, 3, 4, 5},
        {6, 7, 8, 9, 10},
        {11, 12, 13, 14, 15},
        {16, 17, 18, 19, 20},
        {21, 22, 23, 24, 25}
    };
    vector<vector<uint8_t>> expectedOutput = {
        {2, 3, 4, 4, 3},
        {4, 7, 8, 9, 6},
        {8, 12, 13, 14, 10},
        {11, 17, 18, 19, 13},
        {8, 13, 14, 14, 10}
    };
    BoxFilter boxFilter;
    int kernelSize = 3;
    vector<vector<uint8_t>> result = boxFilter.applyBoxFilterFFT(image, kernelSize);     
        ASSERT_FALSE(result.empty());
        ASSERT_EQ(result.size(), image.size());
        ASSERT_EQ(result[0].size(), image[0].size());
        EXPECT_EQ(result, expectedOutput); 
       
}

TEST(BoxFilterTest, ApplyBoxFilterInvalidKernel) {
    vector<vector<uint8_t>> image = {
        {1, 2, 3},
        {4, 5, 6},
        {7, 8, 9}
    };

    int kernelSize = 10;

    try {
        BoxFilter boxFilter;
        vector<vector<uint8_t>> result = boxFilter.applyBoxFilterFFT(image, kernelSize);
        FAIL() << "Expected an exception due to invalid kernel size.";
    } catch (const exception& e) {
        EXPECT_STREQ(e.what(), "Invalid kernel size");
    } catch (...) {
        FAIL() << "Expected exception.";
    }
}

TEST(BoxFilterTest, ApplyBoxFilterEmptyImage) {
    vector<vector<uint8_t>> image;

    int kernelSize = 3;

    try {
        BoxFilter boxFilter;
        vector<vector<uint8_t>> result = boxFilter.applyBoxFilterFFT(image, kernelSize);
        FAIL() << "Expected an exception due to empty image.";
    } catch (const exception& e) {
        EXPECT_STREQ(e.what(), "Image is empty");
    } catch (...) {
        FAIL() << "Expected exception.";
    }
}


TEST(BoxFilterTest, CorrectOutput) {
    vector<vector<vector<uint8_t>>> inputImg = {
        {{10, 20, 30}, {20, 30, 40}, {30, 40, 50}, {40, 50, 60}, {50, 60, 70}},
        {{15, 25, 35}, {25, 35, 45}, {35, 45, 55}, {45, 55, 65}, {55, 65, 75}},
        {{20, 30, 40}, {30, 40, 50}, {40, 50, 60}, {50, 60, 70}, {60, 70, 80}},
        {{25, 35, 45}, {35, 45, 55}, {45, 55, 65}, {55, 65, 75}, {65, 75, 85}},
        {{30, 40, 50}, {40, 50, 60}, {50, 60, 70}, {60, 70, 80}, {70, 80, 90}}
    };

    int kernelSize = 3;
    vector<vector<vector<uint8_t>>> expectedOutput = {
        {{8, 12, 17}, {15, 22, 28}, {22, 28, 35}, {28, 35, 42}, {21, 26, 30}},
        {{13, 20, 27}, {25, 35, 45}, {35, 45, 55}, {45, 55, 65}, {33, 40, 47}},
        {{17, 23, 30}, {30, 40, 50}, {40, 50, 60}, {50, 60, 70}, {37, 43, 50}},
        {{20, 27, 33}, {35, 45, 55}, {45, 55, 65}, {55, 65, 75}, {40, 47, 53}},
        {{14, 19, 23}, {25, 32, 38}, {32, 38, 45}, {38, 45, 52}, {28, 32, 37}}
    };
    BoxFilter boxFilter;
    vector<vector<vector<uint8_t>>> result = boxFilter.applyBoxFilterSlidingRGB(inputImg, kernelSize);
    EXPECT_EQ(result, expectedOutput);
}

TEST(BoxFilterTest, IncorrectOutput) {
    vector<vector<vector<uint8_t>>> inputImg = {
        {{10, 20, 30}, {20, 30, 40}, {30, 40, 50}, {40, 50, 60}, {50, 60, 70}},
        {{15, 25, 35}, {25, 35, 45}, {35, 45, 55}, {45, 55, 65}, {55, 65, 75}},
        {{20, 30, 40}, {30, 40, 50}, {40, 50, 60}, {50, 60, 70}, {60, 70, 80}},
        {{25, 35, 45}, {35, 45, 55}, {45, 55, 65}, {55, 65, 75}, {65, 75, 85}},
        {{30, 40, 50}, {40, 50, 60}, {50, 60, 70}, {60, 70, 80}, {70, 80, 90}}
    };

    int kernelSize = 3;
    //Changed one value for checking not equal
    vector<vector<vector<uint8_t>>> incorrectOutput = {
        {{8, 12, 17}, {15, 22, 28}, {22, 28, 35}, {28, 35, 42}, {21, 26, 30}},
        {{13, 20, 27}, {25, 35, 45}, {35, 45, 55}, {45, 55, 65}, {33, 40, 47}},
        {{17, 23, 30}, {30, 40, 50}, {40, 20, 60}, {50, 60, 70}, {37, 43, 50}},
        {{20, 27, 33}, {35, 45, 55}, {45, 55, 65}, {55, 65, 75}, {40, 47, 53}},
        {{14, 19, 23}, {25, 32, 38}, {32, 38, 45}, {38, 45, 52}, {28, 32, 37}}
    };
    BoxFilter boxFilter;
    vector<vector<vector<uint8_t>>> result = boxFilter.applyBoxFilterSlidingRGB(inputImg, kernelSize);
    EXPECT_EQ(result, incorrectOutput);
}

TEST(BoxFilterTest, InvalidKernelSize) {
    vector<vector<vector<uint8_t>>> inputImg = {
        {{10, 20, 30}, {20, 30, 40}, {30, 40, 50}, {40, 50, 60}, {50, 60, 70}},
        {{15, 25, 35}, {25, 35, 45}, {35, 45, 55}, {45, 55, 65}, {55, 65, 75}},
        {{20, 30, 40}, {30, 40, 50}, {40, 50, 60}, {50, 60, 70}, {60, 70, 80}},
        {{25, 35, 45}, {35, 45, 55}, {45, 55, 65}, {55, 65, 75}, {65, 75, 85}},
        {{30, 40, 50}, {40, 50, 60}, {50, 60, 70}, {60, 70, 80}, {70, 80, 90}}
    };

    int kernelSize = 11; 
    BoxFilter boxFilter;
    vector<vector<vector<uint8_t>>> result = boxFilter.applyBoxFilterSlidingRGB(inputImg, kernelSize);
}

int main() {
    ::testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}