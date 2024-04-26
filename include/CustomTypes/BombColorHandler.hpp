#pragma once

#include "UnityEngine/Material.hpp"
#include "sombrero/shared/FastColor.hpp"

#include "GlobalNamespace/MaterialPropertyBlockController.hpp"
#include "UnityEngine/MonoBehaviour.hpp"
#include "UnityEngine/Renderer.hpp"
#include "custom-types/shared/macros.hpp"

DECLARE_CLASS_CODEGEN(Qosmetics::Notes, BombColorHandler, UnityEngine::MonoBehaviour,
                      DECLARE_INSTANCE_FIELD_PRIVATE(Sombrero::FastColor, lastColor);
                      DECLARE_INSTANCE_FIELD_PRIVATE(ArrayW<UnityEngine::Material*>, customColorMaterials);
                      DECLARE_INSTANCE_FIELD_PRIVATE(ArrayW<UnityEngine::Renderer*>, materialReplacementRenderers);
                      DECLARE_INSTANCE_FIELD_PRIVATE(GlobalNamespace::MaterialPropertyBlockController*, propertyController);

                      DECLARE_INSTANCE_METHOD(void, Awake);
                      DECLARE_INSTANCE_METHOD(void, FetchCCMaterials);
                      DECLARE_INSTANCE_METHOD(void, AddRenderersToPropertyBlockController);
                      DECLARE_INSTANCE_METHOD(void, SetColor, Sombrero::FastColor color);

)
