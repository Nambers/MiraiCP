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
    jmethodID logMethod = nullptr;

public:
    void init();
    void info(const string &);
    void warning(const string &);
    void error(const string &);

private:
    void call_logger(string content, string name, int id, int level);
};


#endif //MIRAICP_PRO_LOADERLOGGER_H
