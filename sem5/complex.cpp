#include <iostream>
#include <cmath>

class C {
public:
    double re, im; // члены класса

    // Конструкторы
    C(double real = 0, double imag = 0) : re(real), im(imag) {} // конструктор от пары чисел
    C(double real) : re(real), im(0) {} // конструктор от одного числа
    C(const C& other) : re(other.re), im(other.im) {} // конструктор копирования

    // Арифметические операции
    C operator+(const C& other) const {
        return C(re + other.re, im + other.im);
    }

    C operator-(const C& other) const {
        return C(re - other.re, im - other.im);
    }

    C operator*(const C& other) const {
        return C(re * other.re - im * other.im, 2 * re * other.im + im * other.re);
    }

    C operator/(const C& other) const {
        if (other.re == 0 || other.im == 0) {
            std::cerr << "Неdefined division by zero!" << std::endl;
            return C();
        }
        return C((re * other.re + im * other.im) / (other.re * other.re + other.im * other.im),
                   (re * other.im - im * other.re) / (other.re * other.re + other.im * other.im));
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
        return C(real * complex.re - complex.im, 2 * real * complex.im + complex.re);
    }

    friend C operator*(const C& complex, double real) {
        return C(complex.re * real - complex.im, 2 * complex.re * real + complex.im);
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
