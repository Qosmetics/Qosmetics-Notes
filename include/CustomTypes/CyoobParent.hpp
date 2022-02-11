#pragma once

#include "CustomTypes/CyoobHandler.hpp"
#include "GlobalNamespace/ColorManager.hpp"
#include "GlobalNamespace/ColorType.hpp"
#include "GlobalNamespace/GameNoteController.hpp"
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

DECLARE_CLASS_CODEGEN_INTERFACES(Qosmetics::Notes, CyoobParent, UnityEngine::MonoBehaviour, classof(GlobalNamespace::INoteControllerDidInitEvent*),

                                 DECLARE_INSTANCE_FIELD(GlobalNamespace::NoteControllerBase*, noteController);
                                 DECLARE_INSTANCE_FIELD(GlobalNamespace::ColorManager*, colorManager);
                                 DECLARE_INSTANCE_FIELD(CyoobHandler*, cyoobHandler);
                                 DECLARE_INSTANCE_FIELD(GlobalNamespace::ColorType, currentColorType);
                                 DECLARE_INSTANCE_METHOD(void, Awake);
                                 DECLARE_INSTANCE_METHOD(void, OnDestroy);
                                 DECLARE_INSTANCE_METHOD(void, Colorize, Sombrero::FastColor thisColor, Sombrero::FastColor otherColor);
                                 DECLARE_INSTANCE_METHOD(void, ColorizeSpecific, GlobalNamespace::GameNoteController* noteController, Sombrero::FastColor color);
                                 DECLARE_OVERRIDE_METHOD_MATCH(void, HandleNoteControllerDidInit, &GlobalNamespace::INoteControllerDidInitEvent::HandleNoteControllerDidInit, GlobalNamespace::NoteControllerBase* noteController);

                                 public
                                 :

                                 static Sombrero::FastColor lastRightColor;
                                 static Sombrero::FastColor lastLeftColor;

                                 static std::unordered_map<GlobalNamespace::NoteControllerBase*, CyoobParent*>
                                     noteControllerToParentMap;

)