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
#include "CustomTypes/ChainHandler.hpp"
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

void SetLayerRecursively(UnityEngine::Transform* obj, int layer)
{
    obj->get_gameObject()->set_layer(layer);
    int childCount = obj->get_childCount();
    for (int i = 0; i < childCount; i++)
        SetLayerRecursively(obj->GetChild(i), layer);
}

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

void DuplicateForMirror(UnityEngine::Transform* parent, StringW origName, StringW mirroredName)
{
    auto orig = parent->Find(origName)->get_gameObject();
    auto mirrored = UnityEngine::Object::Instantiate(orig, parent->get_transform());
    mirrored->set_name(mirroredName);

    SetMirrorableProperties(orig, false);
    SetMirrorableProperties(mirrored, true);
}

void MirrorableFixups(UnityEngine::GameObject* loadedObject)
{
    auto t = loadedObject->get_transform();

    DuplicateForMirror(t, ConstStrings::Notes(), ConstStrings::MirrorNotes());
    DuplicateForMirror(t, ConstStrings::Bomb(), ConstStrings::MirrorBomb());
    DuplicateForMirror(t, ConstStrings::Chains(), ConstStrings::MirrorChains());
}

UnityEngine::Transform* MakeSliderNote(UnityEngine::Transform* parent, UnityEngine::GameObject* orig, StringW name, const Sombrero::FastVector3& localPosition, const Sombrero::FastVector3& localScale)
{
    DEBUG("Making {}", name);
    auto noteParent = GameObject::New_ctor(name)->get_transform();
    noteParent->set_parent(parent);
    noteParent->set_localScale(Sombrero::FastVector3::one());
    noteParent->set_localPosition(Sombrero::FastVector3::zero());
    auto note = Object::Instantiate(orig, noteParent)->get_transform();
    note->set_name(name);
    note->set_localPosition(localPosition);
    note->set_localScale(localScale);
    return noteParent;
}

UnityEngine::Transform* MakeSliderDebris(UnityEngine::Transform* parent, UnityEngine::GameObject* orig, StringW name, const Sombrero::FastVector3& localScale)
{
    auto debrisParent = GameObject::New_ctor(name)->get_transform();
    debrisParent->set_parent(parent);
    debrisParent->set_localPosition(Sombrero::FastVector3::zero());
    debrisParent->set_localScale(Sombrero::FastVector3::one());
    auto debris = Object::Instantiate(orig, debrisParent)->get_transform();
    debris->set_name(name);
    debris->set_localScale(localScale);
    debris->set_localPosition(Sombrero::FastVector3::zero());
    return debrisParent;
}

