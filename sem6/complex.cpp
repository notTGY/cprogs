#include <iostream>
#include <cmath>

class C {
public:
    double re, im; // члены класса

    // Конструкторы
    C(double real = 0, double imag = 0) : re(real), im(imag) {} // конструктор от пары чисел
    C(const C& other) : re(other.re), im(other.im) {} // конструктор копирования

    // Арифметические операции
    C operator+(const C& other) const {
        return C(re + other.re, im + other.im);
    }

    C operator-(const C& other) const {
        return C(re - other.re, im - other.im);
    }

    C operator*(const C& other) const {
        return C(re * other.re - im * other.im, re * other.im + im * other.re);
    }

    C operator/(const C& other) const {
        if (other.re == 0 && other.im == 0) {
            std::cerr << "Неdefined division by zero!" << std::endl;
            return C();
        }
        return C((re * other.re + im * other.im) / (other.re * other.re + other.im * other.im),
                   (other.re * im - re * other.im) / (other.re * other.re + other.im * other.im));
    }

    bool operator==(const C& other) const {
        return re == other.re && im == other.im;
    }

    bool operator!=(const C& other) const {
        return !(*this == other);
    }

    // Операции с действительными числами
    friend C operator+(double real, const C& complex) {
        return C(real + complex.re, complex.im);
    }

    friend C operator+(const C& complex, double real) {
        return C(complex.re + real, complex.im);
    }

    friend C operator*(double real, const C& complex) {
        return C(real * complex.re, real * complex.im);
    }

    friend C operator*(const C& complex, double real) {
        return C(complex.re * real, complex.im * real);
    }

    friend C operator/(double real, const C& complex) {
        if (complex.re == 0 && complex.im == 0) {
            std::cerr << "Неdefined division by zero!" << std::endl;
            return C();
        }
        return C((real * complex.re) / (complex.re * complex.re + complex.im * complex.im),
                   (- real * complex.im) / (complex.re * complex.re + complex.im * complex.im));
    }

    friend C operator/(const C& complex, double real) {
        return C(complex.re/real, complex.im/real);
    }

    friend C operator==(double real, const C& complex) {
        return complex.im == 0 && complex.re == real;
    }

    friend C operator==(const C& complex, double real) {
        return complex.im == 0 && complex.re == real;
    }

    friend C operator!=(double real, const C& complex) {
        return complex.im != 0 || complex.re != real;
    }

    friend C operator!=(const C& complex, double real) {
        return complex.im != 0 || complex.re != real;
    }

    // Вывод комплексного числа в виде two doubles
    friend std::ostream& operator<<(std::ostream& os, const C& complex) {
        return os << complex.re << " + " << complex.im << "i";
    }

    // Модуль комплексного числа
    double abs() const {
        return sqrt(re * re + im * im);
    }
};
