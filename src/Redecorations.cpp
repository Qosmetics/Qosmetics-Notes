#include "GlobalNamespace/BeatmapObjectsInstaller.hpp"
#include "GlobalNamespace/BombNoteController.hpp"
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
    if (noteModelContainer->currentNoteObject && config.get_hasBomb())
    {
        // TODO: Implement global config value honoring
        auto mesh = bombNotePrefab->get_transform()->Find("Mesh");
        auto bombPrefab = noteModelContainer->currentNoteObject->get_transform()->Find(ConstStrings::Bomb());
        auto bomb = UnityEngine::Object::Instantiate(bombPrefab->get_gameObject(), mesh);
        bomb->set_name(ConstStrings::Bomb());
        bomb->get_transform()->set_localScale(UnityEngine::Vector3(0.4f, 0.4f, 0.4f));
        bomb->get_transform()->set_localPosition(Sombrero::FastVector3::zero());
        bomb->get_transform()->set_localRotation(Sombrero::FastQuaternion::identity());
        auto meshRenderer = mesh->get_gameObject()->GetComponent<UnityEngine::MeshRenderer*>();
        meshRenderer->set_enabled(false);
    }
    return bombNotePrefab;
}

REDECORATION_REGISTRATION(mirroredBombNoteControllerPrefab, 10, true, GlobalNamespace::MirroredBombNoteController*, GlobalNamespace::FakeMirrorObjectsInstaller*)
{
    auto noteModelContainer = Qosmetics::Notes::NoteModelContainer::get_instance();
    auto& config = noteModelContainer->GetNoteConfig();
    auto& globalConfig = Qosmetics::Notes::Config::get_config();
    auto gameplayCoreSceneSetupData = container->TryResolve<GlobalNamespace::GameplayCoreSceneSetupData*>();
    if (noteModelContainer->currentNoteObject && config.get_hasBomb() && !globalConfig.forceDefaultBombs)
    {
        auto mesh = mirroredBombNoteControllerPrefab->get_transform()->Find("Mesh");
        auto meshRenderer = mesh->get_gameObject()->GetComponent<UnityEngine::MeshRenderer*>();
        meshRenderer->set_enabled(false);
        // TODO: Implement global config value honoring
        if (config.get_isMirrorable())
        {
            auto bombPrefab = noteModelContainer->currentNoteObject->get_transform()->Find(ConstStrings::MirrorBomb());
            auto bomb = UnityEngine::Object::Instantiate(bombPrefab->get_gameObject(), mesh);
            bomb->set_name(ConstStrings::Bomb());

            float actualBombSize = 0.4f * (globalConfig.overrideNoteSize ? globalConfig.noteSize : 1.0f) * (gameplayCoreSceneSetupData->dyn_gameplayModifiers()->get_proMode() ? 0.5f : 1.0f);
            bomb->get_transform()->set_localScale(Sombrero::FastVector3::one() * actualBombSize);

            bomb->get_transform()->set_localScale(UnityEngine::Vector3(0.4f, 0.4f, 0.4f));
        }
        else if (globalConfig.disableReflections)
        {
            auto mesh = mirroredBombNoteControllerPrefab->get_transform()->Find("Mesh");
            auto meshRenderer = mesh->get_gameObject()->GetComponent<UnityEngine::MeshRenderer*>();
            meshRenderer->set_enabled(false);
        }
    }
    return mirroredBombNoteControllerPrefab;
}

#pragma endregion

