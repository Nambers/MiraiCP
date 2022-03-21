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

#include "MessageSource.h"
#include "Config.h"
#include "Exception.h"
#include "Logger.h"

namespace MiraiCP {
    using json = nlohmann::json;
    void MessageSource::recall(JNIEnv *env) const {
        json j;
        j["source"] = this->serializeToString();
        std::string re = Config::koperation(Config::Recall, j, env);
        if (re == "Y") return;
        if (re == "E2") throw RecallException(MIRAICP_EXCEPTION_WHERE);
    }

    MessageSource::MessageSource(std::string ids,
                                 std::string internalids,
                                 std::string source)
        : ids(std::move(ids)),
          internalids(std::move(internalids)),
          source(std::move(source)) {}

    std::string MessageSource::serializeToString() const {
        return source;
    }

    MessageSource MessageSource::deserializeFromString(const std::string &source) {
        json j = json::parse(source);
        try {
            return {j["ids"].dump(), j["internalIds"].dump(), source};
        } catch (json::type_error &e) {
            Logger::logger.error("消息源序列化出错，格式不符合(MessageSource::deserializeFromString)");
            Logger::logger.error(source);
            Logger::logger.error(e.what());
            throw IllegalArgumentException(std::string("消息源序列化出错，格式不符合(MessageSource::deserializeFromString), ") + e.what(), MIRAICP_EXCEPTION_WHERE);
        }
    }
} // namespace MiraiCP
