#pragma once

#include "sombrero/shared/FastColor.hpp"
#include "sombrero/shared/FastVector3.hpp"

#include "System/Collections/Generic/KeyValuePair_2.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/Material.hpp"
#include "UnityEngine/MonoBehaviour.hpp"
#include "UnityEngine/Renderer.hpp"
#include "custom-types/shared/macros.hpp"

#include "CustomTypes/DebrisColorHandler.hpp"

DECLARE_CLASS_CODEGEN(Qosmetics::Notes, DebrisHandler, UnityEngine::MonoBehaviour,
                      DECLARE_INSTANCE_FIELD(int, previouslyActive);
                      DECLARE_INSTANCE_FIELD(ArrayW<UnityEngine::GameObject*>, objects);
                      DECLARE_INSTANCE_FIELD(ArrayW<DebrisColorHandler*>, colorHandlers);
                      using RendererMaterialPair = System::Collections::Generic::KeyValuePair_2<UnityEngine::Renderer*, UnityEngine::Material*>;
                      DECLARE_INSTANCE_FIELD(ArrayW<ArrayW<RendererMaterialPair>>, sliceMaterials);
                      DECLARE_INSTANCE_FIELD(ArrayW<bool>, anySliceMaterials);
                      DECLARE_INSTANCE_FIELD(Sombrero::FastVector3, lastCutPoint);
                      DECLARE_INSTANCE_FIELD(Sombrero::FastVector3, lastCutNormal);

                      DECLARE_INSTANCE_METHOD(void, Awake);
                      DECLARE_INSTANCE_METHOD(void, SetColors, Sombrero::FastColor thisColor, Sombrero::FastColor thatColor);
                      DECLARE_INSTANCE_METHOD(void, ShowDebris, bool right);
                      DECLARE_INSTANCE_METHOD(void, SetSliceProperties, Sombrero::FastVector3 cutPoint, Sombrero::FastVector3 cutNormal);

                      public
                      :

                      void FindObjects();

);
