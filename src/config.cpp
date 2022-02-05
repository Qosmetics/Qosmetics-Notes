#include "config.hpp"
#include "logging.hpp"

#define GET_STRING(identifier)                                                                                                \
    auto identifier##Itr = member.FindMember(#identifier);                                                                    \
    if (identifier##Itr != member.MemberEnd() && identifier##Itr->value.IsString())                                           \
    {                                                                                                                         \
        actual_config.identifier = std::string(identifier##Itr->value.GetString(), identifier##Itr->value.GetStringLength()); \
    }                                                                                                                         \
    else                                                                                                                      \
        foundEverything = false;

#define SET_STRING(identifier)                                                 \
    auto identifier##Itr = member.FindMember(#identifier);                     \
    if (identifier##Itr != member.MemberEnd())                                 \
    {                                                                          \
        identifier##Itr->value.SetString(actual_config.identifier, allocator); \
    }                                                                          \
    else                                                                       \
    {                                                                          \
        member.AddMember(#identifier, actual_config.identifier, allocator);    \
    }
namespace Qosmetics::Notes
{
    Config actual_config;

    Config& Config::get_config() { return actual_config; }
    void NoteConfigRegistration::SaveConfig(rapidjson::Value& member, rapidjson::Document::AllocatorType& allocator) const
    {
        INFO("Saving config...");
        SET_STRING(lastUsedCyoob);
        INFO("Config Saved!");
    }

    bool NoteConfigRegistration::LoadConfig(const rapidjson::Value& member) const
    {
        bool foundEverything = true;
        INFO("Loading config...");
        GET_STRING(lastUsedCyoob);
        if (foundEverything)
            INFO("Config Loaded!");
        return foundEverything;
    }

    void NoteConfigRegistration::OnProfileSwitched() const
    {
    }

    QOSMETICS_CONFIG_REGISTER(NoteConfigRegistration, "cyoobConfig");

    /*
    struct config_registration_NoteConfigRegistration : public NoteConfigRegistration
    {
        config_registration_NoteConfigRegistration() : NoteConfigRegistration("cyoobConfig")
        {
            Qosmetics::Core::Config::Register(this);
        };
    };
    static config_registration_NoteConfigRegistration config_registration_NoteConfigRegistration_Instance = config_registration_NoteConfigRegistration();
    */
}
