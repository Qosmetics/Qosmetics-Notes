#include "config.hpp"
#include "logging.hpp"

#define GET_JSON_STRING(identifier)                                                                                           \
    auto identifier##Itr = member.FindMember(#identifier);                                                                    \
    if (identifier##Itr != member.MemberEnd() && identifier##Itr->value.IsString())                                           \
    {                                                                                                                         \
        actual_config.identifier = std::string(identifier##Itr->value.GetString(), identifier##Itr->value.GetStringLength()); \
    }                                                                                                                         \
    else                                                                                                                      \
        foundEverything = false;

#define SET_JSON_STRING(identifier)                                            \
    auto identifier##Itr = member.FindMember(#identifier);                     \
    if (identifier##Itr != member.MemberEnd())                                 \
    {                                                                          \
        identifier##Itr->value.SetString(actual_config.identifier, allocator); \
    }                                                                          \
    else                                                                       \
    {                                                                          \
        member.AddMember(#identifier, actual_config.identifier, allocator);    \
    }

#define GET_JSON_BOOL(identifier)                                                 \
    auto identifier##Itr = member.FindMember(#identifier);                        \
    if (identifier##Itr != member.MemberEnd() && identifier##Itr->value.IsBool()) \
    {                                                                             \
        actual_config.identifier = identifier##Itr->value.GetBool();              \
    }                                                                             \
    else                                                                          \
        foundEverything = false;

#define SET_JSON_BOOL(identifier)                                           \
    auto identifier##Itr = member.FindMember(#identifier);                  \
    if (identifier##Itr != member.MemberEnd())                              \
    {                                                                       \
        identifier##Itr->value.SetBool(actual_config.identifier);           \
    }                                                                       \
    else                                                                    \
    {                                                                       \
        member.AddMember(#identifier, actual_config.identifier, allocator); \
    }

#define GET_JSON_DOUBLE(identifier)                                                 \
    auto identifier##Itr = member.FindMember(#identifier);                          \
    if (identifier##Itr != member.MemberEnd() && identifier##Itr->value.IsDouble()) \
    {                                                                               \
        actual_config.identifier = identifier##Itr->value.GetDouble();              \
    }                                                                               \
    else                                                                            \
        foundEverything = false;

#define SET_JSON_DOUBLE(identifier)                                         \
    auto identifier##Itr = member.FindMember(#identifier);                  \
    if (identifier##Itr != member.MemberEnd())                              \
    {                                                                       \
        identifier##Itr->value.SetDouble(actual_config.identifier);         \
    }                                                                       \
    else                                                                    \
    {                                                                       \
        member.AddMember(#identifier, actual_config.identifier, allocator); \
    }

namespace Qosmetics::Notes
{
    Config actual_config;

    Config& Config::get_config() { return actual_config; }
    void NoteConfigRegistration::SaveConfig(rapidjson::Value& member, rapidjson::Document::AllocatorType& allocator) const
    {
        INFO("Saving config...");
        SET_JSON_STRING(lastUsedCyoob);
        SET_JSON_BOOL(overrideNoteSize);
        SET_JSON_DOUBLE(noteSize);
        SET_JSON_BOOL(alsoChangeHitboxes);
        SET_JSON_BOOL(forceDefaultBombs);
        SET_JSON_BOOL(forceDefaultDebris);
        SET_JSON_BOOL(disableReflections);
        SET_JSON_BOOL(disabled);
        INFO("Config Saved!");
    }

    bool NoteConfigRegistration::LoadConfig(const rapidjson::Value& member) const
    {
        bool foundEverything = true;
        INFO("Loading config...");
        GET_JSON_STRING(lastUsedCyoob);
        GET_JSON_BOOL(overrideNoteSize);
        GET_JSON_DOUBLE(noteSize)
        GET_JSON_BOOL(alsoChangeHitboxes);
        GET_JSON_BOOL(forceDefaultBombs);
        GET_JSON_BOOL(forceDefaultDebris);
        GET_JSON_BOOL(disableReflections);
        GET_JSON_BOOL(disabled);
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
