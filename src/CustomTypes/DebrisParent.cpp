#include "CustomTypes/DebrisParent.hpp"
#include "GlobalNamespace/ColorManager.hpp"
#include "logging.hpp"
#include <fmt/format.h>
#include <unordered_map>

DEFINE_TYPE(Qosmetics::Notes, DebrisParent);

#if __has_include("chroma/shared/CoreAPI.hpp")
#include "chroma/shared/CoreAPI.hpp"
#ifndef CHROMA_EXISTS
#define CHROMA_EXISTS
#endif
#endif

namespace Qosmetics::Notes
{
    std::unordered_map<GlobalNamespace::NoteDebris*, DebrisParent*> noteDebrisCache;
    DebrisParent* DebrisParent::GetDebrisParent(GlobalNamespace::NoteDebris* debris)
    {
        auto itr = noteDebrisCache.find(debris);
        if (itr != noteDebrisCache.end())
            return itr->second;
        else
            return nullptr;
    }

    void DebrisParent::Awake()
    {
        debrisHandler = get_gameObject()->GetComponentInChildren<DebrisHandler*>();
        noteDebris = GetComponent<GlobalNamespace::NoteDebris*>();
        noteDebrisCache[noteDebris] = this;
        colorManager = noteDebris->_colorManager;
    }

    void DebrisParent::OnDestroy()
    {
        noteDebrisCache.erase(noteDebris);
    }

    void DebrisParent::SetSliceProperties(GlobalNamespace::ColorType colorType, UnityEngine::Vector3 cutPoint, UnityEngine::Vector3 cutNormal)
    {
        DEBUG("{}: Setting slice properties", fmt::ptr(this));
        debrisHandler->ShowDebris(colorType.value__);
        debrisHandler->SetSliceProperties(cutPoint, cutNormal);

#ifdef CHROMA_EXISTS
        if (Chroma::CoreAPI::isChromaRunning())
        {
            UnityEngine::Color thisColor = colorManager->ColorForType(colorType);
            UnityEngine::Color thatColor = colorManager->ColorForType(1 - colorType);

            debrisHandler->SetColors(thisColor, thatColor);
        }
#endif
    }
}
