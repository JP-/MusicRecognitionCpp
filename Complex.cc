#include <iostream>
#include <cmath>
#include "Complex.hpp"

using namespace std;

/***************************************************************************/


Complex::Complex(double real, double imag) {
	re = real;
	im = imag;
}
/***************************************************************************/

// return abs/modulus/magnitude and angle/phase/argument
double Complex::abs() {
	return hypot(re, im);
} // Math.sqrt(re*re + im*im)

/***************************************************************************/

double Complex::phase() {
	return atan2(im, re);
} // between -pi and pi

/***************************************************************************/

// return a new Complex object whose value is (this + b)
Complex Complex::plus(Complex b) {
	double real = this->re + b.re;
	double imag = this->im + b.im;
	return Complex(real, imag);
}

/***************************************************************************/

// return a new Complex object whose value is (this - b)
Complex Complex::minus(Complex b) {
	double real = this->re - b.re;
	double imag = this->im - b.im;
	return Complex(real, imag);
}

/***************************************************************************/

// return a new Complex object whose value is (this * b)
Complex Complex::times(Complex b) {
	double real = this->re * b.re - this->im * b.im;
	double imag = this->re * b.im + this->im * b.re;
	return Complex(real, imag);
}

/***************************************************************************/

// scalar multiplication
// return a new object whose value is (this * alpha)
Complex Complex::times(double alpha) {
	return Complex(alpha * re, alpha * im);
}

/***************************************************************************/

// return a new Complex object whose value is the conjugate of this
Complex Complex::conjugate() {
	return Complex(re, -im);
}

/***************************************************************************/

// return a new Complex object whose value is the reciprocal of this
Complex Complex::reciprocal() {
	double scale = re * re + im * im;
	return Complex(re / scale, -im / scale);
}

/***************************************************************************/

// return the real or imaginary part
double Complex::getRe() {
	return re;
}

/***************************************************************************/

double Complex::getIm() {
	return im;
}

/***************************************************************************/

// return a / b
Complex Complex::divides(Complex b) {
	return times(b.reciprocal());
}

/***************************************************************************/

// return a new Complex object whose value is the complex exponential of
// this
Complex Complex::Exp() {
	return Complex(exp(re) * cos(im), exp(re) * sin(im));
}

/***************************************************************************/

// return a new Complex object whose value is the complex sine of this
Complex Complex::Sin() {
	return Complex(sin(re) * cosh(im), cos(re) * sinh(im));
}

/***************************************************************************/

// return a new Complex object whose value is the complex cosine of this
Complex Complex::Cos() {
	return Complex(cos(re) * cosh(im), -sin(re) * sinh(im));
}

/***************************************************************************/

// return a new Complex object whose value is the complex tangent of this
Complex Complex::Tan() {
	return Sin().divides(Cos());
}

/***************************************************************************/
