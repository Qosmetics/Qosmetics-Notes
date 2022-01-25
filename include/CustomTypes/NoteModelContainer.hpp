#pragma once

#include "NoteObjectConfig.hpp"
#include "UnityEngine/AssetBundle.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/MonoBehaviour.hpp"
#include "custom-types/shared/coroutine.hpp"
#include "custom-types/shared/macros.hpp"
#include "qosmetics-core/shared/Data/Descriptor.hpp"
#include "qosmetics-core/shared/Data/Manifest.hpp"

DECLARE_CLASS_CODEGEN(Qosmetics::Notes, NoteModelContainer, UnityEngine::MonoBehaviour,
                      DECLARE_INSTANCE_METHOD(void, Start);
                      DECLARE_INSTANCE_METHOD(void, OnDestroy);
                      DECLARE_INSTANCE_FIELD(UnityEngine::AssetBundle*, bundle);
                      DECLARE_INSTANCE_FIELD(bool, isLoading);
                      DECLARE_INSTANCE_FIELD(UnityEngine::GameObject*, currentNoteObject);

                      DECLARE_CTOR(ctor);
                      public
                      :

                      NoteModelContainer * get_instance();
                      void LoadObject(const Qosmetics::Core::Descriptor& descriptor);
                      const NoteObjectConfig& GetNoteConfig();

                      private
                      :

                      custom_types::Helpers::Coroutine LoadBundleRoutine();
                      void UnloadBundle();

                      Qosmetics::Core::Manifest<Qosmetics::Notes::NoteObjectConfig> currentManifest;

                      static NoteModelContainer * instance;

)
