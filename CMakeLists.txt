cmake_minimum_required(VERSION 3.27)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(PROJECT_NAME generational_array_list)

project(${PROJECT_NAME})
set(LIBRARY_NAME generational_array_list)

include(FetchContent)

FetchContent_Declare(
    abseil
    URL https://github.com/abseil/abseil-cpp/releases/download/20250814.1/abseil-cpp-20250814.1.tar.gz
    URL_HASH SHA256=1692f77d1739bacf3f94337188b78583cf09bab7e420d2dc6c5605a4f86785a1
)

FetchContent_MakeAvailable(abseil)

cmake_policy(SET CMP0135 NEW)

add_library(${LIBRARY_NAME} STATIC
    array_list_allocator.hpp
)
add_library(${PROJECT_NAME}::${LIBRARY_NAME} ALIAS ${LIBRARY_NAME})

target_link_libraries(${LIBRARY_NAME} PRIVATE
    absl::btree
)

add_executable(test test.cpp)

target_link_libraries(test PRIVATE
    ${LIBRARY_NAME}
)
