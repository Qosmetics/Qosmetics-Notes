#include "CustomTypes/Cyoob.hpp"
#include "GlobalNamespace/ColorNoteVisuals.hpp"
#include "GlobalNamespace/GameNoteController.hpp"
#include "GlobalNamespace/ILazyCopyHashSet_1.hpp"
#include "UnityEngine/GameObject.hpp"

DEFINE_TYPE(Qosmetics::Notes, Cyoob);

namespace Qosmetics::Notes
{
    void Cyoob::Awake()
    {
        noteController = get_gameObject()->GetComponent<GlobalNamespace::NoteControllerBase*>();
        noteController->get_didInitEvent()->Add(reinterpret_cast<GlobalNamespace::INoteControllerDidInitEvent*>(this));
    }

    void Cyoob::OnDestroy()
    {
        noteController->get_didInitEvent()->Remove(reinterpret_cast<GlobalNamespace::INoteControllerDidInitEvent*>(this));
    }

    void Cyoob::HandleNoteControllerDidInit(GlobalNamespace::NoteControllerBase* noteController)
    {
        auto gameNoteController = reinterpret_cast<GlobalNamespace::GameNoteController*>(noteController);

        auto cnv = GetComponent<GlobalNamespace::ColorNoteVisuals*>();
        cnv->set_showCircle(true);
        cnv->set_showArrow(true);
    }

}