#include <iostream>
#include <fstream>

// Функция для вычисления коэффициентов фильтра
void compute_coeff(double* coefficients, int window_size) {
    double denominator = window_size * (window_size * window_size - 1);
    
    for (int num = 0; num < window_size; ++num) {
        coefficients[num] = -1.0 * (12.0 * num - 6.0 * (window_size - 1)) / denominator;
    }
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
void write_to_file(const double* input, const double* output, int size, double step) {
    std::ofstream data_file("data.txt");
    if (!data_file) {
        std::cerr << "Ошибка при открытии файла!" << std::endl;
        return;
    }

    // Записываем данные: время, оригинальный сигнал, отфильтрованный сигнал
    for (int i = 0; i < size; ++i) {
        double time = i * step; // вычисляем время
        data_file << time << "," << input[i] << "," << (output[i] * 100.0) << std::endl;
    }

    data_file.close();
}

int main() {
    double start_time = 0.0;
    double end_time = 2.0;
    double step = 0.01; 
    int window_size = 21;
    int num_points = static_cast<int>((end_time - start_time) / step) + 1;

    // Выделяем память для коэффициентов, входных и выходных данных
    double* coeff = new double[window_size];
    double* input = new double[num_points];
    double* output = new double[num_points];

    compute_coeff(coeff, window_size);

    // Генерация входного сигнала
    for (int i = 0; i < num_points; ++i) {
        double t = start_time + i * step;
        input[i] = myFunction(t);
    }

    double* pX = new double[window_size](); // Инициализация нулями

    // Применение КИХ фильтра
    for (int i = 0; i < num_points; ++i) {
        output[i] = funcKIX(input[i], pX, coeff, window_size);
    }

    // Записываем данные в файл в нужном формате
    write_to_file(input, output, num_points, step);

    // Освобождаем выделенную память
    delete[] coeff;
    delete[] input;
    delete[] output;
    delete[] pX;
    
    std::cout << "Данные записаны в файл data.txt." << std::endl;

    return 0;
}