#include "GlobalNamespace/BeatmapObjectsInstaller.hpp"
#include "GlobalNamespace/BombNoteController.hpp"
#include "GlobalNamespace/BoxCuttableBySaber.hpp"
#include "GlobalNamespace/ColorScheme.hpp"
#include "GlobalNamespace/EffectPoolsManualInstaller.hpp"
#include "GlobalNamespace/FakeMirrorObjectsInstaller.hpp"
#include "GlobalNamespace/GameNoteController.hpp"
#include "GlobalNamespace/GameplayCoreSceneSetupData.hpp"
#include "GlobalNamespace/GameplayModifiers.hpp"
#include "GlobalNamespace/MirroredBombNoteController.hpp"
#include "GlobalNamespace/MirroredGameNoteController.hpp"
#include "GlobalNamespace/MirroredSliderController.hpp"
#include "GlobalNamespace/NoteDebris.hpp"
#include "GlobalNamespace/NoteDebrisPoolInstaller.hpp"
#include "GlobalNamespace/PlayerSpecificSettings.hpp"

#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/Material.hpp"
#include "UnityEngine/MeshFilter.hpp"
#include "UnityEngine/MeshRenderer.hpp"
#include "UnityEngine/Shader.hpp"
#include "UnityEngine/SphereCollider.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/Vector3.hpp"
#include "qosmetics-core/shared/RedecorationRegister.hpp"
#include "sombrero/shared/FastQuaternion.hpp"
#include "sombrero/shared/FastVector3.hpp"

#include "ConstStrings.hpp"
#include "CustomTypes/BasicNoteScaler.hpp"
#include "CustomTypes/BombColorHandler.hpp"
#include "CustomTypes/BombParent.hpp"
#include "CustomTypes/CyoobColorHandler.hpp"
#include "CustomTypes/CyoobHandler.hpp"
#include "CustomTypes/CyoobParent.hpp"
#include "CustomTypes/DebrisColorHandler.hpp"
#include "CustomTypes/DebrisHandler.hpp"
#include "CustomTypes/DebrisParent.hpp"
#include "CustomTypes/NoteModelContainer.hpp"
#include "MaterialUtils.hpp"
#include "PropertyID.hpp"

#include "config.hpp"
#include "logging.hpp"

#include "Disabling.hpp"

#if __has_include("chroma/shared/NoteAPI.hpp")
#include "chroma/shared/BombAPI.hpp"
#include "chroma/shared/NoteAPI.hpp"
#ifndef CHROMA_EXISTS
#define CHROMA_EXISTS
#endif
#endif

// just a macro to quickly get all the relevant config stuff because it was redundant to type every time and it's the same across everywhere but I can't cahce it into a method or something cause it's like 10 things
#define GET_CONFIG()                                                                                                                    \
    auto noteModelContainer = Qosmetics::Notes::NoteModelContainer::get_instance();                                                     \
    auto& config = noteModelContainer->GetNoteConfig();                                                                                 \
    auto& globalConfig = Qosmetics::Notes::Config::get_config();                                                                        \
    auto gameplayCoreSceneSetupData = container->TryResolve<GlobalNamespace::GameplayCoreSceneSetupData*>();                            \
    auto gameplayModifiers = gameplayCoreSceneSetupData->dyn_gameplayModifiers();                                                       \
    float noteSizeFactor = (globalConfig.overrideNoteSize ? globalConfig.noteSize : 1.0f) * gameplayModifiers->get_notesUniformScale(); \
    bool ghostNotes = gameplayModifiers->get_ghostNotes();                                                                              \
    bool disappearingArrows = gameplayModifiers->get_disappearingArrows();

