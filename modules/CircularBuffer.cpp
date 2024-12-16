#include "CircularBuffer.h"

CircularBuffer::CircularBuffer(size_t size) : size(size), head(0), count(0) {
    buffer = new double[size];
}

CircularBuffer::~CircularBuffer() {
    delete[] buffer;
}

void CircularBuffer::add(double value) {
    buffer[head] = value;
    head = (head + 1) % size;
    if (count < size) count++;
}

double CircularBuffer::get(size_t index) const {
    if (index >= count) throw std::out_of_range("Index out of range");
    return buffer[(head + size - count + index) % size];
}

size_t CircularBuffer::get_count() const {
    return count;
}