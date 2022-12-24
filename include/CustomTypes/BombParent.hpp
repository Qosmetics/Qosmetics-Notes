#pragma once

#include "CustomTypes/BombColorHandler.hpp"

#include "GlobalNamespace/BombNoteController.hpp"
#include "GlobalNamespace/INoteControllerDidInitEvent.hpp"
#include "GlobalNamespace/NoteControllerBase.hpp"
#include "UnityEngine/MonoBehaviour.hpp"
#include "custom-types/shared/macros.hpp"
#include "sombrero/shared/FastColor.hpp"
#include <unordered_map>

#ifndef DECLARE_OVERRIDE_METHOD_MATCH
#define DECLARE_OVERRIDE_METHOD_MATCH(retval, method, mptr, ...) \
    DECLARE_OVERRIDE_METHOD(retval, method, il2cpp_utils::il2cpp_type_check::MetadataGetter<mptr>::get(), __VA_ARGS__)
#endif

#include "private_field.hpp"

DECLARE_CLASS_CODEGEN_INTERFACES(Qosmetics::Notes, BombParent, UnityEngine::MonoBehaviour, classof(GlobalNamespace::INoteControllerDidInitEvent*),
                                 DECLARE_INSTANCE_PRIVATE_FIELD(BombColorHandler*, colorHandler);
                                 DECLARE_INSTANCE_PRIVATE_FIELD(GlobalNamespace::NoteControllerBase*, noteController);
                                 DECLARE_INSTANCE_PRIVATE_FIELD(Sombrero::FastColor, lastBombColor);
                                 DECLARE_INSTANCE_METHOD(void, Awake);
                                 DECLARE_INSTANCE_METHOD(void, OnDestroy);
                                 DECLARE_INSTANCE_METHOD(void, Colorize, Sombrero::FastColor color);

                                 DECLARE_OVERRIDE_METHOD_MATCH(void, HandleNoteControllerDidInit, &GlobalNamespace::INoteControllerDidInitEvent::HandleNoteControllerDidInit, GlobalNamespace::NoteControllerBase* noteController);
                                 public
                                 :

                                 static void ColorizeSpecific(GlobalNamespace::NoteControllerBase* noteController, const Sombrero::FastColor& color);
                                 static std::unordered_map<GlobalNamespace::NoteControllerBase*, BombParent*>
                                     noteControllerToParentMap;

)