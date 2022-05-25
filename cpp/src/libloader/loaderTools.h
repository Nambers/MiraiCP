//
// Created by antares on 5/25/22.
//

#ifndef MIRAICP_PRO_LOADERTOOLS_H
#define MIRAICP_PRO_LOADERTOOLS_H

#include <jni.h>
#include <json.hpp>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

namespace LibLoader {
    std::vector<std::string> collect_plugins(const std::string &cfgPath, nlohmann::json j);

    std::string jstring2str(jstring jStr);

    // multi thread controller
    class MiraiCPThreadsController {
        std::vector<std::unique_ptr<std::thread>> thread_ptr;
        std::recursive_mutex _mtx;

    private:
        MiraiCPThreadsController() = default;

        ~MiraiCPThreadsController() {
            end_all_thread();
        }

    private:
        static MiraiCPThreadsController _threadController;

    public:
        void end_all_thread();

    public:
        static MiraiCPThreadsController &getController() {
            return _threadController;
        }
    };
} // namespace LibLoader

#endif //MIRAICP_PRO_LOADERTOOLS_H
