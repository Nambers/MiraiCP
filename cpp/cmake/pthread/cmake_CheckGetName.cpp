//
// Created by 60168 on 2023-1-25.
//
#include "pthread.h"

int main(){
#ifndef _POSIX_THREADS
    exit(1);
    return 0;
#else
    char buffer[128];
    pthread_getname_np(pthread_self(), buffer, 127);
    return 0;
#endif
}