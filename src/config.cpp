#include "config.hpp"
#include "UI/PreviewViewController.hpp"
#include "UI/SettingsViewController.hpp"
#include "logging.hpp"
#include "static-defines.hpp"

#include "CustomTypes/NoteModelContainer.hpp"
#include "qosmetics-core/shared/Data/Manifest.hpp"

#include "UnityEngine/RectTransform.hpp"

#if __has_include("bs-utils/shared/utils.hpp")
#include "bs-utils/shared/utils.hpp"
#ifndef BS_UTILS_EXISTS
#define BS_UTILS_EXISTS
#endif
#endif

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

#define GET_JSON_BOOL_SETTER(identifier)                                          \
    auto identifier##Itr = member.FindMember(#identifier);                        \
    if (identifier##Itr != member.MemberEnd() && identifier##Itr->value.IsBool()) \
    {                                                                             \
        actual_config.set_##identifier(identifier##Itr->value.GetBool());         \
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

#define SET_JSON_BOOL_GETTER(identifier)                                            \
    auto identifier##Itr = member.FindMember(#identifier);                          \
    if (identifier##Itr != member.MemberEnd())                                      \
    {                                                                               \
        identifier##Itr->value.SetBool(actual_config.get_##identifier());           \
    }                                                                               \
    else                                                                            \
    {                                                                               \
        member.AddMember(#identifier, actual_config.get_##identifier(), allocator); \
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

extern ModInfo modInfo;
namespace Qosmetics::Notes
{
    Config actual_config;

    void Config::UpdateSubmission() const
    {
#ifdef BS_UTILS_EXISTS

        if (actual_config.get_overrideNoteSize() && actual_config.get_alsoChangeHitboxes())
            bs_utils::Submission::disable(modInfo);
        else
            bs_utils::Submission::enable(modInfo);
#else
        INFO("Mod was not compiled with bs utils, score submission might be enabled when it shouldn't!");
#endif
    }

    Config& Config::get_config() { return actual_config; }
    void NoteConfigRegistration::SaveConfig(rapidjson::Value& member, rapidjson::Document::AllocatorType& allocator) const
    {
        INFO("Saving config...");
        SET_JSON_STRING(lastUsedCyoob);
        SET_JSON_BOOL_GETTER(overrideNoteSize);
        SET_JSON_DOUBLE(noteSize);
        SET_JSON_BOOL_GETTER(alsoChangeHitboxes);
        SET_JSON_BOOL(forceDefaultBombs);
        SET_JSON_BOOL(forceDefaultChains);
        SET_JSON_BOOL(forceDefaultDebris);
        SET_JSON_BOOL(forceDefaultChainDebris);
        SET_JSON_BOOL(disableReflections);
        SET_JSON_BOOL(disabled);
        SET_JSON_BOOL(keepMissingReflections);
        INFO("Config Saved!");
    }

    bool NoteConfigRegistration::LoadConfig(const rapidjson::Value& member) const
    {
        bool foundEverything = true;
        INFO("Loading config...");
        GET_JSON_STRING(lastUsedCyoob);
        GET_JSON_BOOL_SETTER(overrideNoteSize);
        GET_JSON_DOUBLE(noteSize)
        GET_JSON_BOOL_SETTER(alsoChangeHitboxes);
        GET_JSON_BOOL(forceDefaultBombs);
        GET_JSON_BOOL(forceDefaultChains);
        GET_JSON_BOOL(forceDefaultDebris);
        GET_JSON_BOOL(forceDefaultChainDebris);
        GET_JSON_BOOL(disableReflections);
        GET_JSON_BOOL(disabled);
        GET_JSON_BOOL(keepMissingReflections);
        if (foundEverything)
            INFO("Config Loaded!");
        return foundEverything;
    }

    void NoteConfigRegistration::OnProfileSwitched() const
    {
        Qosmetics::Notes::PreviewViewController::justChangedProfile = true;
        Qosmetics::Notes::SettingsViewController::justChangedProfile = true;

        auto noteModelContainer = Qosmetics::Notes::NoteModelContainer::get_instance();
        if (actual_config.lastUsedCyoob == "" || actual_config.lastUsedCyoob == "Default")
        {
            noteModelContainer->Default();
            return;
        }

        std::string filePath = fmt::format("{}/{}.cyoob", cyoob_path, actual_config.lastUsedCyoob);
        if (!fileexists(filePath))
        {
            noteModelContainer->Default();
            return;
        }

        auto manifest = Qosmetics::Core::Manifest<Qosmetics::Notes::NoteObjectConfig>(filePath);
        noteModelContainer->LoadObject(manifest, nullptr);
    }

    QOSMETICS_CONFIG_REGISTER(NoteConfigRegistration, "cyoobConfig");
}
