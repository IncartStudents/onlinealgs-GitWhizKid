#include <iostream>
#include <vector>
#include <cmath>
#include <random>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

std::vector<double> compute_coeff(int window_size) {
    std::vector<double> B(window_size);
    for (int num = 0; num < window_size; ++num) {
        B[num] = (12 * num - 6 * (window_size - 1)) / (window_size * (window_size * window_size - 1));
    }
    return B;
}

int main() {
    const int fs = 1000;
    const double duration = 1.0;
    const int N = fs * duration;
    const double freq = 5.0;

    // Создание синусоидального сигнала и добавление шума
    std::vector<double> signal(N);
    std::vector<double> noisy_signal(N);
    std::default_random_engine generator;
    std::uniform_real_distribution<double> distribution(-0.5, 0.5);

    for (int n = 0; n < N; ++n) {
        double t = static_cast<double>(n) / fs;
        double sine_value = std::sin(2 * M_PI * freq * t);
        double noise = distribution(generator); 
        signal[n] = sine_value;
        noisy_signal[n] = sine_value + noise;
    }

    // Параметры фильтра
    const int window_size = 80;
    std::vector<double> B = compute_coeff(window_size);
    std::vector<double> Y(N, 0); 

    // Применение КИХ-фильтра
    for (int n = 0; n < N; ++n) {
        double sum_xn = 0;
        int k = 0;
        for (int i = n; i >= n - window_size + 1; --i) {
            if (i >= 0) { 
                sum_xn += B[k] * noisy_signal[i];
                k++;
            }
        }
        Y[n] = sum_xn;
    }

    return 0;
}