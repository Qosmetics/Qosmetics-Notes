#pragma once

#include "CustomTypes/BombColorHandler.hpp"

#include "GlobalNamespace/BombNoteController.hpp"
#include "GlobalNamespace/NoteControllerBase.hpp"
#include "UnityEngine/MonoBehaviour.hpp"
#include "custom-types/shared/macros.hpp"
#include "sombrero/shared/FastColor.hpp"

DECLARE_CLASS_CODEGEN(Qosmetics::Notes, BombParent, UnityEngine::MonoBehaviour,
                      DECLARE_INSTANCE_FIELD(BombColorHandler*, colorHandler);
                      DECLARE_INSTANCE_FIELD(GlobalNamespace::NoteControllerBase*, noteController);
                      DECLARE_INSTANCE_FIELD(Sombrero::FastColor, lastBombColor);
                      DECLARE_INSTANCE_METHOD(void, Awake);
                      DECLARE_INSTANCE_METHOD(void, Colorize, Sombrero::FastColor color);
                      DECLARE_STATIC_METHOD(void, ColorizeSpecific, GlobalNamespace::BombNoteController* noteController, Sombrero::FastColor color);

)