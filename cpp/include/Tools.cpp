#include "Tools.h"
#include "Logger.h"
#include <utf8.h>
#include <regex>

namespace MiraiCP {
    /*工具类实现*/
    std::string Tools::jstring2str(jstring jStr, JNIEnv *env) {
        if (!jStr) {
            Logger::logger.error("警告:kotlin部分返回空字符串, 位置:Tools::jstring2str");
            return "";
        }
        std::u16string s = reinterpret_cast<const char16_t *>(env->GetStringChars(jStr, nullptr));
        if (s.length() == 0) {
            Logger::logger.error("警告:kotlin部分返回空字符串, 位置:Tools::jstring2str");
            return "";
        }
        std::string x;
        utf8::utf16to8(s.begin(), s.end(), std::back_inserter(x));
        return x;
    }

    jstring Tools::str2jstring(const char *stra, JNIEnv *env) {
        if (!stra) {
            Logger::logger.error("警告:C++部分传入空字符串，位置:Tools::str2jstring");
        }
        std::string str(stra);
        std::vector<unsigned short> utf16line;
        utf8::utf8to16(str.begin(), str.end(), std::back_inserter(utf16line));
        auto *c = new jchar[utf16line.size()];
        for (int i = 0; i < utf16line.size(); i++) {
            c[i] = utf16line[i];
        }
        return env->NewString((jchar *) c, (jsize) utf16line.size());
    }

    std::vector<unsigned long long> Tools::StringToVector(std::string temp) {
        std::vector<unsigned long long> result;
        temp.erase(temp.begin());
        temp.pop_back();
        std::regex ws_re("[,]+");
        std::vector<std::string> v(std::sregex_token_iterator(temp.begin(), temp.end(), ws_re, -1),
                                   std::sregex_token_iterator());
        result.reserve(v.size());
        for (auto &&s: v)
            result.push_back(std::stoull(s));
        return result;
    }

    std::string Tools::escapeFromMiraiCode(const std::string &s) {
        //[	\[
        //]	\]
        //:	\:
        //,	\,
        //\	\\ /
        return Tools::replace(
                Tools::replace(
                        Tools::replace(
                                Tools::replace(
                                        Tools::replace(s,
                                                       "\\\\", "\\"),
                                        "\\,", ","),
                                "\\:", ":"),
                        "\\]", "]"),
                "\\[", "[");
    }

    std::string Tools::escapeToMiraiCode(const std::string &s) {
        //[	\[
        //]	\]
        //:	\:
        //,	\,
        //\	\\ /
        return replace(replace(replace(replace(replace(s,
                                                       "\\", "\\\\"),
                                               ",", "\\,"),
                                       ":", "\\:"),
                               "]", "\\]"),
                       "[", "\\[");
    }
} // namespace MiraiCP