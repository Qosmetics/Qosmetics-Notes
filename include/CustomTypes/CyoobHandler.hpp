#pragma once

#include "CustomTypes/CyoobColorHandler.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/MonoBehaviour.hpp"
#include "custom-types/shared/macros.hpp"

DECLARE_CLASS_CODEGEN(Qosmetics::Notes, CyoobHandler, UnityEngine::MonoBehaviour,
                      DECLARE_INSTANCE_FIELD(ArrayW<UnityEngine::GameObject*>, objects);
                      DECLARE_INSTANCE_FIELD(ArrayW<CyoobColorHandler*>, colorHandlers);
                      DECLARE_INSTANCE_FIELD(int, previouslyActive);

                      DECLARE_CTOR(ctor);

                      public
                      :

                      void ShowNote(bool right, bool dot);
                      void ShowNote(int obj);
                      void SetColors(UnityEngine::Color thisColor, UnityEngine::Color thatColor);

                      void FindNotes();

)