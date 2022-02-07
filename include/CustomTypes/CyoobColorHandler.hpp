#pragma once

#include "GlobalNamespace/ColorNoteVisuals.hpp"

#include "UnityEngine/Color.hpp"
#include "UnityEngine/Material.hpp"
#include "UnityEngine/MonoBehaviour.hpp"
#include "custom-types/shared/macros.hpp"

DECLARE_CLASS_CODEGEN(Qosmetics::Notes, CyoobColorHandler, UnityEngine::MonoBehaviour,
                      DECLARE_INSTANCE_FIELD(UnityEngine::Color, lastThisColor);
                      DECLARE_INSTANCE_FIELD(UnityEngine::Color, lastThatColor);
                      DECLARE_INSTANCE_FIELD(ArrayW<UnityEngine::Material*>, customColorMaterials);
                      DECLARE_INSTANCE_FIELD(GlobalNamespace::ColorNoteVisuals*, colorNoteVisuals);

                      DECLARE_INSTANCE_METHOD(void, Awake);
                      DECLARE_INSTANCE_METHOD(void, FetchCCMaterials);
                      DECLARE_INSTANCE_METHOD(void, SetColors, UnityEngine::Color thisColor, UnityEngine::Color thatColor);

)