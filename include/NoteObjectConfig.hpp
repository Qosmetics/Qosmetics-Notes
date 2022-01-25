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
        }

    private:
        bool hasDebris = false;
        bool hasSlider = false;
        bool hasBomb = false;
        bool showArrows = false;
    };
}