#ifndef ANNOY_CVTYPES_H
#define ANNOY_CVTYPES_H

#include <stdio.h>
#include <string>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __MINGW32__
#include "mman.h"
#include <windows.h>
#else
#include <sys/mman.h>
#endif

#include <string.h>
#include <math.h>
#include <vector>
#include <algorithm>
#include <queue>
#include <limits>

#include "comressedvector.h"

#ifndef ANNOY_NODE_ATTRIBUTE
#define ANNOY_NODE_ATTRIBUTE __attribute__((__packed__))
// TODO: this is turned on by default, but may not work for all architectures! Need to investigate.
#endif

using std::vector;
using std::string;
using std::pair;
using std::numeric_limits;
using std::make_pair;

struct AngularCV {
    template<typename S>
    struct ANNOY_NODE_ATTRIBUTE Node {
        S n_descendants;
        S point_ids[2];
        Node *children[2];
        S *v;

        Node() {
            n_descendants = 0;
            memset(point_ids, 0, sizeof(S) * 2);
            memset(children, 0, sizeof(Node*) * 2);
            v = NULL;
        }
        Node(size_t n) {
            Node();
            v = malloc(sizeof(S) * n);
        }
        ~Node() {
            if (v) free(v);
        }
    };

    template<typename S, typename T>
    static inline T distance(const CompressedVector<S, T>& x, const CompressedVector<S, T>& y) {
        T pp = scalar(x, x);
        T qq = scalar(y, y);
        T pq = scalar(x, y);

        T ppqq = pp * qq;
        if (ppqq > 0) return 2.0 - 2.0 * pq / sqrt(ppqq);
        else return 2.0;
    }

    template<typename S, typename T>
    static inline T margin(const CompressedVector<S, T>& n, const CompressedVector<S, T>& y) {
        return scalar(n, y);
    }

    template<typename S, typename T, typename Random>
    static inline bool side(const CompressedVector<S, T>& n, const CompressedVector<S, T>& y, Random& random) {
        T dot = margin(n, y);
        if (dot != 0)
            return (dot > 0);
        else
            return random.flip();
    }

    template<typename S, typename T>
    static inline void hiperplane(const CompressedVector<S, T>& v1, const CompressedVector<S, T>& v2, CompressedVector<S, T>& v) {
        T v1_norm = norm(v1);
        T v2_norm = norm(v2);
        bitset<MASK_SIZE> _union = v1.mask | v2.mask;

        v.resize(_union.count());

        T v1_temp, v2_temp;
        S v1_count = 0, v2_count = 0, v_count = 0;

        for (S i = 0; i < _union.size(); i++) {
            v1_temp = (v1.mask[i]) ? v1.v[v1_count++] : 0;
            v2_temp = (v2.mask[i]) ? v2.v[v2_count++] : 0;

            if (_union[i]) {
                v.v[i] = v1_temp/v1_norm - v2_temp/v2_norm;
            }
        }

        normalize(v);
    }

    template<typename S, typename T, typename Random>
    static inline void create_split(const vector<CompressedVector<S, T>*>&values, Random& random, CompressedVector<S, T>& n) {
        size_t count = values.size();
        size_t i = random.index(count);
        size_t j = random.index(count-1);
        j += (j >= i); // ensure that i != j

        CompressedVector<S, T>* iv = values[i]->v;
        CompressedVector<S, T>* jv = values[j]->v;

        hiperplane(*iv, *jv, n);
    }

    template<typename T>
    static inline T normalized_distance(T distance) {
        // Used when requesting distances from Python layer
        // Turns out sometimes the squared distance is -0.0
        // so we have to make sure it's a positive number.
        return sqrt(std::max(distance, T(0)));
    }
};

#endif //ANNOY_CVTYPES_H
