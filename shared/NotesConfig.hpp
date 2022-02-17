#pragma once

#include <string>

namespace Qosmetics::Notes
{
    struct NotesConfig
    {

    public:
        const std::string_view get_lastUsedCyoob() const { return lastUsedCyoob; };
        bool get_overrideNoteSize() const { return overrideNoteSize; };
        double get_noteSize() const { return noteSize; };

        bool get_alsoChangeHitboxes() const { return alsoChangeHitboxes; };
        bool get_forceDefaultBombs() const { return forceDefaultBombs; };
        bool get_forceDefaultDebris() const { return forceDefaultDebris; };
        bool get_disableReflections() const { return disableReflections; };
        bool get_disabled() const { return disabled; };
        bool get_keepMissingReflections() const { return keepMissingReflections; };

    private:
        std::string lastUsedCyoob;
        bool overrideNoteSize;
        double noteSize;

        bool alsoChangeHitboxes;
        bool forceDefaultBombs;
        bool forceDefaultDebris;
        bool disableReflections;
        bool disabled;
        bool keepMissingReflections;
    }
}