#pragma region bombs
REDECORATION_REGISTRATION(bombNotePrefab, 10, true, GlobalNamespace::BombNoteController*, GlobalNamespace::BeatmapObjectsInstaller*)
{
    if (Qosmetics::Notes::Disabling::GetAnyDisabling())
        return bombNotePrefab;
    try
    {
        GET_CONFIG()
        auto mesh = bombNotePrefab->get_transform()->Find("Mesh");

        // if we have a note object, and a bomb exists on it, and we don't force default
        bool flag = noteModelContainer->currentNoteObject && config.get_hasBomb() && !globalConfig.forceDefaultBombs;
#ifdef CHROMA_EXISTS
        Chroma::BombAPI::setBombColorable(flag);
#endif
        if (flag)
        {
            bombNotePrefab->get_gameObject()->AddComponent<Qosmetics::Notes::BombParent*>();

#ifdef CHROMA_EXISTS
            auto bombCallbackOpt = Chroma::BombAPI::getBombChangedColorCallbackSafe();
            if (bombCallbackOpt)
            {
                INFO("Adding to note color callback!");
                auto& bombCallback = bombCallbackOpt.value().get();
                bombCallback -= &Qosmetics::Notes::BombParent::ColorizeSpecific;
                bombCallback += &Qosmetics::Notes::BombParent::ColorizeSpecific;
            }
#endif

            auto bombPrefab = noteModelContainer->currentNoteObject->get_transform()->Find(ConstStrings::Bomb());
            auto bomb = UnityEngine::Object::Instantiate(bombPrefab->get_gameObject(), mesh);
            Qosmetics::Notes::MaterialUtils::ReplaceMaterialsForGameObject(bomb);

            bomb->AddComponent<Qosmetics::Notes::BombColorHandler*>();
            bomb->set_name(ConstStrings::Bomb());
            bomb->get_transform()->set_localScale(Sombrero::FastVector3::one() * noteSizeFactor * 0.4f);
            bomb->get_transform()->set_localPosition(Sombrero::FastVector3::zero());
            bomb->get_transform()->set_localRotation(Sombrero::FastQuaternion::identity());
            auto meshRenderer = mesh->get_gameObject()->GetComponent<UnityEngine::MeshRenderer*>();
            meshRenderer->set_enabled(false);

            if (globalConfig.overrideNoteSize && globalConfig.alsoChangeHitboxes)
            {
                auto sphereCollider = mesh->get_gameObject()->GetComponent<UnityEngine::SphereCollider*>();
                sphereCollider->set_radius(sphereCollider->get_radius() * noteSizeFactor);
            }
        }
        // bomb didn't exist, but we do want to change note size
        else if (globalConfig.overrideNoteSize)
        {

            mesh->set_localScale(mesh->get_localScale() * noteSizeFactor);
            if (!globalConfig.alsoChangeHitboxes)
            {
                auto sphereCollider = mesh->get_gameObject()->GetComponent<UnityEngine::SphereCollider*>();
                sphereCollider->set_radius(sphereCollider->get_radius() / noteSizeFactor);
            }
        }
    }
    catch (il2cpp_utils::RunMethodException const& e)
    {
        ERROR("{}", e.what());
    }
    return bombNotePrefab;
}

REDECORATION_REGISTRATION(mirroredBombNoteControllerPrefab, 10, true, GlobalNamespace::MirroredBombNoteController*, GlobalNamespace::FakeMirrorObjectsInstaller*)
{
    if (Qosmetics::Notes::Disabling::GetAnyDisabling())
        return mirroredBombNoteControllerPrefab;
    try
    {
        GET_CONFIG()
        auto mesh = mirroredBombNoteControllerPrefab->get_transform()->Find("Mesh");
        auto meshRenderer = mesh->get_gameObject()->GetComponent<UnityEngine::MeshRenderer*>();
        // if obj exists, it has bomb, we don't force default, the object is mirrorable, and we are not disabling reflections
        if (noteModelContainer->currentNoteObject && config.get_hasBomb() && !globalConfig.forceDefaultBombs && config.get_isMirrorable() && !globalConfig.disableReflections)
        {
            meshRenderer->set_enabled(false);
            auto bombPrefab = noteModelContainer->currentNoteObject->get_transform()->Find(ConstStrings::MirrorBomb());
            auto bomb = UnityEngine::Object::Instantiate(bombPrefab->get_gameObject(), mesh);
            Qosmetics::Notes::MaterialUtils::ReplaceMaterialsForGameObject(bomb);
            bomb->set_name(ConstStrings::Bomb());

            bomb->get_transform()->set_localScale(Sombrero::FastVector3::one() * noteSizeFactor * 0.4f);
            bomb->get_transform()->set_localPosition(Sombrero::FastVector3::zero());
            bomb->get_transform()->set_localRotation(Sombrero::FastQuaternion::identity());
        }
        else if (globalConfig.disableReflections || (!config.get_isMirrorable() && !globalConfig.keepMissingReflections))
        {
            meshRenderer->set_enabled(false);
            if (globalConfig.overrideNoteSize)
            {
                mesh->set_localScale(mesh->get_localScale() * noteSizeFactor);
            }
        }
        else if (globalConfig.overrideNoteSize)
        {
            mesh->set_localScale(mesh->get_localScale() * noteSizeFactor);
        }
    }
    catch (il2cpp_utils::RunMethodException const& e)
    {
        ERROR("{}", e.what());
    }

    return mirroredBombNoteControllerPrefab;
}

#pragma endregion