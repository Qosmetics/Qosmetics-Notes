#pragma once
#include "qosmetics-core/shared/ConfigRegister.hpp"

namespace Qosmetics::Notes
{
    struct Config
    {
        std::string lastUsedCyoob;
        double noteSize = 1.0f;
        bool forceDefaultBombs = false;
        bool forceDefaultChains = false;
        bool forceDefaultDebris = false;
        bool forceDefaultChainDebris = false;
        bool disableReflections = false;
        bool disabled = false;
        bool keepMissingReflections = false;

        static Config& get_config();

        auto get_overrideNoteSize() const { return overrideNoteSize; }
        void set_overrideNoteSize(auto value)
        {
            overrideNoteSize = value;
            UpdateSubmission();
        }
        auto get_alsoChangeHitboxes() const { return alsoChangeHitboxes; }
        void set_alsoChangeHitboxes(auto value)
        {
            alsoChangeHitboxes = value;
            UpdateSubmission();
        }

        void UpdateSubmission() const;

    private:
        bool overrideNoteSize = false;
        bool alsoChangeHitboxes = false;
    };

    struct NoteConfigRegistration : public Qosmetics::Core::Config::Registration
    {
        NoteConfigRegistration(std::string_view memberName) : Qosmetics::Core::Config::Registration(memberName){};
        void SaveConfig(rapidjson::Value& member, rapidjson::Document::AllocatorType& allocator) const override;
        bool LoadConfig(const rapidjson::Value& member) const override;
        void OnProfileSwitched() const override;
    };
}