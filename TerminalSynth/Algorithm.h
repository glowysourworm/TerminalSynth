#pragma once

#ifndef ALGORITHM_H
#define ALGORITHM_H

#include <cmath>
#include <complex>
#include <exception>
#include <vector>

class Algorithm
{
    using Complex = std::complex<double>;    
    
public:

    static void FFT(std::vector<Complex>* destination)
    {
        const double PI = acos(-1);
        const size_t N = destination->size();

        if (N <= 1)
            return;

        if (N % 2 != 0)
            throw new std::exception("Must use an even size array for the FFT algorithm");

        // Divide
        std::vector<Complex> even(N / 2);
        std::vector<Complex> odd(N / 2);

        for (size_t i = 0; i < N / 2; i++) 
        {
            even[i] = destination->at(i * 2);
            odd[i] = destination->at((i * 2) + 1);
        }

        // Conquer
        FFT(&even);
        FFT(&odd);

        // Combine
        for (size_t k = 0; k < N / 2; k++) 
        {
            Complex t = std::polar(1.0, (-2 * PI * k) / N) * odd[k];

            destination->at(k) = even[k] + t;
            destination->at(k + (N / 2)) = even[k] - t;
        }
    }
};

#endif