// Copyright (c) 2022 - 2022. Antares, Eritque arcus and contributors.
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

#ifndef MIRAICP_PRO_LOADEREXCEPTIONS_H
#define MIRAICP_PRO_LOADEREXCEPTIONS_H


#include "LoaderLogger.h"
#include <exception>
#include <string>


namespace LibLoader {
    class LoaderBaseException : public ::std::exception {
    protected:
        using string = std::string;

    protected:
        /// @brief 异常内容
        string errorMessage;

    public:
        /// @brief 发生异常的文件名
        string filename;

        /// @brief 发生异常的行号
        int lineNum = 0;

    protected:
        /// 受保护构造函数，供子类调用
        LoaderBaseException(string info, string _filename, int _lineNum);

    public:
        ~LoaderBaseException() override = default;

    public:
        /// @brief 异常信息（std::exception接口）
        const char *what() const noexcept override { return errorMessage.c_str(); }

        virtual void raise() const noexcept;

    protected:
        void warningRaise() const noexcept;

    public: // 暴露的接口
        // CRTP实现一次，调用静态的exceptionType
        /// 获取异常类型，通用接口
        virtual string getExceptionType() const = 0;

        // 每个子类需要单独实现该静态方法
        /// @brief 返回异常的类型，该静态方法无法正确实现多态，请使用 getExceptionType
        /// @see getExceptionType
        static string exceptionType() { return "LoaderBaseException"; }
    };

    /// @brief 总异常CRTP抽象类，不要直接抛出该类，不知道抛出什么的时候请抛出 LoaderException。
    /// 该类是用于继承的基类，需要新的异常类型时，继承该类并以子类作为模板参数。
    /// 子类需要实现的方法：
    /// 1. 构造函数，要求必须委托 LoaderExceptionCRTP 构造。
    /// 2. `static std::string exceptionType()` 返回一个字符串表示异常类型。
    /// 继承该类后异常类能正确实现多态。
    /// @interface LoaderExceptionCRTP
    /// @note 请勿给该类增加新的属性。如果要增加属性应在 LoaderBaseException 中增加
    template<class T>
    class LoaderExceptionCRTP : public LoaderBaseException {
    public:
        /// 委托构造函数
        explicit LoaderExceptionCRTP(std::string _re, string _filename, int _lineNum)
            : LoaderBaseException(std::move(_re), std::move(_filename), _lineNum) {}

    public:
        // CRTP类型获取实现
        string getExceptionType() const override { return T::exceptionType(); }
    };

    typedef LoaderExceptionCRTP<LoaderBaseException> LoaderException;

    class PluginLoadException : public LoaderExceptionCRTP<PluginLoadException> {
    public:
        PluginLoadException(const string &_pluginPath, string _filename, int _lineNum);

        static string exceptionType() { return "PluginLoadException"; }
    };


    class PluginAlreadyLoadedException : public LoaderExceptionCRTP<PluginAlreadyLoadedException> {
    public:
        PluginAlreadyLoadedException(const string &id, string _filename, int _lineNum);

        static string exceptionType() { return "PluginAlreadyLoadedException"; }

        void raise() const noexcept override;
    };

    class PluginAlreadyEnabledException : public LoaderExceptionCRTP<PluginAlreadyEnabledException> {
    public:
        PluginAlreadyEnabledException(const string &id, string _filename, int _lineNum);

        static string exceptionType() { return "PluginAlreadyEnabledException"; }

        void raise() const noexcept override;
    };

    class PluginNotLoadedException : public LoaderExceptionCRTP<PluginNotLoadedException> {
    public:
        PluginNotLoadedException(const string &path, string _filename, int _lineNum);

        static string exceptionType() { return "PluginNotLoadedException"; }
    };

    class PluginIdDuplicateException : public LoaderExceptionCRTP<PluginIdDuplicateException> {
    public:
        PluginIdDuplicateException(const string &id, const string &conflictPluginPathOld,
                                   const string &conflictPluginPathNew, string _filename, int _lineNum);

        static string exceptionType() { return "PluginIdDuplicateException"; }
    };
} // namespace LibLoader

#endif //MIRAICP_PRO_LOADEREXCEPTIONS_H