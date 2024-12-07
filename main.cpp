#include <iostream>
#include <vector>
#include <cmath>
#include <random>
#include <fstream>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif


void write_to_file(const std::string& filename, const std::vector<double>& time, 
                   const std::vector<double>& noisy_signal, 
                   const std::vector<double>& filtered_signal, 
                   const std::vector<double>& original_signal) {
    std::ofstream file(filename);
    if (file.is_open()) {
        for (size_t i = 0; i < time.size(); ++i) {
            file << time[i] << "," << noisy_signal[i] << "," 
                 << filtered_signal[i] << "," << original_signal[i] << "\n";
        }
        file.close();
    } else {
        std::cerr << "Ошибка при открытии файла для записи!" << std::endl;
    }
}

std::vector<double> compute_coeff(int window_size) {
    std::vector<double> coefficients(window_size, 0.0);
    double denominator = window_size * (window_size * window_size - 1);
    
    for (int num = 0; num < window_size; ++num) {
        coefficients[num] = (12.0 * num - 6.0 * (window_size - 1)) / denominator;
    }
    
    return coefficients;
}

std::vector<double> applyFIRFilter(const std::vector<double>& X, const std::vector<double>& B) {
    int M = B.size(); // Длина фильтра
    int N = X.size(); // Длина входного сигнала
    std::vector<double> Y(N + M - 1, 0.0); // Выходной сигнал

    // Свертка
    for (int n = 0; n < N + M - 1; ++n) {
        for (int m = 0; m < M; ++m) {
            if (n - m >= 0 && n - m < N) { // Проверка на выход за пределы
                Y[n] += B[m] * X[n - m];
            }
        }
    }
    return Y;
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
   // Применение КИХ-фильтра
    std::vector<double> Y = applyFIRFilter(noisy_signal, B);

    // Запись данных в файл
    std::vector<double> time(N);
    for (int i = 0; i < N; ++i) {
        time[i] = static_cast<double>(i) / fs;
    }

    write_to_file("output.txt", time, noisy_signal, Y, signal);

    return 0;
}