#pragma once

#include "GlobalNamespace/ColorNoteVisuals.hpp"
#include "GlobalNamespace/MaterialPropertyBlockController.hpp"

#include "UnityEngine/Material.hpp"
#include "UnityEngine/MonoBehaviour.hpp"
#include "UnityEngine/Renderer.hpp"
#include "custom-types/shared/macros.hpp"
#include "sombrero/shared/FastColor.hpp"

DECLARE_CLASS_CODEGEN(Qosmetics::Notes, CyoobColorHandler, UnityEngine::MonoBehaviour,
                      DECLARE_INSTANCE_FIELD_PRIVATE(Sombrero::FastColor, lastThisColor);
                      DECLARE_INSTANCE_FIELD_PRIVATE(Sombrero::FastColor, lastThatColor);
                      DECLARE_INSTANCE_FIELD_PRIVATE(ArrayW<UnityEngine::Material*>, customColorMaterials);
                      DECLARE_INSTANCE_FIELD_PRIVATE(ArrayW<UnityEngine::Material*>, thisColorMaterials);
                      DECLARE_INSTANCE_FIELD_PRIVATE(ArrayW<UnityEngine::Material*>, thatColorMaterials);
                      DECLARE_INSTANCE_FIELD_PRIVATE(ArrayW<UnityEngine::Renderer*>, materialReplacementRenderers);
                      DECLARE_INSTANCE_FIELD_PRIVATE(GlobalNamespace::MaterialPropertyBlockController*, propertyController);

                      DECLARE_INSTANCE_METHOD(void, Awake);
                      DECLARE_INSTANCE_METHOD(void, FetchCCMaterials);
                      DECLARE_INSTANCE_METHOD(void, AddRenderersToPropertyBlockController);
                      DECLARE_INSTANCE_METHOD(void, SetColors, Sombrero::FastColor thisColor, Sombrero::FastColor thatColor);

)
