//
// Created by Antares on 2023-1-25.
//

#include <android/api-level.h>
#include <cstdio>
int main() {
    std::printf("%d", android_get_device_api_level());
    return 0;
}
