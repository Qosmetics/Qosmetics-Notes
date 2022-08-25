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
#include "chroma/shared/CoreAPI.hpp"
#include "chroma/shared/NoteAPI.hpp"
#ifndef CHROMA_EXISTS
#define CHROMA_EXISTS
#endif
#endif

DEFINE_TYPE(Qosmetics::Notes, CyoobParent);

namespace Qosmetics::Notes
{
    std::unordered_map<GlobalNamespace::NoteControllerBase*, CyoobParent*> CyoobParent::noteControllerToParentMap = {};
    Sombrero::FastColor CyoobParent::globalRightColor = Sombrero::FastColor::black();
    Sombrero::FastColor CyoobParent::globalLeftColor = Sombrero::FastColor::black();
    Sombrero::FastColor CyoobParent::lastRightColor = Sombrero::FastColor::black();
    Sombrero::FastColor CyoobParent::lastLeftColor = Sombrero::FastColor::black();

    void CyoobParent::Awake()
    {
        noteController = get_gameObject()->GetComponent<GlobalNamespace::NoteControllerBase*>();
        noteController->get_didInitEvent()->Add(reinterpret_cast<GlobalNamespace::INoteControllerDidInitEvent*>(this));
        handler = get_gameObject()->GetComponentInChildren<CyoobHandler*>();
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
        if (handler)
        {
            auto noteData = gameNoteController->get_noteData();
            currentColorType = noteData->get_colorType();
            bool right = currentColorType == GlobalNamespace::ColorType::ColorB;

            handler->ShowNote(right, noteData->get_cutDirection() & 0b1000);

#ifdef CHROMA_EXISTS
            if (Chroma::CoreAPI::isChromaRunning())
            {
                auto thisColor = Chroma::NoteAPI::getNoteControllerColorSafe(gameNoteController);
                auto thatColor = right ? globalLeftColor : globalRightColor;
                if (thisColor.has_value())
                    Colorize(thisColor.value(), thatColor);
                else
                    Colorize(right ? globalRightColor : globalLeftColor, thatColor);
            }
#endif
        }
    }

    void CyoobParent::ColorizeSpecific(GlobalNamespace::NoteControllerBase* noteController, const Sombrero::FastColor& color, GlobalNamespace::ColorType colorType)
    {
        if (!noteController)
            return;
        auto cyoobparentItr = noteControllerToParentMap.find(noteController);
        if (cyoobparentItr != noteControllerToParentMap.end())
        {
            if (colorType == GlobalNamespace::ColorType::ColorB)
            {
                // == right
                lastRightColor = color;
                cyoobparentItr->second->Colorize(lastRightColor, lastLeftColor);
            }
            else
            {
                lastLeftColor = color;
                cyoobparentItr->second->Colorize(lastLeftColor, lastRightColor);
            }
        }
    }

    void CyoobParent::Colorize(Sombrero::FastColor leftColor, Sombrero::FastColor rightColor)
    {
        if (handler)
            handler->SetColors(leftColor, rightColor);
    }
}