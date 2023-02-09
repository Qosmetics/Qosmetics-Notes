#include "CustomTypes/BasicNoteScaler.hpp"
#include "config.hpp"

#include "GlobalNamespace/ILazyCopyHashSet_1.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/Transform.hpp"
#include "sombrero/shared/FastVector3.hpp"

DEFINE_TYPE(Qosmetics::Notes, BasicNoteScaler);

namespace Qosmetics::Notes
{
    void BasicNoteScaler::Awake()
    {
        noteController = get_gameObject()->GetComponentInParent<GlobalNamespace::NoteControllerBase*>();
        noteController->get_didInitEvent()->Add(i_INoteControllerDidInitEvent());
    }

    void BasicNoteScaler::OnDestroy()
    {
        noteController->get_didInitEvent()->Remove(i_INoteControllerDidInitEvent());
    }

    GlobalNamespace::INoteControllerDidInitEvent* BasicNoteScaler::i_INoteControllerDidInitEvent() { return reinterpret_cast<GlobalNamespace::INoteControllerDidInitEvent*>(this); }

    void BasicNoteScaler::HandleNoteControllerDidInit(GlobalNamespace::NoteControllerBase* noteController)
    {
        get_transform()->set_localScale(Sombrero::FastVector3::one() * Config::get_config().noteSize * notesUniformScale);
    }
}