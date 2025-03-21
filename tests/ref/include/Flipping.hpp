#ifndef FLIPPING_HPP
#define FLIPPING_HPP
#include <stdexcept>
#include "Image.hpp"
#include <vector>
using namespace std;

enum class FlippingDirection
{

    VERTICAL,
    HORIZONTAL

};

class FlipError : public runtime_error
{
public:
    explicit FlipError(const string &message) : runtime_error(message) {}
};

template <typename T = uint8_t>
class ImageFlipper
{

public:
    static void flip(Image<T> &image, const FlippingDirection direction);

private:
    static void flipVertical(vector<vector<T>> &matrix);
    static void flipHorizontal(vector<vector<T>> &matrix);
};

#endif // FLIPPING_HPP
