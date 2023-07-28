project(kalnytskyi_solution LANGUAGES CXX CUDA)
find_package(CUDA  REQUIRED)
include_directories("${CUDA_INCLUDE_DIRS}")
set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} -arch=sm_86")
set(CMAKE_CUDA_STANDARD 14)
find_package(Thrust REQUIRED CONFIG)
thrust_create_target(Thrust)
add_library(${PROJECT_NAME} SHARED sort.cuh sort.cu kalnytskyi_solution.cpp )
target_link_libraries(${PROJECT_NAME} Thrust)



set_target_properties(
        ${PROJECT_NAME}
        PROPERTIES
        CUDA_SEPARABLE_COMPILATION ON)
