#include "CustomTypes/ChainParent.hpp"
#include "config.hpp"
#include "logging.hpp"

#include "GlobalNamespace/BurstSliderGameNoteController.hpp"
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

DEFINE_TYPE(Qosmetics::Notes, ChainParent);

extern bool useChroma;
namespace Qosmetics::Notes
{
    std::unordered_map<GlobalNamespace::NoteControllerBase*, ChainParent*> ChainParent::noteControllerToParentMap = {};
    Sombrero::FastColor ChainParent::globalRightColor = Sombrero::FastColor::black();
    Sombrero::FastColor ChainParent::globalLeftColor = Sombrero::FastColor::black();
    Sombrero::FastColor ChainParent::lastRightColor = Sombrero::FastColor::black();
    Sombrero::FastColor ChainParent::lastLeftColor = Sombrero::FastColor::black();

    void ChainParent::Awake()
    {
        noteController = get_gameObject()->GetComponent<GlobalNamespace::NoteControllerBase*>();
        noteController->get_didInitEvent()->Add(reinterpret_cast<GlobalNamespace::INoteControllerDidInitEvent*>(this));
        chainHandler = get_gameObject()->GetComponentInChildren<ChainHandler*>();
        noteControllerToParentMap[noteController] = this;
        isHead = get_gameObject()->GetComponent<GlobalNamespace::BurstSliderGameNoteController*>() == nullptr;
    }

    void ChainParent::OnDestroy()
    {
        noteController->get_didInitEvent()->Remove(reinterpret_cast<GlobalNamespace::INoteControllerDidInitEvent*>(this));
        noteControllerToParentMap.erase(noteController);
    }

    void ChainParent::HandleNoteControllerDidInit(GlobalNamespace::NoteControllerBase* noteController)
    {
        auto gameNoteController = reinterpret_cast<GlobalNamespace::GameNoteController*>(noteController);
        if (chainHandler)
        {
            auto noteData = gameNoteController->get_noteData();
            currentColorType = noteData->get_colorType();
            bool right = currentColorType == GlobalNamespace::ColorType::ColorB;

            chainHandler->ShowNote(right, isHead);

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

    void ChainParent::ColorizeSpecific(GlobalNamespace::NoteControllerBase* noteController, const Sombrero::FastColor& color, GlobalNamespace::ColorType colorType)
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

    void ChainParent::Colorize(Sombrero::FastColor leftColor, Sombrero::FastColor rightColor)
    {
        if (chainHandler)
            chainHandler->SetColors(leftColor, rightColor);
    }
}