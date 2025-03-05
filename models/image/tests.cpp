#include <gtest/gtest.h>
#include "image_reader.hpp"
#include "image_writer.hpp"

// Test reading a valid PGM image
TEST(ImageProcessingTest, ReadValidImage) {
    ImageReader reader;
    Image image;
    ImageStatus status = reader.readImage("3things.256.pgm", image);
    EXPECT_EQ(status, ImageStatus::SUCCESS);
    EXPECT_EQ(image.metadata.width, 256);
    EXPECT_EQ(image.metadata.height, 256);
    EXPECT_EQ(image.metadata.maxValue, 255);
    EXPECT_FALSE(image.pixelData.empty());
    EXPECT_EQ(image.pixelData.size(), static_cast<size_t>(256 * 256));
}

// Test writing an image and then reading it back
TEST(ImageProcessingTest, WriteAndReadImage) {
    ImageReader reader;
    ImageWriter writer;
    Image image;
    // Read the valid image
    ImageStatus status = reader.readImage("3things.256.pgm", image);
    EXPECT_EQ(status, ImageStatus::SUCCESS);

    // Write the image to a new file
    status = writer.writeImage("output_test.pgm", image);
    EXPECT_EQ(status, ImageStatus::SUCCESS);

    // Read back the written file
    Image image2;
    status = reader.readImage("output_test.pgm", image2);
    EXPECT_EQ(status, ImageStatus::SUCCESS);
    EXPECT_EQ(image2.metadata.width, image.metadata.width);
    EXPECT_EQ(image2.metadata.height, image.metadata.height);
    EXPECT_EQ(image2.metadata.maxValue, image.metadata.maxValue);
    EXPECT_EQ(image2.pixelData, image.pixelData);
}

// Test behavior when trying to read a non-existent file
TEST(ImageProcessingTest, ReadNonexistentFile) {
    ImageReader reader;
    Image image;
    ImageStatus status = reader.readImage("nonexistent.pgm", image);
    EXPECT_EQ(status, ImageStatus::FILE_NOT_FOUND);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
