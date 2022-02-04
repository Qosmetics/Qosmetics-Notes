#include "CustomTypes/CyoobParent.hpp"
#include "logging.hpp"

#include "GlobalNamespace/ColorNoteVisuals.hpp"
#include "GlobalNamespace/ColorType.hpp"
#include "GlobalNamespace/GameNoteController.hpp"
#include "GlobalNamespace/ILazyCopyHashSet_1.hpp"
#include "GlobalNamespace/NoteCutDirection.hpp"
#include "GlobalNamespace/NoteData.hpp"

#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/Transform.hpp"

DEFINE_TYPE(Qosmetics::Notes, CyoobParent);

namespace Qosmetics::Notes
{
    void CyoobParent::Awake()
    {
        noteController = get_gameObject()->GetComponent<GlobalNamespace::NoteControllerBase*>();
        noteController->get_didInitEvent()->Add(reinterpret_cast<GlobalNamespace::INoteControllerDidInitEvent*>(this));
        colorManager = get_gameObject()->GetComponentInChildren<GlobalNamespace::ColorNoteVisuals*>()->dyn__colorManager();
        cyoobHandler = get_gameObject()->GetComponentInChildren<CyoobHandler*>();
    }

    void CyoobParent::OnDestroy()
    {
        noteController->get_didInitEvent()->Remove(reinterpret_cast<GlobalNamespace::INoteControllerDidInitEvent*>(this));
    }

    void CyoobParent::HandleNoteControllerDidInit(GlobalNamespace::NoteControllerBase* noteController)
    {
        auto gameNoteController = reinterpret_cast<GlobalNamespace::GameNoteController*>(noteController);
        if (!cyoobHandler)
        {
            return;
        }

        auto noteData = gameNoteController->get_noteData();
        bool right = noteData->get_colorType() == GlobalNamespace::ColorType::ColorB;
        bool dot = noteData->get_cutDirection() >= 8;

        cyoobHandler->ShowNote(right, dot);

        UnityEngine::Color thisColor = colorManager->ColorForType(noteData->get_colorType());
        UnityEngine::Color thatColor = colorManager->ColorForType(1 - noteData->get_colorType());

        cyoobHandler->SetColors(thisColor, thatColor);
    }

}