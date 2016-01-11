#ifndef ANNOY_COMPRESSEDVECTOR_H
#define ANNOY_COMPRESSEDVECTOR_H

#include <stdint-gcc.h>
#include <malloc.h>
#include <bitset>

#ifndef MASK_SIZE
#define MASK_SIZE 3
#endif

using std::bitset;

template<typename T>
struct CompressedVector {
    uint16_t size;
    bitset<MASK_SIZE> mask;
    T *v;

    CompressedVector(uint16_t size) {
        this->size = size;
        v = (T*)malloc(size * sizeof(T));
    }
    ~CompressedVector() {
        if (v) free(v);
    }
};

template<typename T>
inline uint16_t true_size(T *data, uint16_t size) {
    uint16_t count = 0;
    for (uint16_t i = 0; i < size; i++) {
        if (data[i] != 0) count++;
    }
    return count;
}

template<typename T>
inline CompressedVector<T>* convert(T *data, uint16_t size) {
    CompressedVector<T> *vector = new CompressedVector<T>(true_size(data, size));

    uint16_t index = 0;
    for (uint16_t i = 0; i < size; i++) {
        if (data[i] != 0) {
            vector->mask.set(i);
            vector->v[index++] = data[i];
        }
    }
    return vector;
}

template<typename T>
inline T scalar(const CompressedVector<T>& v1, const CompressedVector<T>& v2) {
    T result = 0;
    uint16_t v1_index = 0;
    uint16_t v2_index = 0;

    bitset<MASK_SIZE> intersection = v1.mask & v2.mask;
    for (size_t i = 0; i < intersection.size(); i++) {
        if (intersection[i]) {
            result += v1.v[v1_index] * v2.v[v2_index];
        }
        if (v1.mask[i]) v1_index++;
        if (v2.mask[i]) v2_index++;
    }

    return result;
}

template<typename T>
inline T norm(const CompressedVector<T>& v) {
    // TODO: optimize, no need do intersection only mac operation on vector elements
    return sqrt(scalar(v, v));
}

template<typename T>
inline void normalize(CompressedVector<T>& v) {
    T norm = norm(v);
    for (size_t z = 0; z < v.size; z++)
        v.v[z] /= norm;
}

#endif //ANNOY_COMPRESSEDVECTOR_H
