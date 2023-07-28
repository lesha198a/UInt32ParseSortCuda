//
// Created by oleksii on 7/27/23.
//
#include "sort.cuh"

#include <thrust/sort.h>


namespace statCuda {
thrust::device_vector<uint32_t> *d_vec;
uint32_t *arrUint;
} // namespace statCuda

__attribute__((constructor)) void con() {
    statCuda::d_vec = new thrust::device_vector<uint32_t >;
    statCuda::d_vec->reserve(67108864);
    statCuda::arrUint = new uint32_t[134217728];
}

void sort(thrust::device_vector<uint32_t> &d_vec)
{
    thrust::stable_sort(d_vec.begin(), d_vec.end());
}

void des() {
    delete[] statCuda::arrUint;
}
