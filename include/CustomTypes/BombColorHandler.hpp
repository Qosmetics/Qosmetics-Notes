#pragma once

#include "UnityEngine/Material.hpp"
#include "sombrero/shared/FastColor.hpp"

#include "UnityEngine/MonoBehaviour.hpp"
#include "custom-types/shared/macros.hpp"

DECLARE_CLASS_CODEGEN(Qosmetics::Notes, BombColorHandler, UnityEngine::MonoBehaviour,
                      DECLARE_INSTANCE_FIELD(Sombrero::FastColor, lastColor);
                      DECLARE_INSTANCE_FIELD(ArrayW<UnityEngine::Material*>, customColorMaterials);

                      DECLARE_INSTANCE_METHOD(void, Awake);
                      DECLARE_INSTANCE_METHOD(void, FetchCCMaterials);
                      DECLARE_INSTANCE_METHOD(void, SetColor, Sombrero::FastColor color);

)