void SliderFixups(UnityEngine::GameObject* loadedObject)
{
    auto t = loadedObject->get_transform();

    auto notes = t->Find(ConstStrings::Notes());
    auto leftArrow = notes->Find(ConstStrings::LeftArrow());
    auto rightArrow = notes->Find(ConstStrings::RightArrow());
    auto leftDot = notes->Find(ConstStrings::LeftDot());
    auto rightDot = notes->Find(ConstStrings::RightDot());

    DEBUG("Checking if chain existed");
    auto chains = t->Find(ConstStrings::Chains());
    if (!chains)
    {
        DEBUG("Making chain");
        chains = GameObject::New_ctor(ConstStrings::Chains())->get_transform();
        chains->SetParent(t);
    }

    auto leftHead = chains->Find(ConstStrings::LeftHead());
    auto rightHead = chains->Find(ConstStrings::RightHead());
    auto leftLink = chains->Find(ConstStrings::LeftLink());
    auto rightLink = chains->Find(ConstStrings::RightLink());

    if (!leftHead)
        leftHead = MakeSliderNote(chains, leftArrow->get_gameObject(), ConstStrings::LeftHead(), {0.0f, 0.125f, 0.0f}, {1.0f, 0.75f, 1.0f});

    if (!rightHead)
        rightHead = MakeSliderNote(chains, rightArrow->get_gameObject(), ConstStrings::RightHead(), {0.0f, 0.125f, 0.0f}, {1.0f, 0.75f, 1.0f});

    if (!leftLink)
        leftLink = MakeSliderNote(chains, leftDot->get_gameObject(), ConstStrings::LeftLink(), Sombrero::FastVector3::zero(), {1.0f, 0.2f, 1.0f});

    if (!rightLink)
        rightLink = MakeSliderNote(chains, rightDot->get_gameObject(), ConstStrings::RightLink(), Sombrero::FastVector3::zero(), {1.0f, 0.2f, 1.0f});

    DEBUG("Checking if debris exists");
    auto debris = t->Find(ConstStrings::Debris());
    // if we don't even have regular debris, it's not worth actually making it
    if (debris)
    {
        auto chainHeadDebris = t->Find(ConstStrings::ChainHeadDebris());
        auto chainLinkDebris = t->Find(ConstStrings::ChainLinkDebris());

        auto leftDebris = debris->Find(ConstStrings::LeftDebris());
        auto rightDebris = debris->Find(ConstStrings::RightDebris());

        if (!chainHeadDebris && leftDebris && rightDebris)
        {
            DEBUG("Making chainHeadDebris");
            chainHeadDebris = GameObject::New_ctor(ConstStrings::ChainHeadDebris())->get_transform();
            chainHeadDebris->set_parent(t);
            chainHeadDebris->set_localPosition(Sombrero::FastVector3::zero());

            MakeSliderDebris(chainHeadDebris, leftDebris->get_gameObject(), ConstStrings::LeftDebris(), {1.0f, 0.75f, 1.0f});
            MakeSliderDebris(chainHeadDebris, rightDebris->get_gameObject(), ConstStrings::RightDebris(), {1.0f, 0.75f, 1.0f});
        }

        if (!chainLinkDebris && leftDebris && rightDebris)
        {
            DEBUG("Making chainLinkDebris");
            chainLinkDebris = GameObject::New_ctor(ConstStrings::ChainLinkDebris())->get_transform();
            chainLinkDebris->set_parent(t);
            chainLinkDebris->set_localPosition(Sombrero::FastVector3::zero());

            MakeSliderDebris(chainLinkDebris, leftDebris->get_gameObject(), ConstStrings::LeftDebris(), {1.0f, 0.2f, 1.0f});
            MakeSliderDebris(chainLinkDebris, rightDebris->get_gameObject(), ConstStrings::RightDebris(), {1.0f, 0.2f, 1.0f});
        }
    }
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

    auto ct = t->Find(ConstStrings::Chains());
    ct->get_gameObject()->AddComponent<Qosmetics::Notes::ChainHandler*>();
    auto mct = t->Find(ConstStrings::MirrorChains());
    if (mct)
        mct->get_gameObject()->AddComponent<Qosmetics::Notes::ChainHandler*>();
    for (int i = 0; i < 4; i++)
    {
        ct->GetChild(i)->get_gameObject()->AddComponent<Qosmetics::Notes::CyoobColorHandler*>();
        if (mct)
            mct->GetChild(i)->get_gameObject()->AddComponent<Qosmetics::Notes::CyoobColorHandler*>();
    }

    auto bt = t->Find(ConstStrings::Bomb());
    auto mbt = t->Find(ConstStrings::MirrorBomb());
    if (bt)
        bt->get_gameObject()->AddComponent<Qosmetics::Notes::BombColorHandler*>();
    if (mbt)
        mbt->get_gameObject()->AddComponent<Qosmetics::Notes::BombColorHandler*>();

    auto dbt = t->Find(ConstStrings::Debris());
    if (dbt)
    {
        dbt->get_gameObject()->AddComponent<Qosmetics::Notes::DebrisHandler*>();

        dbt->GetChild(0)->get_gameObject()->AddComponent<Qosmetics::Notes::DebrisColorHandler*>();
        dbt->GetChild(1)->get_gameObject()->AddComponent<Qosmetics::Notes::DebrisColorHandler*>();
        SetLayerRecursively(dbt, 9);
    }

    auto chdbt = t->Find(ConstStrings::ChainHeadDebris());
    if (chdbt)
    {
        chdbt->get_gameObject()->AddComponent<Qosmetics::Notes::DebrisHandler*>();

        chdbt->GetChild(0)->get_gameObject()->AddComponent<Qosmetics::Notes::DebrisColorHandler*>();
        chdbt->GetChild(1)->get_gameObject()->AddComponent<Qosmetics::Notes::DebrisColorHandler*>();
        SetLayerRecursively(chdbt, 9);
    }

    auto cldbt = t->Find(ConstStrings::ChainLinkDebris());
    if (cldbt)
    {
        cldbt->get_gameObject()->AddComponent<Qosmetics::Notes::DebrisHandler*>();

        cldbt->GetChild(0)->get_gameObject()->AddComponent<Qosmetics::Notes::DebrisColorHandler*>();
        cldbt->GetChild(1)->get_gameObject()->AddComponent<Qosmetics::Notes::DebrisColorHandler*>();

        SetLayerRecursively(cldbt, 9);
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
        INFO("Loading Note Object {}", currentManifest.get_descriptor().get_name());
        StartCoroutine(custom_types::Helpers::CoroutineHelper::New(LoadBundleRoutine(nullptr)));
    }

    bool NoteModelContainer::LoadObject(const Qosmetics::Core::Manifest<Qosmetics::Notes::NoteObjectConfig>& manifest, std::function<void(NoteModelContainer*)> onFinished)
    {
        if (isLoading)
            return false;
        if (manifest.get_filePath() == currentManifest.get_filePath())
            return false;
        isLoading = true;
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

    custom_types::Helpers::Coroutine NoteModelContainer::LoadBundleRoutine(std::function<void(NoteModelContainer*)> onFinished)
    {
        isLoading = true;
        if (currentNoteObject)
            Object::DestroyImmediate(currentNoteObject);
        if (bundle)
            bundle->Unload(true);

        DEBUG("Loading file {} from cyoob {}", currentManifest.get_fileName(), currentManifest.get_filePath());
        co_yield custom_types::Helpers::CoroutineHelper::New(Qosmetics::Core::BundleUtils::LoadBundleFromZipAsync(currentManifest.get_filePath(), currentManifest.get_fileName(), bundle));

        bool isLegacy = currentManifest.get_config().get_isLegacy();
        DEBUG("Loading {}Cyoob", isLegacy ? "legacy " : "");
        co_yield custom_types::Helpers::CoroutineHelper::New(Qosmetics::Core::BundleUtils::LoadAssetFromBundleAsync<UnityEngine::GameObject*>(bundle, isLegacy ? "_CustomBloq" : "_Cyoob", currentNoteObject));

        auto name = currentNoteObject->get_name();
        currentNoteObject = UnityEngine::Object::Instantiate(currentNoteObject, get_transform());
        SetLayerRecursively(currentNoteObject->get_transform(), 8);
        currentNoteObject->set_name(name);
        currentNoteObject->SetActive(false);

        if (isLegacy)
        {
            DEBUG("Executing legacy object fixups");
            LegacyFixups(currentNoteObject);
        }

        if (!currentManifest.get_config().get_hasSlider())
        {
            DEBUG("Fixing missing sliders due to the model not having them");
            SliderFixups(currentNoteObject);
        }

        if (currentManifest.get_config().get_isMirrorable())
        {
            DEBUG("Pre-Creating mirrorable objects");
            MirrorableFixups(currentNoteObject);
        }

        DEBUG("Adding handlers to object");
        AddHandlers(currentNoteObject);

        if (onFinished)
            onFinished(this);

        isLoading = false;
        co_return;
    }

    bool NoteModelContainer::Default()
    {
        if (isLoading)
            return false;
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
        return true;
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
        if (currentNoteObject && currentNoteObject->m_CachedPtr.m_value)
            Object::DestroyImmediate(currentNoteObject);
        currentNoteObject = nullptr;
        if (bundle && bundle->m_CachedPtr.m_value)
            bundle->Unload(true);
        bundle = nullptr;

        instance = nullptr;
        UnityEngine::Object::DestroyImmediate(this->get_gameObject());
    }

}