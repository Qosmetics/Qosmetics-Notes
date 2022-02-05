#include "GlobalNamespace/BeatmapObjectsInstaller.hpp"
#include "GlobalNamespace/BombNoteController.hpp"
#include "GlobalNamespace/EffectPoolsManualInstaller.hpp"
#include "GlobalNamespace/FakeMirrorObjectsInstaller.hpp"
#include "GlobalNamespace/GameNoteController.hpp"
#include "GlobalNamespace/MirroredBombNoteController.hpp"
#include "GlobalNamespace/MirroredCubeNoteController.hpp"
#include "GlobalNamespace/NoteDebris.hpp"

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

#include "CustomTypes/CyoobColorHandler.hpp"
#include "CustomTypes/CyoobHandler.hpp"
#include "CustomTypes/CyoobParent.hpp"
#include "CustomTypes/DebrisColorHandler.hpp"
#include "CustomTypes/DebrisHandler.hpp"
#include "CustomTypes/DebrisParent.hpp"
#include "CustomTypes/NoteModelContainer.hpp"
#include "PropertyID.hpp"

#define CONST_STRING(identifier)                    \
    static StringW identifier()                     \
    {                                               \
        static ConstString identifier(#identifier); \
        return identifier;                          \
    }

namespace ConstStrings
{
    CONST_STRING(NoteArrow);
    CONST_STRING(NoteCircleGlow);
    CONST_STRING(NoteArrowGlow);
}

#pragma region bombs
REDECORATION_REGISTRATION(bombNotePrefab, 10, true, GlobalNamespace::BombNoteController*, GlobalNamespace::BeatmapObjectsInstaller*)
{
    auto noteModelContainer = Qosmetics::Notes::NoteModelContainer::get_instance();
    auto& config = noteModelContainer->GetNoteConfig();
    if (noteModelContainer->currentNoteObject && config.get_hasBomb())
    {
        auto mesh = bombNotePrefab->get_transform()->Find("Mesh");
        auto bombPrefab = noteModelContainer->currentNoteObject->get_transform()->Find("Bomb");
        auto bomb = UnityEngine::Object::Instantiate(bombPrefab->get_gameObject(), mesh);
        bomb->set_name("Bomb");
        bomb->get_transform()->set_localScale(UnityEngine::Vector3(0.4f, 0.4f, 0.4f));
        bomb->get_transform()->set_localPosition(Sombrero::FastVector3::zero());
        bomb->get_transform()->set_localRotation(Sombrero::FastQuaternion::identity());
        auto meshRenderer = mesh->get_gameObject()->GetComponent<UnityEngine::MeshRenderer*>();
        meshRenderer->set_enabled(false);

        auto renderers = bomb->GetComponentsInChildren<UnityEngine::MeshRenderer*>(true);

        for (auto renderer : renderers)
        {
            auto materials = renderer->get_materials();

            for (auto material : materials)
            {
                material->set_renderQueue(2004);
                if (material->HasProperty(PropertyID::_Alpha()))
                    material->SetFloat(PropertyID::_Alpha(), 1.0f);
                if (material->HasProperty(PropertyID::_StencilRefID()))
                    material->SetFloat(PropertyID::_StencilRefID(), 0);
                if (material->HasProperty(PropertyID::_StencilComp()))
                    material->SetFloat(PropertyID::_StencilComp(), 8);
                if (material->HasProperty(PropertyID::_StencilOp()))
                    material->SetFloat(PropertyID::_StencilOp(), 0);

                if (material->HasProperty(PropertyID::_BlendDstFactor()))
                    material->SetFloat(PropertyID::_BlendDstFactor(), 0);
                if (material->HasProperty(PropertyID::_BlendDstFactorA()))
                    material->SetFloat(PropertyID::_BlendDstFactorA(), 0);
                if (material->HasProperty(PropertyID::_BlendSrcFactor()))
                    material->SetFloat(PropertyID::_BlendSrcFactor(), 1);
                if (material->HasProperty(PropertyID::_BlendSrcFactorA()))
                    material->SetFloat(PropertyID::_BlendSrcFactorA(), 0);
            }
        }
    }
    return bombNotePrefab;
}

REDECORATION_REGISTRATION(mirroredBombNoteControllerPrefab, 10, true, GlobalNamespace::MirroredBombNoteController*, GlobalNamespace::FakeMirrorObjectsInstaller*)
{
    auto noteModelContainer = Qosmetics::Notes::NoteModelContainer::get_instance();
    auto& config = noteModelContainer->GetNoteConfig();
    if (noteModelContainer->currentNoteObject && config.get_hasBomb())
    {
        auto mesh = mirroredBombNoteControllerPrefab->get_transform()->Find("Mesh");
        auto meshRenderer = mesh->get_gameObject()->GetComponent<UnityEngine::MeshRenderer*>();
        meshRenderer->set_enabled(false);
        if (config.get_isMirrorable())
        {
            auto bombPrefab = noteModelContainer->currentNoteObject->get_transform()->Find("Bomb");
            auto bomb = UnityEngine::Object::Instantiate(bombPrefab->get_gameObject(), mesh);
            bomb->set_name("Bomb");
            bomb->get_transform()->set_localScale(UnityEngine::Vector3(0.4f, 0.4f, 0.4f));

            auto renderers = bomb->GetComponentsInChildren<UnityEngine::MeshRenderer*>(true);

            for (auto renderer : renderers)
            {
                auto materials = renderer->get_materials();
                for (auto material : materials)
                {
                    material->set_renderQueue(1951);
                    if (material->HasProperty(PropertyID::_Alpha()))
                        material->SetFloat(PropertyID::_Alpha(), 0.05f);
                    if (material->HasProperty(PropertyID::_StencilRefID()))
                        material->SetFloat(PropertyID::_StencilRefID(), 1);
                    if (material->HasProperty(PropertyID::_StencilComp()))
                        material->SetFloat(PropertyID::_StencilComp(), 3);
                    if (material->HasProperty(PropertyID::_StencilOp()))
                        material->SetFloat(PropertyID::_StencilOp(), 0);

                    if (material->HasProperty(PropertyID::_BlendDstFactor()))
                        material->SetFloat(PropertyID::_BlendDstFactor(), 10);
                    if (material->HasProperty(PropertyID::_BlendDstFactorA()))
                        material->SetFloat(PropertyID::_BlendDstFactorA(), 0);
                    if (material->HasProperty(PropertyID::_BlendSrcFactor()))
                        material->SetFloat(PropertyID::_BlendSrcFactor(), 5);
                    if (material->HasProperty(PropertyID::_BlendSrcFactorA()))
                        material->SetFloat(PropertyID::_BlendSrcFactorA(), 0);
                }
            }
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
        auto actualNotes = noteModelContainer->currentNoteObject->get_transform()->Find("Notes");
        // instantiate the notes prefab
        auto notes = UnityEngine::Object::Instantiate(actualNotes->get_gameObject(), noteCubeTransform);
        notes->set_name("Notes");
        notes->get_transform()->set_localScale({0.4f, 0.4f, 0.4f});

        cyoobParent->cyoobHandler = notes->AddComponent<Qosmetics::Notes::CyoobHandler*>();
        int childCount = notes->get_transform()->get_childCount();
        for (int i = 0; i < childCount; i++)
        {
            auto child = notes->get_transform()->GetChild(i);
            child->get_gameObject()->AddComponent<Qosmetics::Notes::CyoobColorHandler*>();
            child->set_localPosition(Sombrero::FastVector3::zero());
            child->set_localRotation(Sombrero::FastQuaternion::identity());
        }

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

        auto renderers = notes->GetComponentsInChildren<UnityEngine::MeshRenderer*>(true);

        for (auto renderer : renderers)
        {
            auto materials = renderer->get_materials();

            for (auto material : materials)
            {
                material->set_renderQueue(2004);
                if (material->HasProperty(PropertyID::_Alpha()))
                    material->SetFloat(PropertyID::_Alpha(), 1.0f);
                if (material->HasProperty(PropertyID::_StencilRefID()))
                    material->SetFloat(PropertyID::_StencilRefID(), 0);
                if (material->HasProperty(PropertyID::_StencilComp()))
                    material->SetFloat(PropertyID::_StencilComp(), 8);
                if (material->HasProperty(PropertyID::_StencilOp()))
                    material->SetFloat(PropertyID::_StencilOp(), 0);

                if (material->HasProperty(PropertyID::_BlendDstFactor()))
                    material->SetFloat(PropertyID::_BlendDstFactor(), 0);
                if (material->HasProperty(PropertyID::_BlendDstFactorA()))
                    material->SetFloat(PropertyID::_BlendDstFactorA(), 0);
                if (material->HasProperty(PropertyID::_BlendSrcFactor()))
                    material->SetFloat(PropertyID::_BlendSrcFactor(), 1);
                if (material->HasProperty(PropertyID::_BlendSrcFactorA()))
                    material->SetFloat(PropertyID::_BlendSrcFactorA(), 0);
            }
        }
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
            auto actualNotes = noteModelContainer->currentNoteObject->get_transform()->Find("Notes");

            // instantiate the notes prefab
            auto mirroredNotes = UnityEngine::Object::Instantiate(actualNotes->get_gameObject(), mirroredNoteCubeTransform);
            mirroredNotes->set_name("Notes");
            mirroredNotes->get_transform()->set_localScale({0.4f, 0.4f, 0.4f});

            cyoobParent->cyoobHandler = mirroredNotes->AddComponent<Qosmetics::Notes::CyoobHandler*>();

            int childCount = mirroredNotes->get_transform()->get_childCount();
            for (int i = 0; i < childCount; i++)
            {
                auto child = mirroredNotes->get_transform()->GetChild(i);
                child->get_gameObject()->AddComponent<Qosmetics::Notes::CyoobColorHandler*>();
                child->set_localPosition(Sombrero::FastVector3::zero());
                child->set_localRotation(Sombrero::FastQuaternion::identity());
            }

            // if we don't want to show arrows, disable the arrow gameobjects
            if (!config.get_showArrows())
            {
                mirroredNoteCubeTransform->Find(ConstStrings::NoteArrow())->get_gameObject()->SetActive(false);
                mirroredNoteCubeTransform->Find(ConstStrings::NoteCircleGlow())->get_gameObject()->SetActive(false);
            }

            // if the note is not default config, set the mesh to nullptr so it can never show
            if (!config.get_isDefault())
                mirroredNoteCubeTransform->get_gameObject()->GetComponent<UnityEngine::MeshFilter*>()->set_mesh(nullptr);

            auto renderers = mirroredNotes->GetComponentsInChildren<UnityEngine::MeshRenderer*>(true);

            for (auto renderer : renderers)
            {
                auto materials = renderer->get_materials();
                for (auto material : materials)
                {
                    material->set_renderQueue(1951);
                    if (material->HasProperty(PropertyID::_Alpha()))
                        material->SetFloat(PropertyID::_Alpha(), 0.05f);
                    if (material->HasProperty(PropertyID::_StencilRefID()))
                        material->SetFloat(PropertyID::_StencilRefID(), 1);
                    if (material->HasProperty(PropertyID::_StencilComp()))
                        material->SetFloat(PropertyID::_StencilComp(), 3);
                    if (material->HasProperty(PropertyID::_StencilOp()))
                        material->SetFloat(PropertyID::_StencilOp(), 0);

                    if (material->HasProperty(PropertyID::_BlendDstFactor()))
                        material->SetFloat(PropertyID::_BlendDstFactor(), 10);
                    if (material->HasProperty(PropertyID::_BlendDstFactorA()))
                        material->SetFloat(PropertyID::_BlendDstFactorA(), 0);
                    if (material->HasProperty(PropertyID::_BlendSrcFactor()))
                        material->SetFloat(PropertyID::_BlendSrcFactor(), 5);
                    if (material->HasProperty(PropertyID::_BlendSrcFactorA()))
                        material->SetFloat(PropertyID::_BlendSrcFactorA(), 0);
                }
            }
        }
    }
    return mirroredGameNoteControllerPrefab;
}
#pragma endregion

#pragma region debris

GlobalNamespace::NoteDebris* RedecorateNoteDebris(GlobalNamespace::NoteDebris* noteDebrisPrefab)
{
    auto noteModelContainer = Qosmetics::Notes::NoteModelContainer::get_instance();
    auto& config = noteModelContainer->GetNoteConfig();
    if (config.get_hasDebris())
    {
        auto noteDebrisParent = noteDebrisPrefab->get_gameObject()->AddComponent<Qosmetics::Notes::DebrisParent*>();

        auto meshTransform = noteDebrisPrefab->dyn__meshTransform();
        auto actualDebris = noteModelContainer->currentNoteObject->get_transform()->Find("Debris");
        auto debris = UnityEngine::Object::Instantiate(actualDebris->get_gameObject(), meshTransform);
        debris->set_name("Debris");
        debris->get_transform()->set_localScale({0.4f, 0.4f, 0.4f});

        debris->AddComponent<Qosmetics::Notes::DebrisHandler*>();

        int childCount = debris->get_transform()->get_childCount();
        for (int i = 0; i < childCount; i++)
        {
            auto child = debris->get_transform()->GetChild(i);
            child->get_gameObject()->AddComponent<Qosmetics::Notes::DebrisColorHandler*>();
            child->set_localPosition(Sombrero::FastVector3::zero());
            child->set_localRotation(Sombrero::FastQuaternion::identity());
        }

        meshTransform->get_gameObject()->GetComponent<UnityEngine::MeshFilter*>()->set_mesh(nullptr);
    }
    return noteDebrisPrefab;
}

REDECORATION_REGISTRATION(noteDebrisHDPrefab, 10, true, GlobalNamespace::NoteDebris*, GlobalNamespace::EffectPoolsManualInstaller*)
{
    return RedecorateNoteDebris(noteDebrisHDPrefab);
}

REDECORATION_REGISTRATION(noteDebrisLWPrefab, 10, true, GlobalNamespace::NoteDebris*, GlobalNamespace::EffectPoolsManualInstaller*)
{
    return RedecorateNoteDebris(noteDebrisLWPrefab);
}

#pragma endregion