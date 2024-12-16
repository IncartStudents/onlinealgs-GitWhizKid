#include <iostream>
#include <filesystem>
#include <fstream>
#include <chrono>
#include "modules/CircularBuffer.h"
#include "modules/Reader.h"
#include "modules/Processor.h"
#include "modules/Writer.h"


#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif


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
    return std::sin(2 * M_PI * 1.0 * t) + ((rand() % 100) / 100.0 - 0.5);
}

void generate_data(Mode mode, int buffer_size, double step, CircularBuffer& input) {
    for (int i = 0; i < buffer_size; ++i) {
        double t = i * step;
        double value = (mode == Mode::SINE_NOISE) ? generateSineNoise(t) : myFunction(t);
        input.add(value);
    }
}


int main() {
    int window_size = 21;
    int buffer_size = 10000;
    double step = 0.01;
    Mode mode = Mode::SINE_NOISE; //  SINE_NOISE, STEP_SIGNAL, BINARY_FILE

    CircularBuffer input(buffer_size);
    CircularBuffer output(buffer_size);
    std::streampos current_position = 0;
    int processed_count =  0;

    std::string path = "D:/Microsoft VS Code Projects/onlinealgs-GitWhizKid/signals/breath_oxy115829.bin";
    std::string state_file = "state.json";

    load_state(processed_count, current_position, state_file);

    auto total_start = std::chrono::high_resolution_clock::now();

    
    if (mode == Mode::BINARY_FILE) {
        while (true) {
            int read_count = read_from_file(path, input, buffer_size, current_position);
            if (read_count == 0) return 0;

            current_position += read_count * sizeof(double);
            
            processed_count = process_data(window_size,  input, output);
            if (processed_count == 0) break;
            write_to_file(output, processed_count, step);
            save_state(processed_count, current_position, state_file);
            
        } 
    }
    else {
        generate_data(mode, 1000 * buffer_size, step, input);
        processed_count = process_data(window_size,  input, output);
        if (processed_count > 0) {
            write_to_file(output, processed_count, step);
            save_state(processed_count, current_position, state_file);
        }
    } 

    auto total_stop = std::chrono::high_resolution_clock::now();
    auto total_duration = std::chrono::duration_cast<std::chrono::microseconds>(total_stop - total_start).count();

    std::ofstream log_file("time.txt", std::ios::app);
    if (log_file) {
        log_file << "Total processing time: " << total_duration << "μs" << std::endl;
        log_file.close();
    } else {
        std::cerr << "Error while opening log file!" << std::endl;
    }
    return 0;
}