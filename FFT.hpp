
#ifndef FFT

#define FFT

#include <iostream>
#include "Complex.hpp"


// compute the FFT of x[], assuming its length is a power of 2
static Complex * fft(Complex x[], int size)
{
	int N = size;

	// base case
	if (N == 1)
	{
		Complex a(x[0].getRe(), x[0].getIm());
		Complex * array = new Complex[1];
		array[0] = x[0];
		return array;
	}

	// radix 2 Cooley-Tukey FFT
	if (N % 2 != 0)
	{
		std::cout << "N is not a power of 2" << std::endl;
	}

	// fft of even terms
	Complex even[N / 2];
	for (int k = 0; k < N / 2; k++) {
		even[k] = x[2 * k];
	}
	Complex * q = fft(even, N/2);

	// fft of odd terms
	Complex odd[N/2] = even; // reuse the array
	for (int k = 0; k < N / 2; k++) {
		odd[k] = x[2 * k + 1];
	}
	Complex * r = fft(odd, N/2);

	// combine
	Complex * y = new Complex[N];
	for (int k = 0; k < N / 2; k++) {
		double kth = -2 * k * M_PI / N;
		Complex wk (cos(kth), sin(kth));
		y[k] = q[k].plus(wk.times(r[k]));
		y[k + N / 2] = q[k].minus(wk.times(r[k]));
	}

	delete [] r;
	//std::cout << "r liberado" << std::endl;
	delete [] q;
	//std::cout << "q liberado" << std::endl;
	return y;
}

#endif
