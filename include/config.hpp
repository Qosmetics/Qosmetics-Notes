#pragma once
#include "qosmetics-core/shared/ConfigRegister.hpp"

namespace Qosmetics::Notes
{
    struct Config
    {
        std::string lastUsedCyoob;
        bool overrideNoteSize = false;
        double noteSize = 1.0f;
        bool alsoChangeHitboxes = false;
        bool forceDefaultBombs = false;
        bool forceDefaultChains = false;
        bool forceDefaultDebris = false;
        bool forceDefaultChainDebris = false;
        bool disableReflections = false;
        bool disabled = false;
        bool keepMissingReflections = false;

        static Config& get_config();
    };

    struct NoteConfigRegistration : public Qosmetics::Core::Config::Registration
    {
        NoteConfigRegistration(std::string_view memberName) : Qosmetics::Core::Config::Registration(memberName){};
        void SaveConfig(rapidjson::Value& member, rapidjson::Document::AllocatorType& allocator) const override;
        bool LoadConfig(const rapidjson::Value& member) const override;
        void OnProfileSwitched() const override;
    };
}