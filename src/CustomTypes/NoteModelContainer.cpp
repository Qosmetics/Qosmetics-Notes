#include "CustomTypes/NoteModelContainer.hpp"
#include "ConstStrings.hpp"
#include "PropertyID.hpp"
#include "config.hpp"
#include "logging.hpp"
#include "qosmetics-core/shared/Utils/BundleUtils.hpp"
#include "qosmetics-core/shared/Utils/FileUtils.hpp"
#include "qosmetics-core/shared/Utils/ZipUtils.hpp"
#include "static-defines.hpp"

#include "CustomTypes/BombColorHandler.hpp"
#include "CustomTypes/CyoobColorHandler.hpp"
#include "CustomTypes/CyoobHandler.hpp"
#include "CustomTypes/DebrisColorHandler.hpp"
#include "CustomTypes/DebrisHandler.hpp"

#include "GlobalNamespace/SharedCoroutineStarter.hpp"
#include "UnityEngine/Coroutine.hpp"
#include "UnityEngine/Material.hpp"
#include "UnityEngine/MeshRenderer.hpp"
#include "UnityEngine/Transform.hpp"

#include <fmt/format.h>

DEFINE_TYPE(Qosmetics::Notes, NoteModelContainer);

#define STARTCOROUTINE(routine) GlobalNamespace::SharedCoroutineStarter::get_instance()->StartCoroutine(routine)

using namespace UnityEngine;

void LegacyFixups(UnityEngine::GameObject* loadedObject)
{
    auto t = loadedObject->get_transform();
    auto notes = GameObject::New_ctor(ConstStrings::Notes());
    auto nt = notes->get_transform();
    nt->SetParent(t, false);

    auto leftArrow = t->Find(ConstStrings::LeftArrow());
    auto rightArrow = t->Find(ConstStrings::RightArrow());
    auto leftDot = t->Find(ConstStrings::LeftDot());
    auto rightDot = t->Find(ConstStrings::RightDot());

    leftArrow->SetParent(nt, false);
    rightArrow->SetParent(nt, false);
    leftDot->SetParent(nt, false);
    rightDot->SetParent(nt, false);

    auto leftDebris = t->Find(ConstStrings::LeftDebris());
    auto rightDebris = t->Find(ConstStrings::RightDebris());
    if (leftDebris && rightDebris)
    {
        auto debris = GameObject::New_ctor(ConstStrings::Debris());
        auto dt = debris->get_transform();
        dt->SetParent(t, false);
        leftDebris->SetParent(dt, false);
        rightDebris->SetParent(dt, false);
    }
}

void SetMirrorableProperties(UnityEngine::GameObject* loadedObject, bool mirror)
{
    auto renderers = loadedObject->GetComponentsInChildren<UnityEngine::MeshRenderer*>(true);

    for (auto renderer : renderers)
    {
        auto materials = renderer->get_materials();

        for (auto material : materials)
        {
            material->set_renderQueue(mirror ? 1951 : 2004);
            if (material->HasProperty(PropertyID::_Alpha()))
                material->SetFloat(PropertyID::_Alpha(), mirror ? 0.05f : 1.0f);
            if (material->HasProperty(PropertyID::_StencilRefID()))
                material->SetFloat(PropertyID::_StencilRefID(), mirror ? 1 : 0);
            if (material->HasProperty(PropertyID::_StencilComp()))
                material->SetFloat(PropertyID::_StencilComp(), mirror ? 3 : 8);
            if (material->HasProperty(PropertyID::_StencilOp()))
                material->SetFloat(PropertyID::_StencilOp(), 0);

            if (material->HasProperty(PropertyID::_BlendDstFactor()))
                material->SetFloat(PropertyID::_BlendDstFactor(), mirror ? 10 : 0);
            if (material->HasProperty(PropertyID::_BlendDstFactorA()))
                material->SetFloat(PropertyID::_BlendDstFactorA(), 0);
            if (material->HasProperty(PropertyID::_BlendSrcFactor()))
                material->SetFloat(PropertyID::_BlendSrcFactor(), mirror ? 5 : 1);
            if (material->HasProperty(PropertyID::_BlendSrcFactorA()))
                material->SetFloat(PropertyID::_BlendSrcFactorA(), 0);
        }
    }
}

void MirrorableFixups(UnityEngine::GameObject* loadedObject)
{
    auto t = loadedObject->get_transform();
    auto notes = t->Find(ConstStrings::Notes())->get_gameObject();
    auto mirroredNotes = UnityEngine::Object::Instantiate(notes, t->get_transform());
    mirroredNotes->set_name(ConstStrings::MirrorNotes());

    SetMirrorableProperties(notes, false);
    SetMirrorableProperties(mirroredNotes, true);

    auto bomb = t->Find(ConstStrings::Bomb())->get_gameObject();
    auto mirroredBomb = UnityEngine::Object::Instantiate(bomb, t->get_transform());
    mirroredNotes->set_name(ConstStrings::MirrorBomb());
    SetMirrorableProperties(bomb, false);
    SetMirrorableProperties(mirroredBomb, true);
}

