#include <iostream>
#include <fstream>
#include <chrono>

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

// Функция для записи данных в файл
void write_to_file(const double* input, const double* output, int size, double step) {
    std::ofstream data_file("data.txt");
    if (!data_file) {
        std::cerr << "Error while opening file!" << std::endl;
        return;
    }

    // Записываем данные: время, оригинальный сигнал, отфильтрованный сигнал
    for (int i = 0; i < size; ++i) {
        double time = i * step; // вычисляем время
        data_file << time << "," << input[i] << "," << (output[i] * 100.0) << std::endl;
    }

    data_file.close();
}

// Функция для чтения данных из файла
int read_from_file(const std::string& filename, double* input, int max_size) {
    std::ifstream data_file(filename, std::ios::binary); // Добавлен флаг std::ios::binary
    if (!data_file) {
        std::cerr << "Error while opening file!" << std::endl;
        return 0;
    }

    data_file.read(reinterpret_cast<char*>(input), max_size * sizeof(double)); // Чтение данных в бинарном формате
    int count = data_file.gcount() / sizeof(double); // Получаем количество считанных элементов

    data_file.close();
    return count; // Возвращаем количество считанных значений
}

int main() {
    double step = 0.01; 
    int window_size = 21;
    int max_points = 100000000;

    // Выделяем память для коэффициентов, входных и выходных данных
    double* coeff = new double[window_size];
    double* input = new double[max_points];
    double* output = new double[max_points];

    compute_coeff(coeff, window_size);

    // Чтение входного сигнала из файла
    int num_points = read_from_file("breath_oxy115829.bin", input, max_points);
    if (num_points == 0) {
        delete[] coeff;
        delete[] input;
        delete[] output;
        return 1; // Завершаем программу, если не удалось прочитать данные
    }

    double* pX = new double[window_size](); // Инициализация нулями

    // Измерение времени выполнения алгоритма
    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < num_points; ++i) {
        output[i] = funcKIX(input[i], pX, coeff, window_size);
    }

    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);


    // Записываем данные в файл в нужном формате
    write_to_file(input, output, num_points, step);

    // Освобождаем выделенную память
    delete[] coeff;
    delete[] input;
    delete[] output;
    delete[] pX;

    std::cout << "Data written to file data.txt." << std::endl;
    std::cout << "Algorithm execution time: " << duration.count() << "ms" << std::endl;

    return 0;
}