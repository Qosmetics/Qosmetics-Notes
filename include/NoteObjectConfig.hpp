#pragma once
#include "beatsaber-hook/shared/config./rapidjson-utils.hpp"
#include "qosmetics-core/shared/Data/BasicConfig.hpp"

#ifndef GET_BOOL
#define GET_BOOL(identifier)                                                              \
    auto identifier##Itr = value.FindMember(#identifier);                                 \
    if (identifier##Itr != value.MemberEnd())                                             \
    {                                                                                     \
        identifier = identifier##Itr->value.IsBool() && identifier##Itr->value.GetBool(); \
    }

#endif
namespace Qosmetics::Notes
{
    class NoteObjectConfig : public Qosmetics::Core::BasicConfig
    {
    public:
        NoteObjectConfig() : Qosmetics::Core::BasicConfig(){};
        NoteObjectConfig(const rapidjson::Value& value) : Qosmetics::Core::BasicConfig(value)
        {
            GET_BOOL(hasDebris);
            GET_BOOL(hasSlider);
            GET_BOOL(hasBomb);
            GET_BOOL(showArrows);
            GET_BOOL(isLegacy);
            isDefault = false;
        }

        bool get_hasDebris() const { return hasDebris; };
        bool get_hasSlider() const { return hasSlider; };
        bool get_hasBomb() const { return hasBomb; };
        bool get_showArrows() const { return showArrows; };
        bool get_isDefault() const { return isDefault; };
        bool get_isLegacy() const { return isLegacy; };

    private:
        bool hasDebris = false;
        bool hasSlider = false;
        bool hasBomb = false;
        bool showArrows = true;
        bool isDefault = true;
        bool isLegacy = false;
    };
}