#include "ThreadManager.h"
#include "Logger.h"

namespace MiraiCP {
    // 静态成员

    std::map<std::string, ThreadManager::ThreadInfo> ThreadManager::threads = std::map<std::string, ThreadInfo>();
    std::recursive_mutex ThreadManager::mtx = std::recursive_mutex();
    JavaVM *ThreadManager::gvm = nullptr;
    long ThreadManager::JNIVersion = 0;

    // 结束静态成员

    void ThreadManager::setEnv(JNIEnv *e) {
        mtx.lock();
        if (!ThreadManager::included(ThreadManager::getThreadId())) {
            ThreadInfo tmp{e, false};
            ThreadManager::threads.insert(std::pair<std::string, ThreadInfo>(ThreadManager::getThreadId(), tmp));
        } else {
            ThreadManager::threads[ThreadManager::getThreadId()].e = e;
        }
        mtx.unlock();
    }

    void ThreadManager::newEnv(const char *threadName) {
        JNIEnv *env = nullptr;
        JavaVMAttachArgs args{JNIVersion,
                              const_cast<char *>(threadName),
                              nullptr};
        gvm->AttachCurrentThread((void **) &env, &args);
        ThreadInfo tmp{env, true};
        ThreadManager::threads.insert(std::pair<std::string, ThreadInfo>(ThreadManager::getThreadId(), tmp));
        Logger::logger.info("refresh env");
    };

    void ThreadManager::detach() {
        mtx.lock();
        if (ThreadManager::included(ThreadManager::getThreadId())) {
            bool att = ThreadManager::threads[ThreadManager::getThreadId()].attach;
            ThreadManager::threads.erase(ThreadManager::getThreadId());
            if (att)
                gvm->DetachCurrentThread();
        }
        mtx.unlock();
    }

    bool ThreadManager::included(const std::string &id) {
        if (ThreadManager::threads.empty() || ThreadManager::threads.count(id) == 0)
            return false;
        return true;
    }

    JNIEnv *ThreadManager::getEnv() {
        mtx.lock();
        if (!ThreadManager::included(getThreadId())) {
            ThreadManager::newEnv();
        }
        JNIEnv *tmp = ThreadManager::threads[ThreadManager::getThreadId()].e;
        mtx.unlock();
        return tmp;
    }
} // namespace MiraiCP
