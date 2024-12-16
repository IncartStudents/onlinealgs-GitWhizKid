#include "Processor.h"
#include <cmath>
#include <cstdlib>
#include "Reader.h"


void compute_coeff(std::vector<double>& coefficients, int window_size) {
    double denominator = window_size * (window_size * window_size - 1);
    for (int num = 0; num < window_size; ++num) {
        coefficients[num] = -1.0 * (12.0 * num - 6.0 * (window_size - 1)) / denominator;
    }
}

double funcKIX(double X0, std::vector<double>& pX, const std::vector<double>& pB, int n) {
    for (int i = n - 1; i > 0; i--) {
        pX[i] = pX[i - 1];
    }
    pX[0] = X0;

    double Y = 0.0;
    for (int i = 0; i < n; i++) {
        Y += pB[i] * pX[i];
    }
    return Y;
}

int process_data(int window_size, CircularBuffer& input, CircularBuffer& output) {
    std::vector<double> coeff(window_size);
    compute_coeff(coeff, window_size);
    std::vector<double> pX(window_size, 0.0);

    for (int i = 0; i < input.get_count(); ++i) {
        double processed_value = funcKIX(input.get(i), pX, coeff, window_size);
        output.add(processed_value);
    }

    return output.get_count();
}