void AddHandlers(UnityEngine::GameObject* loadedObject)
{
    auto t = loadedObject->get_transform();
    auto nt = t->Find(ConstStrings::Notes());
    auto mnt = t->Find(ConstStrings::MirrorNotes());

    nt->get_gameObject()->AddComponent<Qosmetics::Notes::CyoobHandler*>();
    if (mnt)
        mnt->get_gameObject()->AddComponent<Qosmetics::Notes::CyoobHandler*>();

    for (int i = 0; i < 4; i++)
    {
        nt->GetChild(i)->get_gameObject()->AddComponent<Qosmetics::Notes::CyoobColorHandler*>();
        if (mnt)
            mnt->GetChild(i)->get_gameObject()->AddComponent<Qosmetics::Notes::CyoobColorHandler*>();
    }

    auto dbt = t->Find(ConstStrings::Debris());
    if (dbt)
    {
        dbt->get_gameObject()->AddComponent<Qosmetics::Notes::DebrisHandler*>();

        dbt->GetChild(0)->get_gameObject()->AddComponent<Qosmetics::Notes::DebrisColorHandler*>();
        dbt->GetChild(1)->get_gameObject()->AddComponent<Qosmetics::Notes::DebrisColorHandler*>();
    }

    auto bt = t->Find(ConstStrings::Bomb());
    auto mbt = t->Find(ConstStrings::Bomb());
    if (bt)
        bt->get_gameObject()->AddComponent<Qosmetics::Notes::BombColorHandler*>();
    if (mbt)
        mbt->get_gameObject()->AddComponent<Qosmetics::Notes::BombColorHandler*>();
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
        DEBUG("Created NoteModelContainer instance: {}", fmt::ptr(this));
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

        std::string filePath = fmt::format("{}/{}.cyoob", cyoob_path, lastUsedCyoob);
        if (!fileexists(filePath))
            return;
        currentManifest = Qosmetics::Core::Manifest<NoteObjectConfig>(filePath);
        currentManifest.get_descriptor();
        INFO("Loading Note Object {}", currentManifest.get_descriptor().get_name());
        StartCoroutine(custom_types::Helpers::CoroutineHelper::New(LoadBundleRoutine(nullptr)));
    }

    bool NoteModelContainer::LoadObject(const Qosmetics::Core::Manifest<Qosmetics::Notes::NoteObjectConfig>& manifest, std::function<void(NoteModelContainer*)> onFinished)
    {
        if (isLoading)
            return false;
        if (manifest.get_filePath() == currentManifest.get_filePath())
            return false;
        currentManifest = manifest;
        StartCoroutine(custom_types::Helpers::CoroutineHelper::New(LoadBundleRoutine(onFinished)));
        return true;
    }

    bool NoteModelContainer::LoadObject(const Qosmetics::Core::Descriptor& descriptor, std::function<void(NoteModelContainer*)> onFinished)
    {
        if (isLoading)
            return false;
        if (descriptor.get_filePath() == currentManifest.get_filePath())
            return false;
        INFO("Loading Note Object {}", descriptor.get_name());
        currentManifest = Qosmetics::Core::Manifest<NoteObjectConfig>(descriptor.get_filePath());
        StartCoroutine(custom_types::Helpers::CoroutineHelper::New(LoadBundleRoutine(onFinished)));
        return true;
    }

    const NoteObjectConfig& NoteModelContainer::GetNoteConfig()
    {
        return currentManifest.get_config();
    }

    const Qosmetics::Core::Descriptor& NoteModelContainer::GetDescriptor()
    {
        return currentManifest.get_descriptor();
    }

    /*
    const Qosmetics::Core::Manifest<Qosmetics::Notes::NoteObjectConfig>& NoteModelContainer::GetManifest()
    {
        DEBUG("{} currentManifest: {}", this, &currentManifest);
        auto& descriptor = currentManifest.get_descriptor();
        DEBUG("{} descriptor: {}", this, &descriptor);
        return currentManifest;
    }
    */
    custom_types::Helpers::Coroutine NoteModelContainer::LoadBundleRoutine(std::function<void(NoteModelContainer*)> onFinished)
    {
        isLoading = true;
        if (currentNoteObject)
            Object::DestroyImmediate(currentNoteObject);
        if (bundle)
            bundle->Unload(true);

        co_yield custom_types::Helpers::CoroutineHelper::New(Qosmetics::Core::BundleUtils::LoadBundleFromZipAsync(currentManifest.get_filePath(), currentManifest.get_fileName(), bundle));

        bool isLegacy = currentManifest.get_config().get_isLegacy();
        DEBUG("Loading {}Cyoob", isLegacy ? "legacy " : "");
        co_yield custom_types::Helpers::CoroutineHelper::New(Qosmetics::Core::BundleUtils::LoadAssetFromBundleAsync<UnityEngine::GameObject*>(bundle, isLegacy ? "_CustomBloq" : "_Cyoob", currentNoteObject));

        auto name = currentNoteObject->get_name();
        currentNoteObject->set_layer(8);
        currentNoteObject = UnityEngine::Object::Instantiate(currentNoteObject, get_transform());
        currentNoteObject->set_name(name);
        currentNoteObject->SetActive(false);

        if (isLegacy)
        {
            DEBUG("Executing legacy object fixups");
            LegacyFixups(currentNoteObject);
        }

        if (currentManifest.get_config().get_isMirrorable())
        {
            DEBUG("Pre-Creating mirrorable objects");
            MirrorableFixups(currentNoteObject);
        }

        DEBUG("Adding handlers to object");
        AddHandlers(currentNoteObject);

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

        currentManifest = decltype(currentManifest)();
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

    void NoteModelContainer::OnGameRestart()
    {
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

        UnityEngine::Object::Destroy(this->get_gameObject());
        instance = nullptr;
    }

}