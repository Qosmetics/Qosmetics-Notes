#include "CustomTypes/DebrisParent.hpp"
#include "GlobalNamespace/ColorManager.hpp"
#include "logging.hpp"
#include <unordered_map>

DEFINE_TYPE(Qosmetics::Notes, DebrisParent);

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
        colorManager = noteDebris->dyn__colorManager();
    }

    void DebrisParent::OnDestroy()
    {
        noteDebrisCache.erase(noteDebris);
    }

    void DebrisParent::SetSliceProperties(GlobalNamespace::ColorType colorType, UnityEngine::Vector3 cutPoint, UnityEngine::Vector3 cutNormal)
    {
        DEBUG("%p: Setting slice properties", this);
        debrisHandler->ShowDebris(colorType);
        debrisHandler->SetSliceProperties(cutPoint, cutNormal);

        UnityEngine::Color thisColor = colorManager->ColorForType(colorType);
        UnityEngine::Color thatColor = colorManager->ColorForType(1 - colorType);

        debrisHandler->SetColors(thisColor, thatColor);
    }
}