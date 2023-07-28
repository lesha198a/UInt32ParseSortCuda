//
// Created by oleksii on 7/27/23.
//

#ifndef CODECOMPETITION_SORT_CUH
#define CODECOMPETITION_SORT_CUH


#include <thrust/device_vector.h>
#include <thrust/host_vector.h>
#include <thrust/device_ptr.h>


namespace statCuda {
extern thrust::device_vector<uint32_t> *d_vec;
extern uint32_t *arrUint;
} // namespace statCuda

__attribute__((constructor)) void con();
__attribute__((destructor)) void des();
void sort(thrust::device_vector<uint32_t> &d_vec);

#endif //CODECOMPETITION_SORT_CUH
