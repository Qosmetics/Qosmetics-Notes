#include "CustomTypes/BombParent.hpp"

#include "GlobalNamespace/ILazyCopyHashSet_1.hpp"
#include "UnityEngine/GameObject.hpp"

#if __has_include("chroma/shared/BombAPI.hpp")
#include "chroma/shared/BombAPI.hpp"
#ifndef CHROMA_EXISTS
#define CHROMA_EXISTS
#endif
#endif

DEFINE_TYPE(Qosmetics::Notes, BombParent);

extern bool useChroma;
namespace Qosmetics::Notes
{
    std::unordered_map<GlobalNamespace::NoteControllerBase*, BombParent*> BombParent::noteControllerToParentMap = {};
    void BombParent::Awake()
    {
        noteController = get_gameObject()->GetComponent<GlobalNamespace::NoteControllerBase*>();
        if (useChroma)
            noteController->get_didInitEvent()->Add(reinterpret_cast<GlobalNamespace::INoteControllerDidInitEvent*>(this));

        colorHandler = get_gameObject()->GetComponentInChildren<BombColorHandler*>();
        noteControllerToParentMap[noteController] = this;
    }

    void BombParent::HandleNoteControllerDidInit(GlobalNamespace::NoteControllerBase* noteController)
    {
#ifdef CHROMA_EXISTS
        auto bombNoteController = reinterpret_cast<GlobalNamespace::BombNoteController*>(noteController);

        auto color = Chroma::BombAPI::getBombNoteControllerOverrideColorSafe(bombNoteController);
        if (color.has_value())
            Colorize(color.value());
#endif
    }

    void BombParent::OnDestroy()
    {
        if (useChroma)
            noteController->get_didInitEvent()->Remove(reinterpret_cast<GlobalNamespace::INoteControllerDidInitEvent*>(this));
        noteControllerToParentMap.erase(noteController);
    }

    void BombParent::Colorize(Sombrero::FastColor color)
    {
        if (lastBombColor.operator==(color))
            return;

        colorHandler->SetColor(color);
    }

    void BombParent::ColorizeSpecific(GlobalNamespace::NoteControllerBase* noteController, Sombrero::FastColor const& color)
    {
        auto bombParentItr = noteControllerToParentMap.find(noteController);
        if (bombParentItr != noteControllerToParentMap.end())
        {
            bombParentItr->second->Colorize(color);
            bombParentItr->second->Colorize(color);
        }
    }
}