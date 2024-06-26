#pragma once

#include "CustomTypes/CyoobColorHandler.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/MonoBehaviour.hpp"
#include "custom-types/shared/macros.hpp"

DECLARE_CLASS_CODEGEN(Qosmetics::Notes, CyoobHandler, UnityEngine::MonoBehaviour,
                      DECLARE_INSTANCE_FIELD_PRIVATE(ArrayW<UnityEngine::GameObject*>, objects);
                      DECLARE_INSTANCE_FIELD_PRIVATE(ArrayW<CyoobColorHandler*>, colorHandlers);
                      DECLARE_INSTANCE_FIELD_PRIVATE(int, previouslyActive);

                      DECLARE_CTOR(ctor);

                      public
                      :

                      void ShowNote(bool right, bool dot);
                      void ShowNote(int obj);
                      void SetColors(UnityEngine::Color leftColor, UnityEngine::Color rightColor);

                      void FindNotes();

)
