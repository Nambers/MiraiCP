//
// Created by Antares on 2023-1-25.
//
#include "pthread.h"

int main(){
#ifndef _POSIX_THREADS
    exit(1);
    return 0;
#else
    pthread_setname_np(pthread_self(), "test");
    return 0;
#endif
}