#pragma region normalNotes
REDECORATION_REGISTRATION(normalBasicNotePrefab, 10, true, GlobalNamespace::GameNoteController*, GlobalNamespace::BeatmapObjectsInstaller*)
{
    auto noteModelContainer = Qosmetics::Notes::NoteModelContainer::get_instance();
    auto& config = noteModelContainer->GetNoteConfig();
    if (noteModelContainer->currentNoteObject)
    {
        auto cyoobParent = normalBasicNotePrefab->get_gameObject()->AddComponent<Qosmetics::Notes::CyoobParent*>();
        // get the notecubetransform to edit it's children
        auto noteCubeTransform = normalBasicNotePrefab->get_transform()->Find("NoteCube");
        auto actualNotes = noteModelContainer->currentNoteObject->get_transform()->Find(ConstStrings::Notes());
        // instantiate the notes prefab
        auto notes = UnityEngine::Object::Instantiate(actualNotes->get_gameObject(), noteCubeTransform);
        notes->set_name("Notes");
        notes->get_transform()->set_localScale({0.4f, 0.4f, 0.4f});

        cyoobParent->cyoobHandler = notes->GetComponent<Qosmetics::Notes::CyoobHandler*>();
        int childCount = notes->get_transform()->get_childCount();

        auto gameplayCoreSceneSetupData = container->TryResolve<GlobalNamespace::GameplayCoreSceneSetupData*>();
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

        // TODO: Implement global config value honoring
        // if we don't want to show arrows, disable the arrow gameobjects
        if (!config.get_showArrows())
        {
            noteCubeTransform->Find(ConstStrings::NoteArrow())->get_gameObject()->SetActive(false);
            noteCubeTransform->Find(ConstStrings::NoteArrowGlow())->get_gameObject()->SetActive(false);
            noteCubeTransform->Find(ConstStrings::NoteCircleGlow())->get_gameObject()->SetActive(false);
        }

        // if the note is not default config, set the mesh to nullptr so it can never show
        if (!config.get_isDefault())
            noteCubeTransform->get_gameObject()->GetComponent<UnityEngine::MeshFilter*>()->set_mesh(nullptr);
    }
    return normalBasicNotePrefab;
}

REDECORATION_REGISTRATION(mirroredGameNoteControllerPrefab, 10, true, GlobalNamespace::MirroredCubeNoteController*, GlobalNamespace::FakeMirrorObjectsInstaller*)
{
    auto noteModelContainer = Qosmetics::Notes::NoteModelContainer::get_instance();
    auto& config = noteModelContainer->GetNoteConfig();
    if (noteModelContainer->currentNoteObject)
    {
        auto mirroredNoteCubeTransform = mirroredGameNoteControllerPrefab->get_transform()->Find("NoteCube");
        if (config.get_isMirrorable())
        {
            auto cyoobParent = mirroredGameNoteControllerPrefab->get_gameObject()->AddComponent<Qosmetics::Notes::CyoobParent*>();
            // get the notecubetransform to edit it's children
            auto actualNotes = noteModelContainer->currentNoteObject->get_transform()->Find(ConstStrings::MirrorNotes());

            // instantiate the notes prefab
            auto mirroredNotes = UnityEngine::Object::Instantiate(actualNotes->get_gameObject(), mirroredNoteCubeTransform);
            mirroredNotes->set_name("Notes");
            mirroredNotes->get_transform()->set_localScale({0.4f, 0.4f, 0.4f});

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
                {
                    colorHandler->SetColors(leftColor, rightColor);
                }
                else
                {
                    colorHandler->SetColors(rightColor, leftColor);
                }
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
    if (config.get_hasDebris() && !gameplayCoreSceneSetupData->dyn_playerSpecificSettings()->get_reduceDebris() && !globalConfig.forceDefaultDebris)
    {
        auto noteDebrisParent = noteDebrisPrefab->get_gameObject()->AddComponent<Qosmetics::Notes::DebrisParent*>();

        auto meshTransform = noteDebrisPrefab->dyn__meshTransform();
        auto actualDebris = noteModelContainer->currentNoteObject->get_transform()->Find("Debris");
        auto debris = UnityEngine::Object::Instantiate(actualDebris->get_gameObject(), meshTransform);
        debris->set_name("Debris");
        float actualNoteSize = 0.4f * (globalConfig.overrideNoteSize ? globalConfig.noteSize : 1.0f) * (gameplayCoreSceneSetupData->dyn_gameplayModifiers()->get_proMode() ? 0.5f : 1.0f);
        debris->get_transform()->set_localScale(Sombrero::FastVector3::one() * actualNoteSize);

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