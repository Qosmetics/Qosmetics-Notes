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
#include "GlobalNamespace/MirroredCubeNoteController.hpp"
#include "GlobalNamespace/NoteDebris.hpp"
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
#include "CustomTypes/CyoobColorHandler.hpp"
#include "CustomTypes/CyoobHandler.hpp"
#include "CustomTypes/CyoobParent.hpp"
#include "CustomTypes/DebrisColorHandler.hpp"
#include "CustomTypes/DebrisHandler.hpp"
#include "CustomTypes/DebrisParent.hpp"
#include "CustomTypes/NoteModelContainer.hpp"
#include "PropertyID.hpp"

#include "config.hpp"

#pragma region bombs
REDECORATION_REGISTRATION(bombNotePrefab, 10, true, GlobalNamespace::BombNoteController*, GlobalNamespace::BeatmapObjectsInstaller*)
{
    auto noteModelContainer = Qosmetics::Notes::NoteModelContainer::get_instance();
    auto& config = noteModelContainer->GetNoteConfig();
    auto& globalConfig = Qosmetics::Notes::Config::get_config();
    auto gameplayCoreSceneSetupData = container->TryResolve<GlobalNamespace::GameplayCoreSceneSetupData*>();
    auto mesh = bombNotePrefab->get_transform()->Find("Mesh");
    float noteSizeFactor = (globalConfig.overrideNoteSize ? globalConfig.noteSize : 1.0f) * gameplayCoreSceneSetupData->dyn_gameplayModifiers()->get_notesUniformScale();

    // if we have a note object, and a bomb exists on it
    if (noteModelContainer->currentNoteObject && config.get_hasBomb())
    {
        // TODO: Implement global config value honoring
        auto bombPrefab = noteModelContainer->currentNoteObject->get_transform()->Find(ConstStrings::Bomb());
        auto bomb = UnityEngine::Object::Instantiate(bombPrefab->get_gameObject(), mesh);
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
    return bombNotePrefab;
}

REDECORATION_REGISTRATION(mirroredBombNoteControllerPrefab, 10, true, GlobalNamespace::MirroredBombNoteController*, GlobalNamespace::FakeMirrorObjectsInstaller*)
{
    auto noteModelContainer = Qosmetics::Notes::NoteModelContainer::get_instance();
    auto& config = noteModelContainer->GetNoteConfig();
    auto& globalConfig = Qosmetics::Notes::Config::get_config();
    auto gameplayCoreSceneSetupData = container->TryResolve<GlobalNamespace::GameplayCoreSceneSetupData*>();
    auto mesh = mirroredBombNoteControllerPrefab->get_transform()->Find("Mesh");
    auto meshRenderer = mesh->get_gameObject()->GetComponent<UnityEngine::MeshRenderer*>();
    float noteSizeFactor = (globalConfig.overrideNoteSize ? globalConfig.noteSize : 1.0f) * gameplayCoreSceneSetupData->dyn_gameplayModifiers()->get_notesUniformScale();
    // if obj exists, it has bomb, we don't force default, the object is mirrorable, and we are not disabling reflections
    if (noteModelContainer->currentNoteObject && config.get_hasBomb() && !globalConfig.forceDefaultBombs && config.get_isMirrorable() && !globalConfig.disableReflections)
    {
        // TODO: Implement global config value honoring
        meshRenderer->set_enabled(false);
        auto bombPrefab = noteModelContainer->currentNoteObject->get_transform()->Find(ConstStrings::MirrorBomb());
        auto bomb = UnityEngine::Object::Instantiate(bombPrefab->get_gameObject(), mesh);
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
    return mirroredBombNoteControllerPrefab;
}

#pragma endregion

#pragma region normalNotes
REDECORATION_REGISTRATION(normalBasicNotePrefab, 10, true, GlobalNamespace::GameNoteController*, GlobalNamespace::BeatmapObjectsInstaller*)
{
    auto noteModelContainer = Qosmetics::Notes::NoteModelContainer::get_instance();
    auto& config = noteModelContainer->GetNoteConfig();
    auto& globalConfig = Qosmetics::Notes::Config::get_config();
    auto gameplayCoreSceneSetupData = container->TryResolve<GlobalNamespace::GameplayCoreSceneSetupData*>();
    float noteSizeFactor = (globalConfig.overrideNoteSize ? globalConfig.noteSize : 1.0f) * gameplayCoreSceneSetupData->dyn_gameplayModifiers()->get_notesUniformScale();
    // get the notecubetransform to edit it's children
    auto noteCubeTransform = normalBasicNotePrefab->get_transform()->Find("NoteCube");
    // if a custom note even exists
    if (noteModelContainer->currentNoteObject)
    {
        auto cyoobParent = normalBasicNotePrefab->get_gameObject()->AddComponent<Qosmetics::Notes::CyoobParent*>();
        auto actualNotes = noteModelContainer->currentNoteObject->get_transform()->Find(ConstStrings::Notes());
        // instantiate the notes prefab
        auto notes = UnityEngine::Object::Instantiate(actualNotes->get_gameObject(), noteCubeTransform);
        notes->set_name("Notes");
        notes->get_transform()->set_localScale(Sombrero::FastVector3::one() * noteSizeFactor * 0.4f);

        cyoobParent->cyoobHandler = notes->GetComponent<Qosmetics::Notes::CyoobHandler*>();
        int childCount = notes->get_transform()->get_childCount();

        auto colorScheme = gameplayCoreSceneSetupData->dyn_colorScheme();
        auto leftColor = colorScheme->dyn__saberAColor();
        auto rightColor = colorScheme->dyn__saberBColor();

        for (int i = 0; i < childCount; i++)
        {
            auto child = notes->get_transform()->GetChild(i);
            child->set_localPosition(Sombrero::FastVector3::zero());
            child->set_localRotation(Sombrero::FastQuaternion::identity());

            /// add color handler and set colors
            auto colorHandler = child->get_gameObject()->GetComponent<Qosmetics::Notes::CyoobColorHandler*>();
            colorHandler->FetchCCMaterials();
            if (child->get_name().starts_with("Left"))
            {
                colorHandler->SetColors(leftColor, rightColor);
            }
            else
            {
                colorHandler->SetColors(rightColor, leftColor);
            }
        }

        if (globalConfig.overrideNoteSize && globalConfig.alsoChangeHitboxes)
        {
            for (auto bigCuttable : normalBasicNotePrefab->dyn__bigCuttableBySaberList())
                bigCuttable->set_colliderSize(bigCuttable->get_colliderSize() * noteSizeFactor);
            for (auto smallCuttable : normalBasicNotePrefab->dyn__smallCuttableBySaberList())
                smallCuttable->set_colliderSize(smallCuttable->get_colliderSize() * noteSizeFactor);
        }

        // if we don't want to show arrows, disable the arrow gameobjects
        if (!config.get_showArrows())
        {
            noteCubeTransform->Find(ConstStrings::NoteArrow())->get_gameObject()->SetActive(false);
            noteCubeTransform->Find(ConstStrings::NoteArrowGlow())->get_gameObject()->SetActive(false);
            noteCubeTransform->Find(ConstStrings::NoteCircleGlow())->get_gameObject()->SetActive(false);
        }

        /// don't show the normal mesh of the note
        noteCubeTransform->get_gameObject()->GetComponent<UnityEngine::MeshFilter*>()->set_mesh(nullptr);
    }
    // note didn't exist, but we do want to change note size
    else if (globalConfig.overrideNoteSize)
    {
        noteCubeTransform->set_localScale(noteCubeTransform->get_localScale() * noteSizeFactor);

        // if we don't want to change hitbox sizes, scale the cuttable hitboxes to make them proper size
        if (!globalConfig.alsoChangeHitboxes)
        {
            for (auto bigCuttable : normalBasicNotePrefab->dyn__bigCuttableBySaberList())
                bigCuttable->set_colliderSize(bigCuttable->get_colliderSize() / noteSizeFactor);
            for (auto smallCuttable : normalBasicNotePrefab->dyn__smallCuttableBySaberList())
                smallCuttable->set_colliderSize(smallCuttable->get_colliderSize() / noteSizeFactor);
        }
    }
    return normalBasicNotePrefab;
}

REDECORATION_REGISTRATION(mirroredGameNoteControllerPrefab, 10, true, GlobalNamespace::MirroredCubeNoteController*, GlobalNamespace::FakeMirrorObjectsInstaller*)
{
    auto noteModelContainer = Qosmetics::Notes::NoteModelContainer::get_instance();
    auto& config = noteModelContainer->GetNoteConfig();
    auto& globalConfig = Qosmetics::Notes::Config::get_config();
    auto gameplayCoreSceneSetupData = container->TryResolve<GlobalNamespace::GameplayCoreSceneSetupData*>();
    auto mirroredNoteCubeTransform = mirroredGameNoteControllerPrefab->get_transform()->Find("NoteCube");
    float noteSizeFactor = (globalConfig.overrideNoteSize ? globalConfig.noteSize : 1.0f) * gameplayCoreSceneSetupData->dyn_gameplayModifiers()->get_notesUniformScale();
    // if obj exists, and is mirrorable, and we are not disabling reflections
    if (noteModelContainer->currentNoteObject && config.get_isMirrorable() && !globalConfig.disableReflections)
    {
        auto cyoobParent = mirroredGameNoteControllerPrefab->get_gameObject()->AddComponent<Qosmetics::Notes::CyoobParent*>();
        // get the notecubetransform to edit it's children
        auto actualNotes = noteModelContainer->currentNoteObject->get_transform()->Find(ConstStrings::MirrorNotes());

        // instantiate the notes prefab
        auto mirroredNotes = UnityEngine::Object::Instantiate(actualNotes->get_gameObject(), mirroredNoteCubeTransform);
        mirroredNotes->set_name("Notes");
        mirroredNotes->get_transform()->set_localScale(Sombrero::FastVector3::one() * noteSizeFactor * 0.4f);

        cyoobParent->cyoobHandler = mirroredNotes->GetComponent<Qosmetics::Notes::CyoobHandler*>();

        auto gameplayCoreSceneSetupData = container->TryResolve<GlobalNamespace::GameplayCoreSceneSetupData*>();
        auto colorScheme = gameplayCoreSceneSetupData->dyn_colorScheme();
        auto leftColor = colorScheme->dyn__saberAColor();
        auto rightColor = colorScheme->dyn__saberBColor();

        int childCount = mirroredNotes->get_transform()->get_childCount();
        for (int i = 0; i < childCount; i++)
        {
            auto child = mirroredNotes->get_transform()->GetChild(i);
            child->set_localPosition(Sombrero::FastVector3::zero());
            child->set_localRotation(Sombrero::FastQuaternion::identity());

            /// add color handler and set colors
            auto colorHandler = child->get_gameObject()->GetComponent<Qosmetics::Notes::CyoobColorHandler*>();
            colorHandler->FetchCCMaterials();
            if (child->get_name().starts_with("Left"))
                colorHandler->SetColors(leftColor, rightColor);
            else
                colorHandler->SetColors(rightColor, leftColor);
        }

        // TODO: Implement global config value honoring
        // if we don't want to show arrows, disable the arrow gameobjects
        if (!config.get_showArrows())
        {
            mirroredNoteCubeTransform->Find(ConstStrings::NoteArrow())->get_gameObject()->SetActive(false);
            mirroredNoteCubeTransform->Find(ConstStrings::NoteCircleGlow())->get_gameObject()->SetActive(false);
        }

        // if the note is not default config, set the mesh to nullptr so it can never show
        if (!config.get_isDefault())
            mirroredNoteCubeTransform->get_gameObject()->GetComponent<UnityEngine::MeshFilter*>()->set_mesh(nullptr);
    }
    else if (globalConfig.disableReflections || (!config.get_isMirrorable() && !globalConfig.keepMissingReflections))
    {
        mirroredNoteCubeTransform->get_gameObject()->GetComponent<UnityEngine::MeshFilter*>()->set_mesh(nullptr);
        mirroredNoteCubeTransform->Find(ConstStrings::NoteArrow())->get_gameObject()->SetActive(false);
        mirroredNoteCubeTransform->Find(ConstStrings::NoteCircleGlow())->get_gameObject()->SetActive(false);
    }
    else if (globalConfig.overrideNoteSize)
    {
        mirroredNoteCubeTransform->set_localScale(mirroredNoteCubeTransform->get_localScale() * noteSizeFactor);
    }

    return mirroredGameNoteControllerPrefab;
}

#pragma endregion

#pragma region debris

GlobalNamespace::NoteDebris* RedecorateNoteDebris(GlobalNamespace::NoteDebris* noteDebrisPrefab, Zenject::DiContainer* container)
{
    auto noteModelContainer = Qosmetics::Notes::NoteModelContainer::get_instance();
    auto& config = noteModelContainer->GetNoteConfig();
    auto& globalConfig = Qosmetics::Notes::Config::get_config();
    auto gameplayCoreSceneSetupData = container->TryResolve<GlobalNamespace::GameplayCoreSceneSetupData*>();
    // TODO: Implement global config value honoring
    float noteSizeFactor = (globalConfig.overrideNoteSize ? globalConfig.noteSize : 1.0f) * gameplayCoreSceneSetupData->dyn_gameplayModifiers()->get_notesUniformScale();
    if (config.get_hasDebris() && !gameplayCoreSceneSetupData->dyn_playerSpecificSettings()->get_reduceDebris() && !globalConfig.forceDefaultDebris)
    {
        auto noteDebrisParent = noteDebrisPrefab->get_gameObject()->AddComponent<Qosmetics::Notes::DebrisParent*>();

        auto meshTransform = noteDebrisPrefab->dyn__meshTransform();
        auto actualDebris = noteModelContainer->currentNoteObject->get_transform()->Find("Debris");
        auto debris = UnityEngine::Object::Instantiate(actualDebris->get_gameObject(), meshTransform);
        debris->set_name("Debris");
        debris->get_transform()->set_localScale(Sombrero::FastVector3::one() * noteSizeFactor * 0.4f);

        // debris->AddComponent<Qosmetics::Notes::DebrisHandler*>();

        auto colorScheme = gameplayCoreSceneSetupData->dyn_colorScheme();
        auto leftColor = colorScheme->dyn__saberAColor();
        auto rightColor = colorScheme->dyn__saberBColor();

        int childCount = debris->get_transform()->get_childCount();
        for (int i = 0; i < childCount; i++)
        {
            auto child = debris->get_transform()->GetChild(i);
            child->set_localPosition(Sombrero::FastVector3::zero());
            child->set_localRotation(Sombrero::FastQuaternion::identity());

            /// add color handler and set colors
            auto colorHandler = child->get_gameObject()->GetComponent<Qosmetics::Notes::DebrisColorHandler*>();
            colorHandler->FetchCCMaterials();
            if (child->get_name().starts_with("Left"))
            {
                colorHandler->SetColors(leftColor, rightColor);
            }
            else
            {
                colorHandler->SetColors(rightColor, leftColor);
            }
        }

        meshTransform->get_gameObject()->GetComponent<UnityEngine::MeshFilter*>()->set_mesh(nullptr);
    }
    else if (globalConfig.overrideNoteSize)
    {
        auto t = noteDebrisPrefab->get_transform();
        t->set_localScale(t->get_localScale() * noteSizeFactor);
    }
    return noteDebrisPrefab;
}

REDECORATION_REGISTRATION(noteDebrisHDPrefab, 10, true, GlobalNamespace::NoteDebris*, GlobalNamespace::EffectPoolsManualInstaller*)
{
    return RedecorateNoteDebris(noteDebrisHDPrefab, container);
}

REDECORATION_REGISTRATION(noteDebrisLWPrefab, 10, true, GlobalNamespace::NoteDebris*, GlobalNamespace::EffectPoolsManualInstaller*)
{
    return RedecorateNoteDebris(noteDebrisLWPrefab, container);
}

#pragma endregion