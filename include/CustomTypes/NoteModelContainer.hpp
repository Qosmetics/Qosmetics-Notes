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
                      DECLARE_INSTANCE_METHOD(void, OnGameRestart);
                      DECLARE_INSTANCE_FIELD(UnityEngine::AssetBundle*, bundle);
                      DECLARE_INSTANCE_FIELD(bool, isLoading);
                      DECLARE_INSTANCE_FIELD(UnityEngine::GameObject*, currentNoteObject);

                      DECLARE_CTOR(ctor);
                      public
                      :

                      static NoteModelContainer * get_instance();
                      bool LoadObject(const Qosmetics::Core::Descriptor& descriptor, std::function<void(NoteModelContainer*)> onFinished = nullptr);
                      const NoteObjectConfig& GetNoteConfig();
                      const Qosmetics::Core::Descriptor& GetDescriptor();
                      void Default();

                      private
                      :

                      custom_types::Helpers::Coroutine LoadBundleRoutine(std::function<void(NoteModelContainer*)> onFinished);
                      void UnloadBundle();

                      Qosmetics::Core::Manifest<Qosmetics::Notes::NoteObjectConfig> currentManifest;

                      static NoteModelContainer * instance;

)
