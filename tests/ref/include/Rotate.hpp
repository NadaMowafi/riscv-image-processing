#ifndef ROTATION_HPP
#define ROTATION_HPP

#include <stdexcept>
#include "Image.hpp"
using namespace std;

enum class RotationDirection
{
    CW_90,
    CCW_90,
    ROTATE_180
};

class RotationError : public runtime_error
{
public:
    explicit RotationError(const string &message) : runtime_error(message) {}
};

template <typename T = uint8_t>
class ImageRotator
{
public:
    static void rotate(Image<T> &image, RotationDirection direction);

private:
    static void rotate90CW(vector<vector<T>> &matrix);
    static void rotate90CCW(vector<vector<T>> &matrix);
    static void rotate180(vector<vector<T>> &matrix);
};

#endif // ROTATION_HPP
