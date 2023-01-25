//
// Created by 60168 on 2023-1-26.
//
#include "pthread.h"
#include <type_traits>

int main() {
#ifndef _POSIX_THREADS
    exit(1);
#else
    static_assert(std::is_same_v<int, decltype(pthread_cancel(std::declval<pthread_t>()))>);
#endif
    return 0;
}
