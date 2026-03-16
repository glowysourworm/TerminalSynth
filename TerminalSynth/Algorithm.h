#pragma once

#ifndef ALGORITHM_H
#define ALGORITHM_H

#include <cmath>
#include <complex>
#include <exception>
#include <limits>
#include <vector>

class Algorithm
{
    using Complex = std::complex<double>;    
    
public:

    static void FFT(std::vector<Complex>* destination, double& maxReal)
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

        double maxEvenReal = std::numeric_limits<double>::min();
        double maxOddReal = std::numeric_limits<double>::min();

        // Conquer
        FFT(&even, maxEvenReal);
        FFT(&odd, maxOddReal);

        maxReal = fmax(maxEvenReal, maxReal);
        maxReal = fmax(maxOddReal, maxReal);

        // Combine
        for (size_t k = 0; k < N / 2; k++) 
        {
            Complex t = std::polar(1.0, (-2 * PI * k) / N) * odd[k];

            destination->at(k) = even[k] + t;
            destination->at(k + (N / 2)) = even[k] - t;

            maxReal = fmax(maxReal, destination->at(k).real());
            maxReal = fmax(maxReal, destination->at(k + (N / 2)).real());
        }
    }


    static void IFFT(std::vector<Complex>* destination, double& maxReal)
    {
        const double PI = acos(-1);
        int N = destination->size();

        if (N <= 1) 
            return;

        // Divide
        std::vector<Complex> even(N / 2), odd(N / 2);

        for (size_t i = 0; i < N / 2; i++)
        {
            even[i] = destination->at(i * 2);
            odd[i] = destination->at((i * 2) + 1);
        }

        double maxEvenReal = std::numeric_limits<double>::min();
        double maxOddReal = std::numeric_limits<double>::min();

        // Conquer
        IFFT(&even, maxEvenReal);
        IFFT(&odd, maxOddReal);

        maxReal = fmax(maxEvenReal, maxReal);
        maxReal = fmax(maxOddReal, maxReal);

        // Combine
        for (size_t k = 0; k < N / 2; k++) 
        {
            Complex t = std::polar(1.0, (2.0 * PI * k) / N) * odd[k];

            destination->at(k) = even[k] + t;
            destination->at(k + (N / 2)) = even[k] - t;

            maxReal = fmax(maxReal, destination->at(k).real());
            maxReal = fmax(maxReal, destination->at(k + (N / 2)).real());
        }
    }

    /// <summary>
    /// Multiplies the input vector by a Guassian window function. https://en.wikipedia.org/wiki/Window_function
    /// </summary>
    /// <param name="sigma">Standard deviation, should be less than 0.5 for this function</param>
    /// <param name="destination">Pointer to a vector of signal numbers with an even length</param>
    static void GaussianWindow(double sigma, std::vector<double>* destination)
    {
        const size_t N = destination->size();

        if (sigma > 0.5)
            throw new std::exception("Must use a value of sigma less than or equal to 0.5 for the GuassianWindow algorithm");

        if (N % 2 != 0)
            throw new std::exception("Must use an even size array for the GuassianWindow algorithm");

        size_t Nover2 = N / 2;

        for (int n = 0; n < N; n++)
        {
            double windowValue = std::exp(-0.5 * std::pow((n - (Nover2)) / (sigma * Nover2), 2));

            destination->at(n) *= windowValue;
        }
    }

    /// <summary>
    /// Generates a Guassian window function value. https://en.wikipedia.org/wiki/Window_function
    /// </summary>
    /// <param name="sigma">Standard deviation, should be less than 0.5 for this function</param>
    /// <param name="n">Current index of the signal vector</param>
    /// <param name="N">Length signal vector. MUST BE MULTIPLE OF 2!</param>
    static double GaussianWindow(double sigma, size_t n, size_t N)
    {
        if (sigma > 0.5)
            throw new std::exception("Must use a value of sigma less than or equal to 0.5 for the GuassianWindow algorithm");

        if (N % 2 != 0)
            throw new std::exception("Must use an even size array for the GuassianWindow algorithm");

        size_t Nover2 = N / 2;

        return std::exp(-0.5 * std::pow((n - (Nover2)) / (sigma * Nover2), 2));
    }

    /// <summary>
    /// Generates a Guassian window function value based on a 3-sigma window. https://en.wikipedia.org/wiki/Window_function
    /// </summary>
    /// <param name="n">Current index of the signal vector</param>
    /// <param name="nBegin">Beginning of the gaussian</param>
    /// <param name="nEnd">End of the gaussian</param>
    static double Gaussian3Sigma(size_t n, size_t nBegin, size_t nEnd)
    {
        if (nBegin > nEnd)
            throw new std::exception("Invalid Gaussian Window parameters:  Algorithm.h");

        if (n < nBegin ||
            n > nEnd)
            throw new std::exception("Invalid Gaussian Window parameters:  Algorithm.h");

        if (nBegin == nEnd)
            return n == nBegin ? 1 : 0;

        const double PI = acos(-1);

        size_t mean = (size_t)(nEnd - nBegin / 2.0f);
        size_t sigma = ((nEnd - nBegin) / 6.0f);            // 6 total sigma lengths

        if (sigma == 0)
            return 1;

        float A = 1.0f / (std::sqrtf(2 * PI) * sigma);
        float z = (n - mean / (float)sigma);

        // Standard Gaussian centered at the midway point of the window, 3-sigma out.
        return A * std::exp(-0.5 * z * z);
    }

    /// <summary>
    /// Generates a (non-random) number on the poisson distribution PDF
    /// </summary>
    static double Poisson(int x, double lambda)
    {
        if (x < 0)
            throw new std::exception("Invalid Poisson x input:  Algorithm.h");

        if (lambda < 1)
            throw new std::exception("Invalid Poisson lambda parameter:  Algorithm.h");

        return (pow(lambda, x) * exp(-1 * lambda)) / Factorial(x);
    }

    static size_t Factorial(int n)
    {
        if (n < 0)
            throw new std::exception("Invalid factorial number:  Algorithm.h");

        if (n == 0)
            return 1;

        else
            return n * Factorial(n - 1);
    }
};

#endif