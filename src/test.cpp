#include "annoylib.h"
#include "kissrandom.h"
#include "comressedvector.h"
#include "cvtypes.h"
#include "annoycv.h"

using std::vector;


void test_legacy_annoy();
void test_cv_annoy();
void test_compressed_array();

static float_t elem[7][3] = {{1, 0, 0}, {0,1,0}, {0,0,1}, {1, 2, 0}, {5, 5, 5}, {3,5,7}, {2,1,4}};

int main(int argc, char *argv[]) {

    // test_legacy_annoy();
    // test_compressed_array();
    test_cv_annoy();

    return 0;
}

void test_legacy_annoy() {
    AnnoyIndex<int32_t, float_t, Angular, Kiss64Random> *index = new AnnoyIndex<int32_t, float_t, Angular, Kiss64Random>(3);

    index->add_item(0, elem[0]);
    index->add_item(1, elem[1]);
    index->add_item(2, elem[2]);
    index->add_item(3, elem[3]);
    index->add_item(4, elem[4]);
    index->add_item(5, elem[5]);
    index->add_item(6, elem[6]);

    index->build(-1);

    vector<int32_t> result;
    index->get_nns_by_item(4, 100, (size_t)-1, &result, NULL);

    delete index;
}

void test_cv_annoy() {
    AnnoyIndexCV<uint32_t, float_t, AngularCV, Kiss64Random> *index = new AnnoyIndexCV<uint32_t, float_t, AngularCV, Kiss64Random>(3, 2);

    index->add_item(0, elem[0]);
    index->add_item(1, elem[1]);
    index->add_item(2, elem[2]);
    index->add_item(3, elem[3]);
    index->add_item(4, elem[4]);
    index->add_item(5, elem[5]);
    index->add_item(6, elem[6]);
}

void test_compressed_array() {
    CompressedVector<int32_t, float_t> v1;
    CompressedVector<int32_t, float_t> v2;

    convert(elem[3], 3, v1);
    convert(elem[4], 3, v2);

    float_t s = scalar(v1, v2);
    printf("Scalar of elements %d and %d is %.2f\n", 3, 4, s);

    float_t n = norm(v2);
    printf("Norm of element %d is %.2f\n", 4, n);
}