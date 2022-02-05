#pragma once

#include "CustomTypes/DebrisHandler.hpp"

#include "GlobalNamespace/ColorManager.hpp"
#include "GlobalNamespace/ColorType.hpp"
#include "GlobalNamespace/NoteDebris.hpp"
#include "UnityEngine/MonoBehaviour.hpp"
#include "custom-types/shared/macros.hpp"

DECLARE_CLASS_CODEGEN(Qosmetics::Notes, DebrisParent, UnityEngine::MonoBehaviour,
                      DECLARE_INSTANCE_FIELD(DebrisHandler*, debrisHandler);
                      DECLARE_INSTANCE_FIELD(GlobalNamespace::NoteDebris*, noteDebris);
                      DECLARE_INSTANCE_FIELD(GlobalNamespace::ColorManager*, colorManager);

                      DECLARE_INSTANCE_METHOD(void, Awake);
                      DECLARE_INSTANCE_METHOD(void, OnDestroy);
                      DECLARE_INSTANCE_METHOD(void, SetSliceProperties, GlobalNamespace::ColorType colorType, UnityEngine::Vector3 cutPoint, UnityEngine::Vector3 cutNormal);

                      public
                      : static DebrisParent * GetDebrisParent(GlobalNamespace::NoteDebris * debris);

)