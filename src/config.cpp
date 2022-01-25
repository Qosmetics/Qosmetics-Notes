#include "config.hpp"

#define GET_STRING(identifier)                                                                                                \
    auto identifier##Itr = member.FindMember(#identifier);                                                                    \
    if (identifier##Itr != member.MemberEnd() && identifier##Itr->value.IsString())                                           \
    {                                                                                                                         \
        actual_config.identifier = std::string(identifier##Itr->value.GetString(), identifier##Itr->value.GetStringLength()); \
    }                                                                                                                         \
    else                                                                                                                      \
        foundEverything = false;

namespace Qosmetics::Notes
{
    Config actual_config;
    Config& Qosmetics::Notes::Config::config = actual_config;

    void NoteConfigRegistration::SaveConfig(rapidjson::Value& member, rapidjson::Document::AllocatorType& allocator) const
    {
        member.AddMember("lastUsedCyoob", actual_config.lastUsedCyoob, allocator);
    }

    bool NoteConfigRegistration::LoadConfig(const rapidjson::Value& member) const
    {
        bool foundEverything = true;
        GET_STRING(lastUsedCyoob);
        return foundEverything;
    }

    void NoteConfigRegistration::OnProfileSwitched() const
    {
    }

    QOSMETICS_CONFIG_REGISTER(NoteConfigRegistration, "cyoobConfig");
}
