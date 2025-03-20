#ifndef FLIPPING_HPP
#define FLIPPING_HPP
#include <stdexcept>
#include "image.hpp"
#include <vector>
using namespace std;

enum class FlippingDirection
{

    VERTICAL,
    HORIZONTAL

};

class FlipError : public runtime_error {
    public:
        explicit FlipError(const string& message) : runtime_error(message) {}
};

class ImageFlipper 
{

public:
   void flip(Image&image, const FlippingDirection direction);

private:
   void flipVertical(vector<vector<uint8_t>>& matrix);
   void flipHorizontal(vector<vector<uint8_t>>& matrix);

};

#endif  //FLIPPING_HPP
