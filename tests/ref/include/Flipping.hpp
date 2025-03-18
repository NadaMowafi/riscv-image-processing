#ifndef FLIPPING_HPP
#define FLIPPING_HPP
#include <stdexcept>
#include "image.hpp"
#include <vector>

enum class FlippingDirection
{

    VERTICAL,
    HORIZONTAL

};

class FlipError : public std::runtime_error {
    public:
        explicit FlipError(const std::string& message) : std::runtime_error(message) {}
};

class ImageFlipper 
{

public:
   void flip(Image&image, const FlippingDirection direction);

private:
   void flipVertical(std::vector<std::vector<uint8_t>>& matrix);
   void flipHorizontal(std::vector<std::vector<uint8_t>>& matrix);

};

#endif  //FLIPPING_HPP
