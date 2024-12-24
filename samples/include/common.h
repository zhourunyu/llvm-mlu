#ifndef SAMPLES_COMMON_H
#define SAMPLES_COMMON_H

#include <cstdlib>
#include <cmath>
#include <sys/time.h>
#include <CL/sycl.hpp>

// get current time in microseconds
long long getTime() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec * 1000000) + tv.tv_usec;
}

// initialize float array with random values
template <typename T, size_t n>
typename std::enable_if_t<sycl::detail::is_sgenfloat<T>::value, void>
initArray(std::array<T, n> &array, T min = -1, T max = 1) {
    static int init = 0;
    if (!init) {
        srand(time(nullptr));
        init = 1;
    }
    for (auto &i : array) {
        i = rand() / (float)RAND_MAX;
        i = min + i * (max - min);
    }
}

// initialize int array with random values
template <typename T, size_t n>
typename std::enable_if_t<std::is_integral<T>::value, void>
initArray(std::array<T, n> &array, T min = -3, T max = 3) {
    static int init = 0;
    if (!init) {
        srand(time(nullptr));
        init = 1;
    }
    for (auto &i : array) {
        i = rand() % (max - min + 1) + min;
    }
}

bool isClose(double a, double b, double rel_tol = 1e-6, double abs_tol = 1e-6) {
    return std::abs(a - b) <= std::max(rel_tol * std::max(std::abs(a), std::abs(b)), abs_tol);
}

// compare two float arrays
template <typename T, size_t n>
typename std::enable_if_t<sycl::detail::is_sgenfloat<T>::value, int>
compareResult(const std::array<T, n> &array1, const std::array<T, n> &array2, double rel_tol = 1e-6, double abs_tol = 1e-6) {
    for (size_t i = 0; i < n; i++) {
        if (!isClose(array1[i], array2[i], rel_tol, abs_tol)) {
            std::cout << "Mismatch at index " << i << ": " << array1[i] << " != " << array2[i] << std::endl;
            return 1;
        }
    }
    return 0;
}

// compare two int arrays
template <typename T, size_t n>
typename std::enable_if_t<std::is_integral<T>::value, int>
compareResult(const std::array<T, n> &array1, const std::array<T, n> &array2) {
    size_t size = array1.size();
    for (size_t i = 0; i < size; i++) {
        if (array1[i] != array2[i]) {
            std::cout << "Mismatch at index " << i << ": " << array1[i] << " != " << array2[i] << std::endl;
            return 1;
        }
    }
    return 0;
}

#endif // SAMPLES_COMMON_H