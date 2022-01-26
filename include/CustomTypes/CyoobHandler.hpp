#pragma once

#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/MonoBehaviour.hpp"
#include "custom-types/shared/macros.hpp"

DECLARE_CLASS_CODEGEN(Qosmetics::Notes, CyoobHandler, UnityEngine::MonoBehaviour,
                      DECLARE_INSTANCE_FIELD(ArrayW<UnityEngine::GameObject*>, objects);
                      DECLARE_INSTANCE_FIELD(int, previouslyActive);

                      DECLARE_CTOR(ctor);

                      public
                      :

                      void ShowNote(bool right, bool dot);
                      void ShowNote(int obj);

                      void FindNotes();

)