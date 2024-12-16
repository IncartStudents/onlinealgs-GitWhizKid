#ifndef CIRCULAR_BUFFER_H
#define CIRCULAR_BUFFER_H

#include <vector>
#include <stdexcept>

class CircularBuffer {
public:
    CircularBuffer(size_t size);
    ~CircularBuffer();

    void add(double value);
    double get(size_t index) const;
    size_t get_count() const;

private:
    size_t size;
    double* buffer;
    size_t head;
    size_t count;
};

#endif // CIRCULAR_BUFFER_H