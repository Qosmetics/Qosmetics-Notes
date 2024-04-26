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
                      DECLARE_INSTANCE_FIELD_PRIVATE(UnityEngine::AssetBundle*, bundle);
                      DECLARE_INSTANCE_FIELD_PRIVATE(bool, isLoading);
                      DECLARE_INSTANCE_FIELD_PRIVATE(UnityEngine::GameObject*, currentNoteObject);

                      DECLARE_CTOR(ctor);
                      public
                      :

                      using Manifest = Qosmetics::Core::Manifest<Qosmetics::Notes::NoteObjectConfig>;
                      __declspec(property(get = get_CurrentNoteObject)) UnityEngine::GameObject * CurrentNoteObject;
                      UnityEngine::GameObject * get_CurrentNoteObject() const;

                      static NoteModelContainer * get_instance();
                      bool LoadObject(const Manifest& manifest, std::function<void(NoteModelContainer*)> onFinished);
                      bool LoadObject(const Qosmetics::Core::Descriptor& descriptor, std::function<void(NoteModelContainer*)> onFinished = nullptr);
                      const NoteObjectConfig& GetNoteConfig();
                      const Qosmetics::Core::Descriptor& GetDescriptor();
                      bool Default();

                      private
                      :

                      custom_types::Helpers::Coroutine LoadBundleRoutine(std::function<void(NoteModelContainer*)> onFinished);
                      void Unload();

                      Manifest currentManifest;

                      static NoteModelContainer * instance;

)
