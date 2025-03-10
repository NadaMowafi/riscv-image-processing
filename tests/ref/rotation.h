#ifndef ROTATION_H
#define ROTATION_H

#include <stdexcept>
#include "image.hpp"

enum class RotationDirection {
    CW_90,
    CCW_90,
    ROTATE_180
};

class RotationError : public std::runtime_error {
public:
    explicit RotationError(const std::string& message) : std::runtime_error(message) {}
};

class ImageRotator {
public:
    static void rotate(Image& image, RotationDirection direction);

private:
    static void rotate90CW(std::vector<std::vector<uint8_t>>& matrix);
    static void rotate90CCW(std::vector<std::vector<uint8_t>>& matrix);
    static void rotate180(std::vector<std::vector<uint8_t>>& matrix);
};

#endif // ROTATION_H
