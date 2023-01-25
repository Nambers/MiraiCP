//
// Created by Antares on 2023-1-25.
//
#include "pthread.h"

int main() {
    pthread_setname_np(pthread_self(), "test");
    return 0;
}
