#include "../TaskGeneratorAPI.h"
#include <cstdio>
#include <span>
#include <thread>
//#include <chrono>
//#include <thrust/host_vector.h>
#include <thrust/copy.h>
#include <pthread.h>
#include "sort.cuh"

namespace {

pthread_mutex_t safeCopy;
uint64_t all_sum = 0;
uint32_t *arrUint = nullptr;
size_t sizeUint = 0;

typedef struct
{
    char *input;
    uint32_t size;
    uint32_t *buf;
} parseArgs;

void *parse_thread(void *args)
{
    parseArgs *argsCasted = static_cast<parseArgs *>(args);
    uint32_t *hardcore = argsCasted->buf;
    size_t hardcoreIndex = 0;
    char *input = argsCasted->input;
    char *end = input + argsCasted->size;
    uint32_t buf = 0;
    char cBuf;
    uint64_t sumLocal = 0;
    while ((input + 1) != end) {
        cBuf = *input;
        if (cBuf == ' ') {
            hardcore[hardcoreIndex++] = buf;
            sumLocal += buf;
            buf = 0;
            input++;
            continue;
        }
        buf = cBuf - '0' + (buf * 10);
        input++;
    }
    cBuf = *input;
    buf = cBuf - '0' + (buf * 10);
    hardcore[hardcoreIndex++] = buf;
    sumLocal += buf;

    pthread_mutex_lock(&safeCopy);
    uint32_t *dst = arrUint + sizeUint;
    sizeUint += hardcoreIndex;
    all_sum += sumLocal;
    pthread_mutex_unlock(&safeCopy);
    memcpy(dst, hardcore, hardcoreIndex * sizeof(uint32_t));
    return nullptr;
}
} // namespace

extern "C" {
DllExport TaskOutputData CallConv ComputeTask(TaskInputData data)
{
    arrUint = statCuda::arrUint;
    uint32_t *bufUInt = arrUint + 67108864;
    size_t threadCount = std::thread::hardware_concurrency();
    pthread_t threads[64];
    parseArgs threadsArgs[64];
    pthread_mutex_init(&safeCopy, nullptr);
    uint32_t threadOffset = 67108864 / threadCount;
    char *dataCasted = const_cast<char *>(data.data);
    char *dataOffset = dataCasted;
    char *dataEnd = dataCasted + data.data_size;
    size_t defaultDataOffset = data.data_size / threadCount;
    size_t nextDataOffsetLen;
    char *nextDataOffset;
    for (int i = 0; i < threadCount; ++i) {
        nextDataOffsetLen = defaultDataOffset;
        nextDataOffset = dataOffset + nextDataOffsetLen;
        if (nextDataOffset - dataCasted < data.data_size) {
            while (*nextDataOffset != ' ' && nextDataOffset != dataEnd) {
                nextDataOffset++;
                nextDataOffsetLen++;
            }
        } else if (nextDataOffset - dataCasted > data.data_size) {
            nextDataOffsetLen = data.data_size - (dataOffset - dataCasted);
            nextDataOffset = dataEnd;
        }

        threadsArgs[i].input = dataOffset;
        threadsArgs[i].size = nextDataOffsetLen;
        threadsArgs[i].buf = bufUInt;
        pthread_create(threads + i, nullptr, parse_thread, threadsArgs + i);
        if (nextDataOffset == dataEnd) {
            threadCount = i + 1;
            break;
        }
        bufUInt += threadOffset;
        dataOffset += nextDataOffsetLen + 1;
    }

    for (int i = 0; i < threadCount; ++i) {
        pthread_join(threads[i], nullptr);
    }

    TaskOutputData out{0};

    out.average = all_sum / sizeUint;
    statCuda::d_vec->assign(arrUint, arrUint + sizeUint);
    sort(*statCuda::d_vec);
    thrust::copy(statCuda::d_vec->begin(), statCuda::d_vec->end(), arrUint);
    out.median = arrUint[sizeUint / 2];
    out.min_value = arrUint[0];
    out.max_value = arrUint[sizeUint - 1];

    return out;
}
}
