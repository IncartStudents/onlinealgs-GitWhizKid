#include <iostream>
#include <fstream>
#include <chrono>
#include <vector>
#include <cmath>
#include <thread>
#include <random>
#include <limits>
#include <nlohmann/json.hpp>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

std::streampos current_position = 0;

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

    data_file.seekg(current_position);
    std::vector<double> temp(buffer_size);
    data_file.read(reinterpret_cast<char*>(temp.data()), buffer_size * sizeof(double));
    int read_count = data_file.gcount() / sizeof(double);
    
    for (int i = 0; i < read_count; ++i) {
        input.add(temp[i]);
    }

    current_position = data_file.tellg();
    data_file.close();
    return read_count;
}

void save_state(const CircularBuffer& input, const CircularBuffer& output, int processed_count, const std::string& filename) {
    nlohmann::json j;
    j["processed_count "] = processed_count;
    j["current_position"] = static_cast<long long>(current_position);
    j["input"] = std::vector<double>(input.get_count());
    j["output"] = std::vector<double>(output.get_count());

    for (size_t i = 0; i < input.get_count(); ++i) {
        j["input"][i] = input.get(i);
    }
    for (size_t i = 0; i < output.get_count(); ++i) {
        j["output"][i] = output.get(i);
    }

    std::ofstream file(filename);
    if (file.is_open()) {
        file << j.dump(4); // Сохранение в формате JSON с отступами
        file.close();
    } else {
        std::cerr << "Error while opening state file!" << std::endl;
    }
}

void load_state(CircularBuffer& input, CircularBuffer& output, int& processed_count, const std::string& filename) {
    nlohmann::json j;
    std::ifstream file(filename);
    if (file.is_open()) {
        file >> j;
        processed_count = j["processed_count"];

        for (const auto& value : j["input"]) {
            input.add(value);
        }
        for (const auto& value : j["output"]) {
            output.add(value);
        }
        current_position = static_cast<std::streampos>(j["current_position"].get<long long>());
        file.close();
    } else {
        std::cerr << "Error while opening state file!" << std::endl;
    }
}

int process_data(Mode mode, int window_size, int buffer_size, CircularBuffer& input, CircularBuffer& output, double step, const std::string& path, bool& running) {
    
    std::vector<double> coeff(window_size);
    compute_coeff(coeff, window_size);
    std::vector<double> pX(window_size, 0.0);

    if (mode == Mode::BINARY_FILE) {
        int read_count = read_from_file(path, input, buffer_size);
        if (read_count == 0) return 0;
    }
    else {
        for (int i = 0; i < buffer_size && running; ++i) {
            double t = i * step;
            double value = (mode == Mode::SINE_NOISE) ? generateSineNoise(t) : myFunction(t);
            input.add(value);
        }
    }

    // Применение функции funcKIX к входным данным
    for (int i = 0; i < input.get_count() && running; ++i) {
        double processed_value = funcKIX(input.get(i), pX, coeff, window_size);
        output.add(processed_value);
    }

    write_to_file(input, output, output.get_count(), step);

    return output.get_count();
}

int main() {
    int window_size = 21;
    int buffer_size = 10000;
    double step = 0.01;
    Mode mode = Mode::BINARY_FILE;
    bool running = true;

    CircularBuffer input(buffer_size);
    CircularBuffer output(buffer_size);
    int processed_count = 0;

    std::string path = "D:/Microsoft VS Code Projects/onlinealgs-GitWhizKid/breath_oxy115829.bin"; // Путь к бинарному файлу
    std::string state_file = "state.json"; // Файл для сохранения состояния

    load_state(input, output, processed_count, state_file);

    auto total_start = std::chrono::high_resolution_clock::now();

    while (running){
        processed_count = process_data(mode, window_size, buffer_size, input, output, step, path, running);
        if (processed_count == 0) {
            running = false;
            break;
        }
        save_state(input, output, processed_count, state_file);
    }
    
    auto total_stop = std::chrono::high_resolution_clock::now();
    auto total_duration = std::chrono::duration_cast<std::chrono::microseconds>(total_stop - total_start).count();
   
    std::ofstream log_file("log.txt", std::ios::app);
    if (log_file) {
        log_file << "Total processing time: " << total_duration << "μs" << std::endl;
        log_file.close();
    } else {
        std::cerr << "Error while opening log file!" << std::endl;
    }
    
    return 0;
}