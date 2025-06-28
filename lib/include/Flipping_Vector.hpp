#ifndef FLIPPINGV_HPP
#define FLIPPINGV_HPP
#include <stdexcept>
#include <riscv_vector.h>
#include "Image.hpp"
#include <vector>
using namespace std;

enum class FlippingDirection_V
{
    VERTICAL,
    HORIZONTAL
};

class FlipError_V : public runtime_error
{
public:
    explicit FlipError_V(const string &message) : runtime_error(message) {}
};

template <typename T = uint8_t>
class ImageFlipper_V
{

public:
    static void __riscv_flip(Image<T> &image, const FlippingDirection_V direction);

private:
    static void __riscv_flipVertical(vector<vector<T>> &matrix);
    static void __riscv_flipHorizontal(vector<vector<T>> &matrix);
    static vuint8m1_t create_reverse_index(size_t vl);
};

#endif // FLIPPING_HPP
