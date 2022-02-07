#include "CustomTypes/NoteModelContainer.hpp"
#include "config.hpp"
#include "logging.hpp"
#include "qosmetics-core/shared/Utils/BundleUtils.hpp"
#include "qosmetics-core/shared/Utils/FileUtils.hpp"
#include "qosmetics-core/shared/Utils/ZipUtils.hpp"
#include "static-defines.hpp"

#include "GlobalNamespace/SharedCoroutineStarter.hpp"
#include "UnityEngine/Coroutine.hpp"
#include "UnityEngine/Transform.hpp"

DEFINE_TYPE(Qosmetics::Notes, NoteModelContainer);

#define STARTCOROUTINE(routine) GlobalNamespace::SharedCoroutineStarter::get_instance()->StartCoroutine(routine)

using namespace UnityEngine;

void LegacyFixups(UnityEngine::GameObject* loadedObject)
{
    static auto Bomb = ConstString("Bomb");
    static auto LeftArrow = ConstString("LeftArrow");
    static auto RightArrow = ConstString("RightArrow");
    static auto LeftDot = ConstString("LeftDot");
    static auto RightDot = ConstString("RightDot");
    static auto LeftDebris = ConstString("LeftDebris");
    static auto RightDebris = ConstString("RightDebris");
    static auto Notes = ConstString("Notes");

    auto t = loadedObject->get_transform();
    auto notes = GameObject::New_ctor(Notes);
    auto nt = notes->get_transform();
    nt->SetParent(t, false);

    auto leftArrow = t->Find(LeftArrow);
    auto rightArrow = t->Find(RightArrow);
    auto leftDot = t->Find(LeftDot);
    auto rightDot = t->Find(RightDot);

    leftArrow->SetParent(nt, false);
    rightArrow->SetParent(nt, false);
    leftDot->SetParent(nt, false);
    rightDot->SetParent(nt, false);

    auto leftDebris = t->Find(LeftDebris);
    auto rightDebris = t->Find(RightDebris);
    if (leftDebris && rightDebris)
    {
        static auto Debris = ConstString("Debris");
        auto debris = GameObject::New_ctor(Debris);
        auto dt = debris->get_transform();
        dt->SetParent(t, false);
        leftDebris->SetParent(dt, false);
        rightDebris->SetParent(dt, false);
    }
}

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
        bundle = nullptr;
        isLoading = false;
        currentNoteObject = nullptr;
        currentManifest = Qosmetics::Core::Manifest<Qosmetics::Notes::NoteObjectConfig>();
    }

    void NoteModelContainer::Start()
    {
        auto lastUsedCyoob = Qosmetics::Notes::Config::get_config().lastUsedCyoob;
        if (lastUsedCyoob == "" || lastUsedCyoob == "Default")
            return;

        std::string filePath = string_format("%s/%s.cyoob", cyoob_path, lastUsedCyoob.c_str());
        if (!fileexists(filePath))
            return;
        currentManifest = Qosmetics::Core::Manifest<NoteObjectConfig>(filePath);
        INFO("Loading Note Object %s", currentManifest.get_descriptor().get_name().data());
        StartCoroutine(custom_types::Helpers::CoroutineHelper::New(LoadBundleRoutine(nullptr)));
    }

    void NoteModelContainer::LoadObject(const Qosmetics::Core::Descriptor& descriptor, std::function<void(NoteModelContainer*)> onFinished)
    {
        if (isLoading)
            return;
        if (descriptor.get_filePath() == currentManifest.get_filePath())
            return;
        INFO("Loading Note Object %s", descriptor.get_name().data());
        currentManifest = Qosmetics::Core::Manifest<NoteObjectConfig>(descriptor.get_filePath());
        StartCoroutine(custom_types::Helpers::CoroutineHelper::New(LoadBundleRoutine(onFinished)));
    }

    const NoteObjectConfig& NoteModelContainer::GetNoteConfig()
    {
        return currentManifest.get_config();
    }

    custom_types::Helpers::Coroutine NoteModelContainer::LoadBundleRoutine(std::function<void(NoteModelContainer*)> onFinished)
    {
        isLoading = true;
        if (currentNoteObject)
            Object::DestroyImmediate(currentNoteObject);
        if (bundle)
            bundle->Unload(true);

        co_yield custom_types::Helpers::CoroutineHelper::New(Qosmetics::Core::BundleUtils::LoadBundleFromZipAsync(currentManifest.get_filePath(), currentManifest.get_fileName(), bundle));

        bool isLegacy = currentManifest.get_config().get_isLegacy();
        DEBUG("Loading %sCyoob", isLegacy ? "legacy " : "");
        co_yield custom_types::Helpers::CoroutineHelper::New(Qosmetics::Core::BundleUtils::LoadAssetFromBundleAsync<UnityEngine::GameObject*>(bundle, isLegacy ? "_CustomBloq" : "_Cyoob", currentNoteObject));

        auto name = currentNoteObject->get_name();
        currentNoteObject->set_layer(8);
        currentNoteObject = UnityEngine::Object::Instantiate(currentNoteObject, get_transform());
        currentNoteObject->set_name(name);
        if (isLegacy)
            LegacyFixups(currentNoteObject);

        currentNoteObject->SetActive(false);

        isLoading = false;
        if (onFinished)
            onFinished(this);

        co_return;
    }

    void NoteModelContainer::Default()
    {
        if (isLoading)
            return;
        if (currentNoteObject)
        {
            Object::DestroyImmediate(currentNoteObject);
            currentNoteObject = nullptr;
        }
        if (bundle)
        {
            bundle->Unload(true);
            bundle = nullptr;
        }
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