#pragma once

#include "sombrero/shared/FastColor.hpp"

#include "UnityEngine/Color.hpp"
#include "UnityEngine/Material.hpp"
#include "UnityEngine/MonoBehaviour.hpp"
#include "custom-types/shared/macros.hpp"

DECLARE_CLASS_CODEGEN(Qosmetics::Notes, DebrisColorHandler, UnityEngine::MonoBehaviour,
                      DECLARE_INSTANCE_FIELD(Sombrero::FastColor, lastThisColor);
                      DECLARE_INSTANCE_FIELD(Sombrero::FastColor, lastThatColor);
                      DECLARE_INSTANCE_FIELD(ArrayW<UnityEngine::Material*>, customColorMaterials);

                      DECLARE_INSTANCE_METHOD(void, Awake);
                      DECLARE_INSTANCE_METHOD(void, FetchCCMaterials);
                      DECLARE_INSTANCE_METHOD(void, SetColors, Sombrero::FastColor thisColor, Sombrero::FastColor thatColor);

);