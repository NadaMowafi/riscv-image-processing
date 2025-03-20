#ifndef ROTATION_HPP
#define ROTATION_HPP

#include <stdexcept>
#include "image.hpp"
using namespace std;

enum class RotationDirection {
    CW_90,
    CCW_90,
    ROTATE_180
};

class RotationError : public runtime_error {
public:
    explicit RotationError(const string& message) : runtime_error(message) {}
};

class ImageRotator {
public:
    static void rotate(Image& image, RotationDirection direction);

private:
    static void rotate90CW(vector<vector<uint8_t>>& matrix);
    static void rotate90CCW(vector<vector<uint8_t>>& matrix);
    static void rotate180(vector<vector<uint8_t>>& matrix);
};

#endif // ROTATION_HPP
