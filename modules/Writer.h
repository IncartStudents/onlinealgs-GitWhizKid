#ifndef WRITER_H
#define WRITER_H

#include "CircularBuffer.h"
#include <string>

void write_to_file(const CircularBuffer& output, int size, double step);
void save_state(int processed_count, std::streampos current_position, const std::string& filename);

#endif // WRITER_H