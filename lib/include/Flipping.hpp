#ifndef FLIPPING_HPP
#define FLIPPING_HPP
#include <stdexcept>
#include <riscv_vector.h>
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
    static void __riscv_flip(Image<T> &image, const FlippingDirection direction);

private:
    static void __riscv_flipVertical(vector<vector<T>> &matrix);
    static void __riscv_flipHorizontal(vector<vector<T>> &matrix);
    static vuint8m1_t create_reverse_index(size_t vl);
};

#endif // FLIPPING_HPP
