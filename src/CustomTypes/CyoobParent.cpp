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

extern bool useChroma;
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

#ifdef CHROMA_EXISTS
            if (useChroma)
            {
                auto thisColor = Chroma::NoteAPI::getNoteControllerColorSafe(gameNoteController).value_or(right ? globalRightColor : globalLeftColor);
                auto thatColor = right ? globalLeftColor : globalRightColor;

                Colorize(thisColor, thatColor);
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
        else
            ERROR("Couldn't find notecontroller in the map, thus not able to colorize Cyoob!");
    }

    void CyoobParent::Colorize(Sombrero::FastColor leftColor, Sombrero::FastColor rightColor)
    {
        if (cyoobHandler)
            cyoobHandler->SetColors(leftColor, rightColor);
    }
}