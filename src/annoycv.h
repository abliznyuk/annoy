#ifndef ANNOY_ANNOYCV_H
#define ANNOY_ANNOYCV_H

#include "cvtypes.h"
#include "annoylib.h"

#include <map>

using std::map;
using std::pair;

template<typename S, typename T, typename Distance, typename Random>
class AnnoyIndexCV /*: public AnnoyIndexInterface<S, T>*/ {

public:
    typedef Distance D;
    typedef typename D::template Node<S> Node;
    typedef CompressedVector<S, T> CV;

protected:
    uint32_t _dimention;
    S _leaf_size;
    map<S, size_t> _items_idx;
    Random _random;
    void* _nodes;
    void* _items;
    size_t _items_size;
    vector<Node *> _roots;

    bool _verbose;
    bool _loaded;

public:
    AnnoyIndexCV(int dimention, int leaf_size) : _random() {
        _dimention = (uint32_t)dimention;
        _leaf_size = leaf_size;
        _verbose = false;

        reinitialize();
    }
    ~AnnoyIndexCV() {
        //unload();
    }

    void add_item(S item, const T* w) {
        CV vector;
        convert(w, _dimention, vector);

        size_t element_size = offsetof(CV, v) + vector.size * sizeof(T);

        _items = realloc(_items, _items_size + element_size);
        _items_idx.insert(std::pair<S, size_t>(item, _items_size));
        _items_size += element_size;

        CV* n = _get_item(item);

        n->size = vector.size;
        n->mask = vector.mask;
        memcpy(&n->v, vector.v, vector.size * sizeof(T));
    }

    void reinitialize() {
        _nodes = NULL;

        _items = NULL;
        _items_idx.clear();
        _items_size = 0;

        _loaded = false;
        _roots.clear();
    }

protected:
    inline CV* _get_item(S i) {
        return (CV*)((uint8_t *)_items + _items_idx[i]);
    }
};

#endif //ANNOY_ANNOYCV_H
