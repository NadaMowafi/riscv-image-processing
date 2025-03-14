#include "image.hpp"
#include <vector>

enum class FlippingDirection
{

    Vertical,
    Horizontal

};

class ImageFlipper 
{

public:
   void flip(Image&image, const FlippingDirection direction);

private:
   void flipVertical(std::vector<std::vector<uint8_t>>& matrix);
   void flipHorizontal(std::vector<std::vector<uint8_t>>& matrix);

};

#endif

