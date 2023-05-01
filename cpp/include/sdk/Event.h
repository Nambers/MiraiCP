// Copyright (c) 2020 - 2023. Eritque arcus and contributors.
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

#ifndef MIRAICP_PRO_EVENT_H
#define MIRAICP_PRO_EVENT_H


#include "MiraiCPMacros.h"
// -----------------------
#include "Events/EventBase.h"
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <vector>


namespace MiraiCP {

    class MiraiCPEvent;
    class FriendRecallEvent;
    class MemberRecallEvent;

    /// 撤回信息
    namespace RecallEvent {
        // deprecated
        using FriendRecallEvent = MiraiCP::FriendRecallEvent;
        // deprecated
        using MemberRecallEvent = MiraiCP::MemberRecallEvent;
    }; // namespace RecallEvent


    /// 事件监听操控, 可用于stop停止监听和resume继续监听
    class MIRAICP_EXPORT NodeHandle {
        struct NodeHandleInternal;

    private:
        NodeHandleInternal *handle;

    public:
        explicit NodeHandle(bool a);
        NodeHandle(NodeHandle &&) = delete;
        NodeHandle(const NodeHandle &) = delete;
        ~NodeHandle();
        [[nodiscard]] bool isEnable() const;
        void stop();
        void resume();
    };

    class MIRAICP_EXPORT Event {
    private: // typedefs
        class eventNode {
        public:
            std::function<bool(MiraiCPEvent *)> func;

        private:
            /// 回调的handle，用于管理
            NodeHandle _handle;

        public:
            eventNode();

            explicit eventNode(std::function<bool(MiraiCPEvent *)> f);

        public:
            /// 返回true代表block之后的回调
            bool run(MiraiCPEvent *a) const;

            NodeHandle *getHandle() {
                return &_handle;
            }
        };

        using priority_level = unsigned char;
        using event_vector = std::vector<std::unique_ptr<eventNode>>;
        using eventNodeTable = std::vector<std::map<priority_level, event_vector>>;

    private: // member
        eventNodeTable _all_events_;
        mutable std::shared_mutex eventsMtx;

    private:
        Event();

    public: // singleton mode
        static Event processor;

    private:
        template<typename EventClass>
        constexpr static size_t id() {
            static_assert(std::is_base_of_v<MiraiCPEvent, EventClass>, "只支持广播继承MiraiCPEvent的事件");
            return static_cast<size_t>(EventClass::get_event_type());
        }

        bool internalNoRegistered(int index) const;

        void internalClear() noexcept;

        void internalBroadcast(MiraiCPEvent *event, size_t eventId) const;

        NodeHandle *internalRegister(std::function<bool(MiraiCPEvent *)> callback, size_t where, priority_level level);

    public:
        static bool noRegistered(int index) {
            return processor.internalNoRegistered(index);
        }

        /// 清空全部配置
        static void clear() noexcept {
            processor.internalClear();
        }

        static void incomingEvent(BaseEventData j, int type);

        /// 广播一个事件, 必须为MiraiCPEvent的派生类
        template<typename EventClass>
        static void broadcast(EventClass &&val) {
            using UnderlyingClass = std::decay_t<EventClass>;
            static_assert(std::is_base_of_v<MiraiCPEvent, UnderlyingClass>, "只支持广播MiraiCPEvent的派生类");
            MiraiCPEvent *p = &val;
            processor.internalBroadcast(p, id<UnderlyingClass>());
        }

        /**
         * @brief 注册一个事件的回调
         * @param T 事件类型
         * @param callback 要注册的回调函数，忽略返回值
         * @param priority_level 优先级，范围：0-255，越低的优先级越先执行，默认100
         * @doxygenEg{1018, callbackHandle.cpp, NodeHandle使用}
         */
        template<typename EventClass, typename = std::enable_if_t<std::is_base_of_v<MiraiCPEvent, EventClass>>>
        static NodeHandle *registerEvent(std::function<void(EventClass)> callback, priority_level level = 100) {
            std::function<bool(MiraiCPEvent *)> tmp = [=](MiraiCPEvent *p) {
                callback(*static_cast<EventClass *>(p));
                return false;
            };
            return processor.internalRegister(std::move(tmp), id<EventClass>(), level);
        }

        /**
         * @brief 注册一个可以阻塞后续回调函数的回调。
         * 回调返回true时，将会忽略所有优先级低于当前回调，以及注册顺序晚于当前回调且优先级等于当前回调的所有其他回调函数
         * @param T 事件类型
         * @param callback 要注册的回调函数，必须返回bool值
         * @param priority_level 优先级，范围：0-255，越低的优先级越先执行，默认100
         * @doxygenEg{1019, callbackHandle.cpp, NodeHandle使用}
         */
        template<typename EventClass, typename = std::enable_if_t<std::is_base_of_v<MiraiCPEvent, EventClass>>>
        static NodeHandle *registerBlockingEvent(std::function<bool(EventClass)> callback, priority_level level = 100) {
            std::function<bool(MiraiCPEvent *)> tmp = [=](MiraiCPEvent *p) {
                return callback(*static_cast<EventClass *>(p));
            };
            return processor.internalRegister(std::move(tmp), id<EventClass>(), level);
        }
    };
} // namespace MiraiCP


#endif //MIRAICP_PRO_EVENT_H
