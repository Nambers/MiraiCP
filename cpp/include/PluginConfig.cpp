#include "PluginConfig.h"

namespace MiraiCP {
    json PluginConfig::serialize() {
        json j;
        j["name"] = name;
        j["version"] = version;
        j["author"] = author;
        j["description"] = description;
        j["time"] = time;
        j["id"] = id;
        return j;
    }
} // namespace MiraiCP
