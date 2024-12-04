#include <iostream>
#include <fstream>
#include <chrono>
#include <vector>
#include <cmath>
#include <thread>
#include <random>
#include <limits>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

enum class Mode {
    SINE_NOISE,
    STEP_SIGNAL,
    BINARY_FILE
};

class CircularBuffer {
public:
    CircularBuffer(size_t size) : size(size), head(0), count(0) {
        buffer = new double[size];
    }

    ~CircularBuffer() {
        delete[] buffer;
    }

    void add(double value) {
        buffer[head] = value;
        head = (head + 1) % size;
        if (count < size) count++;
    }

    double get(size_t index) const {
        if (index >= count) throw std::out_of_range("Index out of range");
        return buffer[(head + size - count + index) % size];
    }

    size_t get_count() const {
        return count;
    }

private:
    size_t size; // Максимальный размер буфера
    double* buffer; // Массив для хранения данных
    size_t head; // Индекс следующего элемента для записи
    size_t count; // Текущее количество элементов в буфере
};

void compute_coeff(std::vector<double>& coefficients, int window_size) {
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

double generateSineNoise(double t) {
    return std::sin(2 * M_PI * 1.0 * t) + ((rand() % 100) / 100.0 - 0.5); // Синусоида с шумом
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

void write_to_file(const CircularBuffer& input, const CircularBuffer& output, int size, double step) {
    std::ofstream data_file("data.txt");
    if (!data_file) {
        std::cerr << "Error while opening file!" << std::endl;
        return;
    }

    for (int i = 0; i < size; ++i) {
        double time = i * step;
        data_file << time << "," << input.get(i) << "," << (output.get(i) * 100.0) << std::endl;
    }

    data_file.close();
}

int read_from_file(const std::string& filename, CircularBuffer& input, int buffer_size) {
    std::ifstream data_file(filename, std::ios::binary);
    if (!data_file) {
        std::cerr << "Error while opening file!" << std::endl;
        return 0;
    }

    int total_read = 0;
    while (true) {
        std::vector<double> temp(buffer_size);
        data_file.read(reinterpret_cast<char*>(temp.data()), buffer_size * sizeof(double));
        int read_count = data_file.gcount() / sizeof(double);
        
        if (read_count == 0) {
            break; // Конец файла
        }

        for (int i = 0; i < read_count; ++i) {
            input.add(temp[i]);
        }
        total_read += read_count;
    }

    data_file.close();
    return total_read;
}

void process_data(Mode mode, int window_size, int buffer_size, double step) {
    std::vector<double> coeff(window_size);
    compute_coeff(coeff, window_size);

    CircularBuffer input(buffer_size);
    CircularBuffer output(buffer_size);
    std::vector<double> pX(window_size, 0.0);

    auto start = std::chrono::high_resolution_clock::now();

    if (mode == Mode::BINARY_FILE) {
        read_from_file("D:/Microsoft VS Code Projects/onlinealgs-GitWhizKid/breath_oxy115829.bin", input, buffer_size);
    } else {
        for (int i = 0; i < buffer_size; ++i) {
            double t = i * step;
            double value = (mode == Mode::SINE_NOISE) ? generateSineNoise(t) : myFunction(t);
            input.add(value);
        }
    }

    // Применение функции funcKIX к входным данным
    for (int i = 0; i < input.get_count(); ++i) {
        double processed_value = funcKIX(input.get(i), pX, coeff, window_size);
        output.add(processed_value);
    }

    write_to_file(input, output, input.get_count(), step);

    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    std::ofstream log_file("log.txt", std::ios::app);
    if (log_file) {
        log_file << "Processing completed in: " << duration.count() << "μs" << std::endl;
        log_file.close();
    } else {
        std::cerr << "Error while opening log file!" << std::endl;
    }
}

int main() {
    int window_size = 21;
    int buffer_size = 10000;
    double step = 0.01;
    Mode mode = Mode::BINARY_FILE;

    process_data(mode, window_size, buffer_size, step);
    return 0;
}