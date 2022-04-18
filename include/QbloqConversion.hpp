#pragma once
#include "beatsaber-hook/shared/config/rapidjson-utils.hpp"
#include "custom-types/shared/coroutine.hpp"
#include <string>
#include <vector>

namespace Qosmetics::Notes::QbloqConversion
{

    struct LegacyDescriptor
    {
        std::string authorName;
        std::string objectName;
        std::string description;

        LegacyDescriptor(const rapidjson::Document& doc)
        {
            authorName = doc["authorName"].GetString();
            objectName = doc["objectName"].GetString();
            description = doc["description"].GetString();
        }
    };

    struct LegacyConfig
    {
        bool disableBaseGameArrows;
        bool hasBomb;
        bool hasDebris;
        LegacyConfig(const rapidjson::Document& doc)
        {
            disableBaseGameArrows = doc["disableBaseGameArrows"].GetBool();
            hasBomb = doc["hasBomb"].GetBool();
            hasDebris = doc["hasDebris"].GetBool();
        }
    };

    void ConvertOldQbloqs(std::function<void()> onFinished = nullptr);

    std::vector<std::string> GetQbloqFiles();
    std::vector<std::pair<std::string, std::string>> GetNonConverted(std::vector<std::string> filePaths);
    custom_types::Helpers::Coroutine ConvertAllFoundQbloqs(std::vector<std::pair<std::string, std::string>> oldNewPathPairs, std::function<void()> onFinished = nullptr);
}
