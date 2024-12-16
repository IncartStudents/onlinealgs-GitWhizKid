#include "Writer.h"
#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>


void write_to_file(const CircularBuffer& output, int size, double step) {
    std::ofstream data_file("data.txt"); // Открываем файл в текстовом режиме
    if (!data_file) {
        std::cerr << "Error while opening file! (write_to_file)" << std::endl;
        return;
    }

    for (int i = 0; i < size; ++i) {
        double time = i * step;
        double output_value = output.get(i) * 100.0;

        // Записываем данные в текстовом формате
        data_file << time << "\t" << output_value << "\n"; // Используем табуляцию для разделения значений
    }

    data_file.close();
}

void save_state(int processed_count, std::streampos current_position, const std::string& filename) {
    nlohmann::json j;
    j["processed_count"] = processed_count;
    j["current_position"] = static_cast<long long>(current_position);

    std::ofstream file(filename);
    if (file.is_open()) {
        file << j.dump(4);
        file.close();
    } else {
        std::cerr << "Error while opening state file! (save_state)" << std::endl;
    }
}