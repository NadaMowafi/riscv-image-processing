#include <gtest/gtest.h>
#include "ImageReader.hpp"
#include "ImageWriter.hpp"
#include "Image.hpp"
#include <vector>
#include <string>
#include "Add.hpp"

using namespace std;

TEST(ImageComparisonTest, ADDSaturationTest) {
    ImageReader reader;
    ImageWriter writer; 
    Image image1;
    Image image2;
    ImageStatus status1 = reader.readImage("barb.512.pgm", image1);
    ASSERT_EQ(status1, ImageStatus::SUCCESS) << "Failed to read image1";
    ImageStatus status2= reader.readImage("3things256.pgm", image2);
    ASSERT_EQ(status2, ImageStatus::SUCCESS) << "Failed to read image2";

    vector<vector<uint8_t>> output = add(image1.pixelMatrix, image2.pixelMatrix, CLIPPING::SATURATION);

    Image addedImage;
    addedImage.metadata = image1.metadata;
    addedImage.pixelMatrix = output;
    addedImage.pixelData = image1.pixelData;
    status1 = writer.writeImage("addedImage.pgm", addedImage);
    ASSERT_EQ(status1, ImageStatus::SUCCESS) << "Failed to write added added image";

    Image matlabImage;
    ImageStatus status3 = reader.readImage("matlabImage.pgm", matlabImage);    
    ASSERT_EQ(status3, ImageStatus::SUCCESS) << "Failed to read matlab image";
  
    EXPECT_EQ(addedImage.pixelMatrix, matlabImage.pixelMatrix);


}

TEST(ImageComparisonTest, ADDWeightedSaturationTest) {
    ImageReader reader;
    ImageWriter writer; 
    Image image1;
    Image image2;
    ImageStatus status1 = reader.readImage("barb.512.pgm", image1);
    ASSERT_EQ(status1, ImageStatus::SUCCESS) << "Failed to read image1";
    ImageStatus status2= reader.readImage("3things256.pgm", image2);
    ASSERT_EQ(status2, ImageStatus::SUCCESS) << "Failed to read image2";

    vector<vector<uint8_t>> output = addWeighted(image1.pixelMatrix,0.6, image2.pixelMatrix,0.4,50,CLIPPING::SATURATION);

    Image addedImage;
    addedImage.metadata = image1.metadata;
    addedImage.pixelMatrix = output;
    addedImage.pixelData = image1.pixelData;
    status1 = writer.writeImage("addedWeightedImage.pgm", addedImage);
    ASSERT_EQ(status1, ImageStatus::SUCCESS) << "Failed to write added added image";

    Image matlabImage;
    ImageStatus status3 = reader.readImage("MatlabAddedWeightedImage.pgm", matlabImage);    
    ASSERT_EQ(status3, ImageStatus::SUCCESS) << "Failed to read matlab image";
  
    EXPECT_EQ(addedImage.pixelMatrix, matlabImage.pixelMatrix);

}


int main() {
    ::testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}