#include <gtest/gtest.h>
#include "BoxFilter.h"
#include "FFT.h"

TEST(BoxFilterTest, ApplyBoxFilterSuccess) {
    std::vector<std::vector<double>> image = {
        {1, 2, 3},
        {4, 5, 6},
        {7, 8, 9}
    };
    BoxFilter boxFilter;
    int kernelSize = 3;
    std::vector<std::vector<double>> result = boxFilter.applyBoxFilterFFT(image, kernelSize);
    ASSERT_NO_THROW({
        std::vector<std::vector<double>> result = boxFilter.applyBoxFilterFFT(image, kernelSize);        
        ASSERT_FALSE(result.empty());
        ASSERT_EQ(result.size(), image.size());
        ASSERT_EQ(result[0].size(), image[0].size());
        std::cout << "Result at (1,1): " << result[1][1] << std::endl;
        EXPECT_NEAR(result[1][1], 5.0, 0.1);
    });
}

TEST(BoxFilterTest, ApplyBoxFilterInvalidKernel) {
    std::vector<std::vector<double>> image = {
        {1, 2, 3},
        {4, 5, 6},
        {7, 8, 9}
    };

    int kernelSize = 10;

    try {
        BoxFilter boxFilter;
        std::vector<std::vector<double>> result = boxFilter.applyBoxFilterFFT(image, kernelSize);
        FAIL() << "Expected an exception due to invalid kernel size.";
    } catch (const std::exception& e) {
        EXPECT_STREQ(e.what(), "Invalid kernel size");
    } catch (...) {
        FAIL() << "Expected std::exception.";
    }
}

TEST(BoxFilterTest, ApplyBoxFilterEmptyImage) {
    std::vector<std::vector<double>> image;

    int kernelSize = 3;

    try {
        BoxFilter boxFilter;
        std::vector<std::vector<double>> result = boxFilter.applyBoxFilterFFT(image, kernelSize);
        FAIL() << "Expected an exception due to empty image.";
    } catch (const std::exception& e) {
        EXPECT_STREQ(e.what(), "Image is empty");
    } catch (...) {
        FAIL() << "Expected std::exception.";
    }
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
