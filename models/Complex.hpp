#ifndef COMPLEX_HPP
#define COMPLEX_HPP

#include <iostream>
#include <cmath>

using namespace std;

class Complex {
public:
    double real;
    double imag;

    Complex();
    Complex(double r, double i);

    Complex operator+(const Complex& other) const;
    Complex operator-(const Complex& other) const;
    Complex operator*(const Complex& other) const;
    Complex operator/(const Complex& other) const;

    void operator+=(const Complex& other);
    void operator-=(const Complex& other);
    void operator*=(const Complex& other);
    void operator/=(const Complex& other);
    void operator*=(double scalar);

    bool operator==(const Complex& other) const;

    double magnitude() const;
    Complex conjugate() const;

    friend ostream& operator<<(std::ostream& os, const Complex& c);
};

#endif // COMPLEX_H
