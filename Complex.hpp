
#ifndef COMPLEX

#define COMPLEX

#include <iostream>


class Complex
{
	private:
		double re; // the real part
		double im; // the imaginary part

	public:

		Complex(){}
		
		// create a new object with the given real and imaginary parts
		Complex(double real, double imag);


		// return abs/modulus/magnitude and angle/phase/argument
		double abs(); // Math.sqrt(re*re + im*im)

		double phase(); // between -pi and pi

		// return a new Complex object whose value is (this + b)
		Complex plus(Complex b);

		// return a new Complex object whose value is (this - b)
		Complex minus(Complex b);

		// return a new Complex object whose value is (this * b)
		Complex times(Complex b);

		// scalar multiplication
		// return a new object whose value is (this * alpha)
		Complex times(double alpha);

		// return a new Complex object whose value is the conjugate of this
		Complex conjugate();

		// return a new Complex object whose value is the reciprocal of this
		Complex reciprocal();

		// return the real or imaginary part
		double getRe();

		double getIm();

		// return a / b
		Complex divides(Complex b);

		// return a new Complex object whose value is the complex
		// exponential of this
		Complex Exp();

		// return a new Complex object whose value is the complex sine of this
		Complex Sin();

		// return a new Complex object whose value is the complex cosine of this
		Complex Cos();

		// return a new Complex object whose value is the complex tangent of this
		Complex Tan();

};

#endif