#include "CustomTypes/CyoobParent.hpp"
#include "config.hpp"
#include "logging.hpp"

#include "GlobalNamespace/ColorNoteVisuals.hpp"
#include "GlobalNamespace/ColorType.hpp"
#include "GlobalNamespace/GameNoteController.hpp"
#include "GlobalNamespace/ILazyCopyHashSet_1.hpp"
#include "GlobalNamespace/NoteCutDirection.hpp"
#include "GlobalNamespace/NoteData.hpp"

#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/Transform.hpp"

#if __has_include("chroma/shared/NoteAPI.hpp")
#include "chroma/shared/NoteAPI.hpp"
#ifndef CHROMA_EXISTS
#define CHROMA_EXISTS
#endif
#endif

DEFINE_TYPE(Qosmetics::Notes, CyoobParent);

namespace Qosmetics::Notes
{
    std::unordered_map<GlobalNamespace::NoteControllerBase*, CyoobParent*> CyoobParent::noteControllerToParentMap = {};
    Sombrero::FastColor CyoobParent::lastRightColor = Sombrero::FastColor::black();
    Sombrero::FastColor CyoobParent::lastLeftColor = Sombrero::FastColor::black();

    void CyoobParent::Awake()
    {
        noteController = get_gameObject()->GetComponent<GlobalNamespace::NoteControllerBase*>();
        noteController->get_didInitEvent()->Add(reinterpret_cast<GlobalNamespace::INoteControllerDidInitEvent*>(this));
        cyoobHandler = get_gameObject()->GetComponentInChildren<CyoobHandler*>();
        noteControllerToParentMap[noteController] = this;
    }

    void CyoobParent::OnDestroy()
    {
        noteController->get_didInitEvent()->Remove(reinterpret_cast<GlobalNamespace::INoteControllerDidInitEvent*>(this));
        noteControllerToParentMap.erase(noteController);
    }

    void CyoobParent::HandleNoteControllerDidInit(GlobalNamespace::NoteControllerBase* noteController)
    {
        auto gameNoteController = reinterpret_cast<GlobalNamespace::GameNoteController*>(noteController);
        if (cyoobHandler)
        {
            auto noteData = gameNoteController->get_noteData();
            currentColorType = noteData->get_colorType();
            bool right = currentColorType == GlobalNamespace::ColorType::ColorB;
            bool dot = noteData->get_cutDirection() >= 8;

            cyoobHandler->ShowNote(right, dot);
        }
    }

    void CyoobParent::ColorizeSpecific(GlobalNamespace::GameNoteController* noteController, Sombrero::FastColor color)
    {
        auto cyoobparentItr = noteControllerToParentMap.find(noteController);
        if (cyoobparentItr != noteControllerToParentMap.end())
        {
            bool right = currentColorType == GlobalNamespace::ColorType::ColorB;

            if (right)
            {
                lastRightColor = color;
                cyoobparentItr->second->Colorize(color, lastLeftColor);
            }
            else
            {
                lastLeftColor = color;
                cyoobparentItr->second->Colorize(color, lastRightColor);
            }
        }
    }

    void CyoobParent::Colorize(Sombrero::FastColor thisColor, Sombrero::FastColor thatColor)
    {
        cyoobHandler->SetColors(thisColor, thatColor);
    }
}