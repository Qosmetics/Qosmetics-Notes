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

DECLARE_CLASS_CODEGEN_INTERFACES(Qosmetics::Notes, CyoobParent, UnityEngine::MonoBehaviour, classof(GlobalNamespace::INoteControllerDidInitEvent*),

                                 DECLARE_INSTANCE_FIELD_PRIVATE(GlobalNamespace::NoteControllerBase*, noteController);
                                 DECLARE_INSTANCE_FIELD_PRIVATE(GlobalNamespace::ColorManager*, colorManager);
                                 DECLARE_INSTANCE_FIELD_PRIVATE(CyoobHandler*, handler);
                                 DECLARE_INSTANCE_FIELD_PRIVATE(GlobalNamespace::ColorType, currentColorType);
                                 DECLARE_INSTANCE_METHOD(void, Awake);
                                 DECLARE_INSTANCE_METHOD(void, OnDestroy);
                                 DECLARE_INSTANCE_METHOD(void, Colorize, Sombrero::FastColor thisColor, Sombrero::FastColor otherColor);
                                 DECLARE_OVERRIDE_METHOD_MATCH(void, HandleNoteControllerDidInit, &GlobalNamespace::INoteControllerDidInitEvent::HandleNoteControllerDidInit, GlobalNamespace::NoteControllerBase* noteController);

                                 public
                                 :

                                 static Sombrero::FastColor lastRightColor;
                                 static Sombrero::FastColor lastLeftColor;
                                 static Sombrero::FastColor globalRightColor;
                                 static Sombrero::FastColor globalLeftColor;

                                 static void ColorizeSpecific(GlobalNamespace::NoteControllerBase* noteController, const Sombrero::FastColor& color, GlobalNamespace::ColorType colorType);
                                 static std::unordered_map<GlobalNamespace::NoteControllerBase*, CyoobParent*> noteControllerToParentMap;

                                 __declspec(property(get = get_Handler, put = set_Handler)) CyoobHandler * Handler;
                                 CyoobHandler * get_Handler() const;
                                 void set_Handler(CyoobHandler* handler);

)
