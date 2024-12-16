#ifndef READER_H
#define READER_H

#include <string>
#include "CircularBuffer.h"

int read_from_file(const std::string& filename, CircularBuffer& input, int buffer_size, std::streampos& current_position);
void load_state(int& processed_count, std::streampos& current_position, const std::string& filename);

#endif // READER_H