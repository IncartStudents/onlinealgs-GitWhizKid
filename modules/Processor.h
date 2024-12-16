#ifndef PROCESSOR_H
#define PROCESSOR_H

#include "CircularBuffer.h"
#include <vector>
#include <string>

enum class Mode {
    SINE_NOISE,
    STEP_SIGNAL,
    BINARY_FILE
};

int process_data(int window_size, CircularBuffer& input, CircularBuffer& output);

#endif // PROCESSOR_H