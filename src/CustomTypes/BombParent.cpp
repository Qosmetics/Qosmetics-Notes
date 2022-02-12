#include "CustomTypes/BombParent.hpp"

#include "UnityEngine/GameObject.hpp"

DEFINE_TYPE(Qosmetics::Notes, BombParent);

namespace Qosmetics::Notes
{
    std::unordered_map<GlobalNamespace::NoteControllerBase*, BombParent*> BombParent::noteControllerToParentMap = {};
    void BombParent::Awake()
    {
        noteController = get_gameObject()->GetComponent<GlobalNamespace::NoteControllerBase*>();
        colorHandler = get_gameObject()->GetComponentInChildren<BombColorHandler*>();
        noteControllerToParentMap[noteController] = this;
    }

    void BombParent::OnDestroy()
    {
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