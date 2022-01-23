#pragma once

#include "GlobalNamespace/INoteControllerDidInitEvent.hpp"
#include "GlobalNamespace/NoteControllerBase.hpp"
#include "UnityEngine/MonoBehaviour.hpp"
#include "custom-types/shared/macros.hpp"

#ifndef DECLARE_OVERRIDE_METHOD_MATCH
#define DECLARE_OVERRIDE_METHOD_MATCH(retval, method, mptr, ...) \
    DECLARE_OVERRIDE_METHOD(retval, method, il2cpp_utils::il2cpp_type_check::MetadataGetter<mptr>::get(), __VA_ARGS__)
#endif

DECLARE_CLASS_CODEGEN_INTERFACES(Qosmetics::Notes, Cyoob, UnityEngine::MonoBehaviour, classof(GlobalNamespace::INoteControllerDidInitEvent*),
                                 DECLARE_INSTANCE_FIELD(GlobalNamespace::NoteControllerBase*, noteController);
                                 DECLARE_INSTANCE_METHOD(void, Awake);
                                 DECLARE_INSTANCE_METHOD(void, OnDestroy);
                                 DECLARE_OVERRIDE_METHOD_MATCH(void, HandleNoteControllerDidInit, &GlobalNamespace::INoteControllerDidInitEvent::HandleNoteControllerDidInit, GlobalNamespace::NoteControllerBase* noteController);

)