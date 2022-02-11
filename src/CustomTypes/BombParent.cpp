#include "CustomTypes/BombParent.hpp"

#include "UnityEngine/GameObject.hpp"

DEFINE_TYPE(Qosmetics::Notes, BombParent);

namespace Qosmetics::Notes
{
    void BombParent::Awake()
    {
        noteController = get_gameObject()->GetComponent<GlobalNamespace::NoteControllerBase*>();
        colorHandler = get_gameObject()->GetComponentInChildren<BombColorHandler*>();
    }

    void BombParent::Colorize(Sombrero::FastColor color)
    {
        if (lastBombColor.operator==(color))
            return;

        colorHandler->SetColor(color);
    }

    void BombParent::ColorizeSpecific(GlobalNamespace::BombNoteController* noteController, Sombrero::FastColor color)
    {
    }
}