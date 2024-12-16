#include "Reader.h"
#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>


int read_from_file(const std::string& filename, CircularBuffer& input, int buffer_size, std::streampos& current_position) {
    std::ifstream data_file(filename, std::ios::binary);
    if (!data_file) {
        std::cerr << "Error while opening file! (read_from_file)" << std::endl;
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

void load_state(int& processed_count, std::streampos& current_position, const std::string& filename) {
    nlohmann::json j;
    std::ifstream file(filename);
    if (file.is_open()) {
        file >> j;
        processed_count = j["processed_count"];

        current_position = static_cast<std::streampos>(j["current_position"].get<long long>());
        file.close();
    } else {
        current_position = 0;
        std::cerr << "Error while opening state file! (load_state)" << std::endl;
    }
}