// Copyright (c) 2020 - 2022. Eritque arcus and contributors.
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

#ifndef MIRAICP_PRO_EXCEPTION_H
#define MIRAICP_PRO_EXCEPTION_H

#include "MiraiCPMacros.h"
// -----------------------
#include <exception>
#include <string>
#include <thread>


namespace MiraiCP {
    /// @brief 总异常抽象类，用于一般捕获，不要直接抛出该类，不知道抛出什么的时候请抛出 MiraiCPException
    /// @interface MiraiCPExceptionBase
    class MIRAICP_EXPORT MiraiCPExceptionBase : public ::std::exception {
    protected:
        using string = std::string;

    protected:
        /// @brief 异常内容
        string re;

    public:
        /// @brief 发生异常的文件名
        string filename;

        /// @brief 发生异常的行号
        int lineNum = 0;

    protected:
        /// 受保护构造函数，供子类调用
        MiraiCPExceptionBase(string info, string _filename, int _lineNum) : re(std::move(info)), filename(std::move(_filename)), lineNum(_lineNum) {}

    public:
        ~MiraiCPExceptionBase() override = default;

    public:
        /// 异常信息
        const char *what() const noexcept override { return re.c_str(); }

        /// 返回std::string的异常信息
        string getError() const { return re; }

        /// 实际抛出方法
        void raise() const;

    public: // 暴露的接口
        /// basicRaise 基本抛出方法，子类重写该方法
        virtual void basicRaise() const;

        // CRTP实现一次，调用静态的exceptionType
        /// 获取异常类型，通用接口
        virtual string getExceptionType() const = 0;

        // 每个子类需要单独实现该静态方法
        /// 返回异常的类型，该静态方法无法正确实现多态，请使用 getExceptionType
        /// @see getExceptionType
        static string exceptionType() { return "MiraiCPException"; }
    };

    /// @brief 总异常CRTP抽象类，不要直接抛出该类，不知道抛出什么的时候请抛出 MiraiCPException。
    /// 该类是用于继承的基类，需要新的异常类型时，继承该类并以子类作为模板参数。
    /// 子类需要实现的方法：
    /// 1. 构造函数，要求必须委托MiraiCPExceptionCRTP构造。
    /// 2. `static std::string exceptionType()` 返回一个字符串表示异常类型。
    /// 继承该类后异常类能正确实现多态。
    /// @interface MiraiCPExceptionCRTP
    /// @note 请勿给该类增加新的属性。如果要增加属性应在 MiraiCPExceptionBase 中增加
    template<typename T>
    class MIRAICP_EXPORT MiraiCPExceptionCRTP : public MiraiCPExceptionBase {
    public:
        /// 委托构造函数
        explicit MiraiCPExceptionCRTP(string _re, string _filename, int _lineNum) : MiraiCPExceptionBase(std::move(_re), std::move(_filename), _lineNum) {
        }

    public:
        // CRTP类型获取实现
        string getExceptionType() const override { return T::exceptionType(); }
    };

    /// @brief 通用MiraiCP异常
    /// @param const string &description, string _filename, int _lineNum
    /// @see MiraiCPExceptionBase
    class MIRAICP_EXPORT MiraiCPException : public MiraiCPExceptionCRTP<MiraiCPExceptionBase> {};
} // namespace MiraiCP

#endif //MIRAICP_PRO_EXCEPTION_H
