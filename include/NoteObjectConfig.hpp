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

#ifndef ADD_BOOL
#define ADD_BOOL(identifier) val.AddMember(#identifier, identifier, allocator)
#endif

#ifndef CONST_GETTER
#define CONST_GETTER(identifier) \
    auto get_##identifier() const { return identifier; }
#endif

namespace Qosmetics::Notes
{
    class NoteObjectConfig : public Qosmetics::Core::BasicConfig
    {
    public:
        NoteObjectConfig() : Qosmetics::Core::BasicConfig(){};
        NoteObjectConfig(bool showArrows, bool hasBomb, bool hasDebris) : showArrows(showArrows), hasBomb(hasBomb), hasDebris(hasDebris), isLegacy(true){};
        NoteObjectConfig(const rapidjson::Value& value) : Qosmetics::Core::BasicConfig(value)
        {
            GET_BOOL(hasDebris);
            GET_BOOL(hasChainHeadDebris);
            GET_BOOL(hasChainLinkDebris);
            GET_BOOL(hasSlider);
            GET_BOOL(hasBomb);
            GET_BOOL(showArrows);
            GET_BOOL(isLegacy);
            GET_BOOL(isMirrorable);
            isDefault = false;
        }

        CONST_GETTER(hasDebris)
        CONST_GETTER(hasChainHeadDebris)
        CONST_GETTER(hasChainLinkDebris)
        CONST_GETTER(hasSlider)
        CONST_GETTER(hasBomb)
        CONST_GETTER(showArrows)
        CONST_GETTER(isDefault)
        CONST_GETTER(isLegacy)
        CONST_GETTER(isMirrorable)

        rapidjson::Value ToJson(rapidjson::Document::AllocatorType& allocator) override
        {
            rapidjson::Value val;
            val.SetObject();

            ADD_BOOL(hasDebris);
            ADD_BOOL(hasChainHeadDebris);
            ADD_BOOL(hasChainLinkDebris);
            ADD_BOOL(hasSlider);
            ADD_BOOL(hasBomb);
            ADD_BOOL(showArrows);
            ADD_BOOL(isLegacy);
            ADD_BOOL(isMirrorable);

            return val;
        }

    private:
        bool hasDebris = false;
        bool hasChainHeadDebris = false;
        bool hasChainLinkDebris = false;
        bool hasSlider = false;
        bool hasBomb = false;
        bool showArrows = true;
        bool isDefault = true;
        bool isLegacy = false;
        bool isMirrorable = false;
    };
}