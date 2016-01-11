#ifndef ANNOY_COMPRESSEDVECTOR_H
#define ANNOY_COMPRESSEDVECTOR_H

#include <stdint.h>
#include <malloc.h>
#include <bitset>

#ifndef MASK_SIZE
#define MASK_SIZE 3
#endif

using std::bitset;

template<typename S, typename T>
struct CompressedVector {
    S size;
    bitset<MASK_SIZE> mask;
    T *v;

    CompressedVector() {
        this->size = 0;
        this->v = NULL;
    }

    CompressedVector(S size) {
        this->size = size;
        v = (T*)malloc(size * sizeof(T));
    }

    ~CompressedVector() {
        if (v) free(v);
    }

    void resize(S size) {
        this->size = size;
        if (v) {
            v = (T*)realloc(v, size * sizeof(T));
        } else {
            v = (T*)malloc(size * sizeof(T));
        }
    }
};

template<typename S, typename T>
inline S true_size(const T* data, S size) {
    S count = 0;
    for (S i = 0; i < size; i++) {
        if (data[i] != 0) count++;
    }
    return count;
}

template<typename S, typename T>
inline void convert(const T* data, S size, CompressedVector<S, T>& vector) {
    vector.resize(true_size(data, size));

    uint16_t index = 0;
    for (uint16_t i = 0; i < size; i++) {
        if (data[i] != 0) {
            vector.mask.set(i);
            vector.v[index++] = data[i];
        }
    }
}

template<typename S, typename T>
inline T scalar(const CompressedVector<S, T>& v1, const CompressedVector<S, T>& v2) {
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

template<typename S, typename T>
inline T norm(const CompressedVector<S, T>& v) {
    // TODO: optimize, no need do intersection only mac operation on vector elements
    return sqrt(scalar(v, v));
}

template<typename S, typename T>
inline void normalize(CompressedVector<S, T>& v) {
    T norm = norm(v);
    for (size_t z = 0; z < v.size; z++)
        v.v[z] /= norm;
}

#endif //ANNOY_COMPRESSEDVECTOR_H
