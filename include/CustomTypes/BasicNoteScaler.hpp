#pragma once

#include "GlobalNamespace/INoteControllerDidInitEvent.hpp"
#include "GlobalNamespace/NoteControllerBase.hpp"
#include "UnityEngine/MonoBehaviour.hpp"
#include "custom-types/shared/macros.hpp"
#include "sombrero/shared/FastVector3.hpp"

DECLARE_CLASS_CODEGEN_INTERFACES(Qosmetics::Notes, BasicNoteScaler, UnityEngine::MonoBehaviour, classof(GlobalNamespace::INoteControllerDidInitEvent*),
                                 DECLARE_INSTANCE_FIELD(float, notesUniformScale);
                                 DECLARE_INSTANCE_FIELD_PRIVATE(GlobalNamespace::NoteControllerBase*, noteController);
                                 DECLARE_INSTANCE_METHOD(void, Awake);
                                 DECLARE_INSTANCE_METHOD(void, OnDestroy);
                                 DECLARE_INSTANCE_METHOD(GlobalNamespace::INoteControllerDidInitEvent*, i_INoteControllerDidInitEvent);
                                 DECLARE_OVERRIDE_METHOD_MATCH(void, HandleNoteControllerDidInit, &GlobalNamespace::INoteControllerDidInitEvent::HandleNoteControllerDidInit, GlobalNamespace::NoteControllerBase* noteController);

)
