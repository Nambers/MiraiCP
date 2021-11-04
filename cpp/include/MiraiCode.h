// Copyright (c) 2021-2021. Eritque arcus and contributors.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or any later version(in your opinion).
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#ifndef MIRAICP_PRO_MIRAICODE_H
#define MIRAICP_PRO_MIRAICODE_H

#include "Tools.h"
#include <string>

namespace MiraiCP {
    /// MiraiCode父类, 指可以被转换成miraicode的类型
    class MiraiCodeable {
    public:
        virtual std::string toMiraiCode() const = 0;
    };

    /// @brief miraicode字符串
    /// @attention MiraiCode会把非miraicode组成部分(非[mirai:])转码, 输出转码前的文本用toString, 参考: https://github.com/mamoe/mirai/blob/dev/docs/Messages.md#%E8%BD%AC%E4%B9%89%E8%A7%84%E5%88%99
    /// @detail 为了便捷使用，构造函数不以explicit注释
    class MiraiCode : public MiraiCodeable {
    private:
        std::string content;

    public:
        /// 输出当前内容, 会自动转码
        std::string toString() {
            return Tools::escapeFromMiraiCode(this->content);
        }

        /// 和toString作用一样, 不过不会自动转码
        std::string toMiraiCode() const override {
            return content;
        }

        /// 从MiraiCodeable类型初始化一个miraicode字符串
        MiraiCode(MiraiCodeable *a) { // NOLINT(google-explicit-constructor)
            content = a->toMiraiCode();
        }

        /// 从文本初始化一个miraicode字符串, 根据第二个参数决定是否转码, 默认不转码
        /// @attention 如果是传入文本MiraiCode，请勿转码，转码只是为了[mirai:xxx:<应该转码的部分>], 如果<应该转码>的部分里面含有'[]:,'内容，请调用Tools::escapeToMiraiCode转码
        MiraiCode(const std::string &a, bool convert = false) { // NOLINT(google-explicit-constructor)
            if (!convert)
                content = a;
            else
                content = Tools::escapeToMiraiCode(a);
        }

        MiraiCode operator+(MiraiCodeable *a) {
            return {content + a->toMiraiCode()};
        }

        MiraiCode operator+(const std::string &a) {
            return {content + a};
        }

        MiraiCode operator+(const MiraiCode &a) {
            return {content + a.content};
        }

        MiraiCode operator+(MiraiCode *a) {
            return {content + a->content};
        }

        MiraiCode &operator=(const std::string &a) {
            this->content = a;
            return *this;
        }

        MiraiCode plus(MiraiCodeable *a) {
            return {content + a->toMiraiCode()};
        }

        MiraiCode plus(const std::string &a) {
            return MiraiCode(a) + this;
        }

        /// 不执行转义，适用于已经被MiraiCode转义过的字符串
        static MiraiCode MiraiCodeWithoutEscape(const std::string &a) {
            return {a, false};
        }

        /// 不执行转义，因为MiraiCodeable的toMiraiCode已经转义过了
        static MiraiCode MiraiCodeWithoutEscape(MiraiCodeable *a) {
            return {a->toMiraiCode(), false};
        }
    };
} // namespace MiraiCP

#endif //MIRAICP_PRO_MIRAICODE_H
