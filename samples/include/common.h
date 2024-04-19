#ifndef SAMPLES_COMMON_H
#define SAMPLES_COMMON_H

#include <cstdlib>
#include <cmath>
#include <sys/time.h>
#include <CL/sycl.hpp>

// maximum absolute error
const double eps = 1e-6;

// get current time in microseconds
long long getTime() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec * 1000000) + tv.tv_usec;
}

// initialize float array with random values
template <typename T>
typename std::enable_if<std::is_floating_point<T>::value, void>::type
initArray(std::vector<T> &array, T min = -1, T max = 1) {
    srand(time(nullptr));
    for (auto &i : array) {
        i = rand() / (float)RAND_MAX;
        i = min + i * (max - min);
    }
}

// initialize int array with random values
template <typename T>
typename std::enable_if<std::is_integral<T>::value, void>::type
initArray(std::vector<T> &array, T min = -3, T max = 3) {
    srand(time(nullptr));
    for (auto &i : array) {
        i = rand() % (max - min + 1) + min;
    }
}

bool isClose(double a, double b, double rel_tol = 1e-6, double abs_tol = 1e-6) {
    return std::abs(a - b) <= std::max(rel_tol * std::max(std::abs(a), std::abs(b)), abs_tol);
}

// compare two float arrays
template <typename T>
typename std::enable_if<std::is_floating_point<T>::value, int>::type
compareResult(const std::vector<T> &array1, const std::vector<T> &array2, double rel_tol = 1e-6, double abs_tol = 1e-6) {
    size_t size = array1.size();
    for (size_t i = 0; i < size; i++) {
        if (!isClose(array1[i], array2[i], rel_tol, abs_tol)) {
            std::cout << "Mismatch at index " << i << ": " << array1[i] << " != " << array2[i] << std::endl;
            return 1;
        }
    }
    return 0;
}

// compare two int arrays
template <typename T>
typename std::enable_if<std::is_integral<T>::value, int>::type
compareResult(const std::vector<T> &array1, const std::vector<T> &array2) {
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