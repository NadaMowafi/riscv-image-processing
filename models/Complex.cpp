#include "Complex.hpp"

using namespace std;

Complex::Complex() : real(0.0), imag(0.0) {}

Complex::Complex(double r, double i) : real(r), imag(i) {}

Complex Complex::operator+(const Complex& other) const {
    return Complex(real + other.real, imag + other.imag);
}

Complex Complex::operator-(const Complex& other) const {
    return Complex(real - other.real, imag - other.imag);
}

Complex Complex::operator*(const Complex& other) const {
    return Complex(
        real * other.real - imag * other.imag,
        real * other.imag + imag * other.real
    );
}

Complex Complex::operator/(const Complex& other) const {
    double denominator = other.real * other.real + other.imag * other.imag;
    if (denominator == 0) {
        throw runtime_error("Division by zero in Complex numbers!");
    }
    return Complex(
        (real * other.real + imag * other.imag) / denominator,
        (imag * other.real - real * other.imag) / denominator
    );
}

void Complex::operator+=(const Complex& other) {
    real += other.real;
    imag += other.imag;
}

void Complex::operator-=(const Complex& other) {
    real -= other.real;
    imag -= other.imag;
}

void Complex::operator*=(const Complex& other) {
    double newReal = real * other.real - imag * other.imag;
    double newImag = real * other.imag + imag * other.real;
    real = newReal;
    imag = newImag;
}

void Complex::operator/=(const Complex& other) {
    double denominator = other.real * other.real + other.imag * other.imag;
    if (denominator == 0) {
        throw runtime_error("Division by zero in Complex numbers!");
    }
    double newReal = (real * other.real + imag * other.imag) / denominator;
    double newImag = (imag * other.real - real * other.imag) / denominator;
    real = newReal;
    imag = newImag;
}

void Complex::operator*=(double scalar) {
    real *= scalar;
    imag *= scalar;
}

bool Complex::operator==(const Complex& other) const {
    return real == other.real && imag == other.imag;
}

double Complex::magnitude() const {
    return std::sqrt(real * real + imag * imag);
}

Complex Complex::conjugate() const {
    return Complex(real, -imag);
}

std::ostream& operator<<(std::ostream& os, const Complex& c) {
    os << c.real << " + " << c.imag << "i";
    return os;
}