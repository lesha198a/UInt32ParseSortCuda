#pragma once
#include <stdint.h>
#include <stdlib.h>

#define TEST_TYPE uint32_t

struct TaskInputData
{
    const char *data;
    size_t data_size;
};

struct TaskOutputData
{
    TEST_TYPE average;
    TEST_TYPE median;
    TEST_TYPE max_value;
    TEST_TYPE min_value;
};

#define COMPUTE_TASK_PROC_NAME "ComputeTask"

#ifdef WIN32
#define DllExport __declspec(dllexport)
#define CallConv __cdecl
typedef DllExport TaskOutputData(CallConv *ComputeTask_proc)(TaskInputData);
#else
#define DllExport
#define CallConv
typedef TaskOutputData (*ComputeTask_proc)(TaskInputData);
#endif
