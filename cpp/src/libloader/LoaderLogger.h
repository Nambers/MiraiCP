//
// Created by antares on 5/25/22.
//

#ifndef MIRAICP_PRO_LOADERLOGGER_H
#define MIRAICP_PRO_LOADERLOGGER_H


#include <jni.h>
#include <string>


class LoaderLogger {
    using string = std::string;

private:
    jmethodID log = nullptr;

public:
    void init();
    void info(string);
    void warning(string);
    void error(string);

private:
    void call_logger(string);
};


#endif //MIRAICP_PRO_LOADERLOGGER_H
