#include "CustomTypes/NoteModelContainer.hpp"
#include "config.hpp"
#include "qosmetics-core/shared/Utils/BundleUtils.hpp"
#include "qosmetics-core/shared/Utils/ZipUtils.hpp"
#include "static-defines.hpp"

#include "UnityEngine/Transform.hpp"

DEFINE_TYPE(Qosmetics::Notes, NoteModelContainer);

using namespace UnityEngine;

namespace Qosmetics::Notes
{
    NoteModelContainer* NoteModelContainer::instance = nullptr;

    NoteModelContainer* NoteModelContainer::get_instance()
    {
        if (instance)
            return instance;
        auto go = GameObject::New_ctor(StringW(___TypeRegistration::get()->name()));
        Object::DontDestroyOnLoad(go);
        return go->AddComponent<NoteModelContainer*>();
    }

    void NoteModelContainer::ctor()
    {
        instance = this;
    }

    void NoteModelContainer::Start()
    {
        // TODO: Start loading last loaded object or something I guess
        auto lastUsedCyoob = Qosmetics::Notes::Config::config.lastUsedCyoob;
        if (lastUsedCyoob == "" || lastUsedCyoob == "Default")
            return;

        std::string filePath = string_format("%s/%s", cyoob_path, lastUsedCyoob.c_str());
        currentManifest = Qosmetics::Core::Manifest<NoteObjectConfig>(filePath);
        StartCoroutine(custom_types::Helpers::CoroutineHelper::New(LoadBundleRoutine()));
    }

    void NoteModelContainer::LoadObject(const Qosmetics::Core::Descriptor& descriptor)
    {
        currentManifest = Qosmetics::Core::Manifest<NoteObjectConfig>(descriptor.get_filePath());
        StartCoroutine(custom_types::Helpers::CoroutineHelper::New(LoadBundleRoutine()));
    }

    const NoteObjectConfig& NoteModelContainer::GetNoteConfig()
    {
        return currentManifest.get_config();
    }

    custom_types::Helpers::Coroutine NoteModelContainer::LoadBundleRoutine()
    {
        co_yield custom_types::Helpers::CoroutineHelper::New(Qosmetics::Core::BundleUtils::LoadBundleFromZipAsync(currentManifest.get_filePath(), currentManifest.get_fileName(), bundle));

        co_yield custom_types::Helpers::CoroutineHelper::New(Qosmetics::Core::BundleUtils::LoadAssetFromBundleAsync<UnityEngine::GameObject*>(bundle, "_Cyoob", currentNoteObject));

        auto name = currentNoteObject->get_name();
        currentNoteObject = UnityEngine::Object::Instantiate(currentNoteObject, get_transform());
        currentNoteObject->set_name(name);

        co_return;
    }

    void NoteModelContainer::OnDestroy()
    {
        instance = nullptr;
        UnloadBundle();
    }

    void NoteModelContainer::UnloadBundle()
    {
        if (bundle)
            bundle->Unload(false);
        bundle = nullptr;
    }
}