#include <iostream>
#include <vector>
#include <fstream>

// Функция для вычисления коэффициентов фильтра
std::vector<double> compute_coeff(int window_size) {
    std::vector<double> coefficients(window_size, 0.0);
    double denominator = window_size * (window_size * window_size - 1);
    
    for (int num = 0; num < window_size; ++num) {
        coefficients[num] = (12.0 * num - 6.0 * (window_size - 1)) / denominator;
    }
    
    return coefficients;
}

double myFunction(double t) {
    if (t < 0 || t > 2) {
        return 0; 
    } else if (t <= 0.75) {
        return -0.5; 
    } else if (t <= 1.25) {
        return 2 * t - 2; 
    } else {
        return 0.5; 
    }
}

// Функция для применения КИХ фильтра
double funcKIX(double X0, double* pX, double* pB, int n) {
    // Сдвигаем прошлые значения X
    for (int i = n - 1; i > 0; i--) {
        pX[i] = pX[i - 1]; // Двигаем прошлые X на шаг
    }
    pX[0] = X0; // Сохраняем текущее входное значение

    double Y = 0.0;
    // Суммируем произведения коэффициентов и входных значений
    for (int i = 0; i < n; i++) {
        Y += pB[i] * pX[i]; // Надо складывать все X начиная с X[0].
    }
    return Y;
}

// Функция для записи данных в файл
void write_to_file(const std::vector<double>& input, const std::vector<double>& output, double step) {
    std::ofstream data_file("data.txt");
    if (!data_file) {
        std::cerr << "Ошибка при открытии файла!" << std::endl;
        return;
    }

    // Записываем данные: время, оригинальный сигнал, отфильтрованный сигнал
    for (size_t i = 0; i < input.size(); ++i) {
        double time = i * step; // вычисляем время
        data_file << time << "," << input[i] << "," << output[i] << std::endl;
    }

    data_file.close();
}

int main() {
    double start_time = 0.0;
    double end_time = 2.0;
    double step = 0.01; 
    int window_size = 5; 

    std::vector<double> coeff = compute_coeff(window_size);
    std::vector<double> input;
    std::vector<double> output;

    // Массив для хранения предыдущих входных значений
    std::vector<double> pX(window_size, 0.0);

    // Генерация входного сигнала
    for (double t = start_time; t <= end_time; t += step) {
        double value = myFunction(t);
        input.push_back(value);
    }

    // Применение КИХ фильтра
    for (const auto& value : input) {
        double filtered_value = funcKIX(value, pX.data(), coeff.data(), window_size);
        output.push_back(filtered_value);
    }

    // Записываем данные в файл в нужном формате
    write_to_file(input, output, step);

    std::cout << "Данные записаны в файл data.txt." << std::endl;

    return 0;
}