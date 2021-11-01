#include "Member.h"

namespace MiraiCP {
    /*成员类实现*/
    Member::Member(QQID id, QQID groupid, QQID botid, JNIEnv *env)
        : Contact() {
        this->_type = 3;
        this->_id = id;
        this->_groupid = groupid;
        this->_botid = botid;
        refreshInfo(env);
    }

    unsigned int Member::getPermission(JNIEnv *env) {
        if (isAnonymous) return 0;
        json j;
        j["contactSource"] = this->serializationToString();
        std::string re = Config::koperation(Config::QueryM, j, env);
        return stoi(re);
    }

    void Member::mute(int time, JNIEnv *env) {
        json j;
        j["time"] = time;
        j["contactSource"] = this->serializationToString();
        std::string re = Config::koperation(Config::MuteM, j, env);
        if (re == "E3") {
            throw BotException();
        }
        if (re == "E4") {
            throw MuteException();
        }
    }

    void Member::kick(const std::string &reason, JNIEnv *env) {
        json j;
        j["message"] = reason;
        j["contactSource"] = this->serializationToString();
        std::string re = Config::koperation(Config::KickM, j, env);
        if (re == "E3") {
            throw BotException();
        }
    }

    void Member::modifyAdmin(bool admin, JNIEnv *env) {
        if (isAnonymous) return;
        json j;
        j["admin"] = admin;
        j["contactSource"] = this->serializationToString();
        std::string re = Config::koperation(Config::ModifyAdmin, j, env);
        if (re == "E1") {
            throw BotException();
        }
    }

} // namespace MiraiCP