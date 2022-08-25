#include "Disabling.hpp"
#include "GlobalNamespace/BeatmapObjectsInstaller.hpp"
#include "GlobalNamespace/BoxCuttableBySaber.hpp"
#include "GlobalNamespace/BurstSliderGameNoteController.hpp"
#include "GlobalNamespace/ColorScheme.hpp"
#include "GlobalNamespace/FakeMirrorObjectsInstaller.hpp"
#include "GlobalNamespace/GameNoteController.hpp"
#include "GlobalNamespace/GameplayCoreSceneSetupData.hpp"
#include "GlobalNamespace/GameplayModifiers.hpp"
#include "GlobalNamespace/MirroredGameNoteController.hpp"
#include "GlobalNamespace/NoteDebris.hpp"
#include "GlobalNamespace/NoteDebrisPoolInstaller.hpp"
#include "GlobalNamespace/PlayerSpecificSettings.hpp"
#include "Zenject/DiContainer.hpp"

#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/Material.hpp"
#include "UnityEngine/MeshFilter.hpp"
#include "UnityEngine/MeshRenderer.hpp"
#include "UnityEngine/Shader.hpp"
#include "UnityEngine/SphereCollider.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/Vector3.hpp"
#include "sombrero/shared/FastQuaternion.hpp"
#include "sombrero/shared/FastVector3.hpp"

#include "ConstStrings.hpp"
#include "CustomTypes/BasicNoteScaler.hpp"
#include "CustomTypes/ChainHandler.hpp"
#include "CustomTypes/ChainParent.hpp"
#include "CustomTypes/CyoobColorHandler.hpp"
#include "CustomTypes/CyoobHandler.hpp"
#include "CustomTypes/CyoobParent.hpp"
#include "CustomTypes/NoteModelContainer.hpp"
#include "MaterialUtils.hpp"
#include "PropertyID.hpp"
#include "config.hpp"
#include "logging.hpp"

#include "qosmetics-core/shared/RedecorationRegister.hpp"

extern ModInfo modInfo;

#if __has_include("chroma/shared/NoteAPI.hpp")
#include "chroma/shared/BombAPI.hpp"
#include "chroma/shared/NoteAPI.hpp"
#ifndef CHROMA_EXISTS
#define CHROMA_EXISTS
#endif
#endif

#define GET_CONFIG()                                                                                                                          \
    auto noteModelContainer = Qosmetics::Notes::NoteModelContainer::get_instance();                                                           \
    auto& config = noteModelContainer->GetNoteConfig();                                                                                       \
    auto& globalConfig = Qosmetics::Notes::Config::get_config();                                                                              \
    auto gameplayCoreSceneSetupData = container->TryResolve<GlobalNamespace::GameplayCoreSceneSetupData*>();                                  \
    auto gameplayModifiers = gameplayCoreSceneSetupData->gameplayModifiers;                                                                   \
    float noteSizeFactor = (globalConfig.get_overrideNoteSize() ? globalConfig.noteSize : 1.0f) * gameplayModifiers->get_notesUniformScale(); \
    bool ghostNotes = gameplayModifiers->get_ghostNotes();                                                                                    \
    bool disappearingArrows = gameplayModifiers->get_disappearingArrows();

static void SetAndFixObjectChildren(UnityEngine::Transform* obj, Sombrero::FastColor leftColor, Sombrero::FastColor rightColor)
{
    int childCount = obj->get_childCount();
    for (int i = 0; i < childCount; i++)
    {
        auto child = obj->get_transform()->GetChild(i);
        child->set_localScale(Sombrero::FastVector3::one());
        child->set_localPosition(Sombrero::FastVector3::zero());
        child->set_localRotation(Sombrero::FastQuaternion::identity());

        /// add color handler and set colors
        auto colorHandler = child->get_gameObject()->GetComponent<Qosmetics::Notes::CyoobColorHandler*>();
        if (colorHandler)
        {
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
    }
}

template <typename Parent, typename Handler>
GlobalNamespace::MirroredGameNoteController* RedecorateMirroredGameNoteController(GlobalNamespace::MirroredGameNoteController* prefab, Zenject::DiContainer* container, StringW mirrorObjectName, const bool& forceDefaultNotes)
{
    if (Qosmetics::Notes::Disabling::GetAnyDisabling())
        return prefab;
    try
    {
        GET_CONFIG();
        auto mirroredNoteCubeTransform = prefab->get_transform()->Find("NoteCube");
        bool replacePrefab = noteModelContainer->currentNoteObject && config.get_isMirrorable() && !globalConfig.disableReflections && !ghostNotes && !disappearingArrows && !forceDefaultNotes;
        // if obj exists, and is mirrorable, and we are not disabling reflections
        if (replacePrefab)
        {
            auto objectParent = prefab->get_gameObject()->AddComponent<Parent>();
            // get the notecubetransform to edit it's children
            auto actualNotes = noteModelContainer->currentNoteObject->get_transform()->Find(mirrorObjectName);

            // instantiate the notes prefab
            auto mirroredObject = UnityEngine::Object::Instantiate(actualNotes->get_gameObject(), mirroredNoteCubeTransform);
            Qosmetics::Notes::MaterialUtils::ReplaceMaterialsForGameObject(mirroredObject);
            mirroredObject->set_name(mirrorObjectName);
            mirroredObject->get_transform()->set_localPosition(Sombrero::FastVector3::zero());
            mirroredObject->get_transform()->set_localScale(Sombrero::FastVector3::one() * 0.4f);

            objectParent->handler = mirroredObject->GetComponent<Handler>();

            auto gameplayCoreSceneSetupData = container->TryResolve<GlobalNamespace::GameplayCoreSceneSetupData*>();
            auto colorScheme = gameplayCoreSceneSetupData->colorScheme;
            auto leftColor = colorScheme->saberAColor;
            auto rightColor = colorScheme->saberBColor;

            SetAndFixObjectChildren(mirroredObject->get_transform(), leftColor, rightColor);

            // Update note sizes
            if (globalConfig.get_overrideNoteSize())
                mirroredNoteCubeTransform->get_gameObject()->AddComponent<Qosmetics::Notes::BasicNoteScaler*>();

            // if we don't want to show arrows, disable the arrow gameobjects
            if (!config.get_showArrows())
            {
                auto noteArrow = mirroredNoteCubeTransform->Find(ConstStrings::NoteArrow());
                if (noteArrow)
                    noteArrow->get_gameObject()->SetActive(false);
                auto noteArrowGlow = mirroredNoteCubeTransform->Find(ConstStrings::NoteArrowGlow());
                if (noteArrowGlow)
                    noteArrowGlow->get_gameObject()->SetActive(false);
                auto noteCircleGlow = mirroredNoteCubeTransform->Find(ConstStrings::NoteCircleGlow());
                if (noteCircleGlow)
                    noteCircleGlow->get_gameObject()->SetActive(false);
                auto circle = mirroredNoteCubeTransform->Find(ConstStrings::Circle());
                if (circle)
                    circle->get_gameObject()->SetActive(false);
            }

            // if the note is not default config, set the mesh to nullptr so it can never show
            if (!config.get_isDefault())
                mirroredNoteCubeTransform->get_gameObject()->GetComponent<UnityEngine::MeshFilter*>()->set_mesh(nullptr);
        }
        // we want to remove existing reflections when:
        // disable reflectiosn is on OR
        // we have an object, and our object is not mirrorable && we don't want to keep the missing reflections
        else if (globalConfig.disableReflections || (noteModelContainer->currentNoteObject && !config.get_isMirrorable() && !globalConfig.keepMissingReflections))
        {
            mirroredNoteCubeTransform->get_gameObject()->GetComponent<UnityEngine::MeshFilter*>()->set_mesh(nullptr);
            auto noteArrow = mirroredNoteCubeTransform->Find(ConstStrings::NoteArrow());
            if (noteArrow)
                noteArrow->get_gameObject()->SetActive(false);
            auto noteArrowGlow = mirroredNoteCubeTransform->Find(ConstStrings::NoteArrowGlow());
            if (noteArrowGlow)
                noteArrowGlow->get_gameObject()->SetActive(false);
            auto noteCircleGlow = mirroredNoteCubeTransform->Find(ConstStrings::NoteCircleGlow());
            if (noteCircleGlow)
                noteCircleGlow->get_gameObject()->SetActive(false);
            auto circle = mirroredNoteCubeTransform->Find(ConstStrings::Circle());
            if (circle)
                circle->get_gameObject()->SetActive(false);

            if (globalConfig.get_overrideNoteSize())
                mirroredNoteCubeTransform->get_gameObject()->AddComponent<Qosmetics::Notes::BasicNoteScaler*>();
        }
        else if (globalConfig.get_overrideNoteSize())
        {
            mirroredNoteCubeTransform->get_gameObject()->AddComponent<Qosmetics::Notes::BasicNoteScaler*>();
        }
    }
    catch (il2cpp_utils::RunMethodException const& e)
    {
        ERROR("{}", e.what());
    }
    return prefab;
}

#pragma region normalNotes
template <typename Parent, typename Handler>
GlobalNamespace::NoteController* RedecorateGameNoteController(GlobalNamespace::NoteController* notePrefab, Zenject::DiContainer* container, StringW objectName, const bool& forceDefaultNotes)
{
    if (Qosmetics::Notes::Disabling::GetAnyDisabling())
        return notePrefab;
    try
    {
        GET_CONFIG()
        // get the notecubetransform to edit it's children
        auto noteCubeTransform = notePrefab->get_transform()->Find(ConstStrings::NoteCube());
        // if a custom note even exists
        bool addCustomPrefab = noteModelContainer->currentNoteObject && !ghostNotes && !disappearingArrows && !forceDefaultNotes;

        if (addCustomPrefab)
        {
            auto colorScheme = gameplayCoreSceneSetupData->colorScheme;
            auto leftColor = colorScheme->saberAColor;
            auto rightColor = colorScheme->saberBColor;

            auto objectParent = notePrefab->get_gameObject()->AddComponent<Parent*>();
            auto actualNotes = noteModelContainer->currentNoteObject->get_transform()->Find(objectName);
            // instantiate the notes prefab
            auto notes = UnityEngine::Object::Instantiate(actualNotes->get_gameObject(), noteCubeTransform);
            Qosmetics::Notes::MaterialUtils::ReplaceMaterialsForGameObject(notes);
            notes->set_name(objectName);
            notes->get_transform()->set_localPosition(Sombrero::FastVector3::zero());
            notes->get_transform()->set_localScale(Sombrero::FastVector3::one() * 0.4f);

            objectParent->handler = notes->GetComponent<Handler*>();
            int childCount = notes->get_transform()->get_childCount();

            SetAndFixObjectChildren(notes->get_transform(), leftColor, rightColor);

            // Update note sizes
            if (globalConfig.get_overrideNoteSize())
            {
                noteCubeTransform->get_gameObject()->AddComponent<Qosmetics::Notes::BasicNoteScaler*>();

                if (!globalConfig.get_alsoChangeHitboxes())
                {
                    ArrayW bigCuttables = CRASH_UNLESS(il2cpp_utils::GetFieldValue<ArrayW<GlobalNamespace::BoxCuttableBySaber*>>(notePrefab, "_bigCuttableBySaberList"));
                    for (auto bigCuttable : bigCuttables)
                        bigCuttable->set_colliderSize(bigCuttable->get_colliderSize() / noteSizeFactor);
                    ArrayW smallCuttables = CRASH_UNLESS(il2cpp_utils::GetFieldValue<ArrayW<GlobalNamespace::BoxCuttableBySaber*>>(notePrefab, "_smallCuttableBySaberList"));
                    for (auto smallCuttable : smallCuttables)
                        smallCuttable->set_colliderSize(smallCuttable->get_colliderSize() / noteSizeFactor);
                }
            }

            // if we don't want to show arrows, disable the arrow gameobjects
            if (!config.get_showArrows())
            {
                auto noteArrow = noteCubeTransform->Find(ConstStrings::NoteArrow());
                if (noteArrow)
                    noteArrow->get_gameObject()->SetActive(false);
                auto noteArrowGlow = noteCubeTransform->Find(ConstStrings::NoteArrowGlow());
                if (noteArrowGlow)
                    noteArrowGlow->get_gameObject()->SetActive(false);
                auto noteCircleGlow = noteCubeTransform->Find(ConstStrings::NoteCircleGlow());
                if (noteCircleGlow)
                    noteCircleGlow->get_gameObject()->SetActive(false);
                auto circle = noteCubeTransform->Find(ConstStrings::Circle());
                if (circle)
                    circle->get_gameObject()->SetActive(false);
            }

            /// don't show the normal mesh of the note
            noteCubeTransform->get_gameObject()->GetComponent<UnityEngine::MeshFilter*>()->set_mesh(nullptr);
        }
        // note didn't exist, but we do want to change note size
        else if (globalConfig.get_overrideNoteSize())
        {
            noteCubeTransform->get_gameObject()->AddComponent<Qosmetics::Notes::BasicNoteScaler*>();

            // if we don't want to change hitbox sizes, scale the cuttable hitboxes to make them proper size
            if (!globalConfig.get_alsoChangeHitboxes())
            {
                ArrayW bigCuttables = CRASH_UNLESS(il2cpp_utils::GetFieldValue<ArrayW<GlobalNamespace::BoxCuttableBySaber*>>(notePrefab, "_bigCuttableBySaberList"));
                for (auto bigCuttable : bigCuttables)
                    bigCuttable->set_colliderSize(bigCuttable->get_colliderSize() / noteSizeFactor);
                ArrayW smallCuttables = CRASH_UNLESS(il2cpp_utils::GetFieldValue<ArrayW<GlobalNamespace::BoxCuttableBySaber*>>(notePrefab, "_smallCuttableBySaberList"));
                for (auto smallCuttable : smallCuttables)
                    smallCuttable->set_colliderSize(smallCuttable->get_colliderSize() / noteSizeFactor);
            }
        }
    }
    catch (il2cpp_utils::RunMethodException const& e)
    {
        ERROR("{}", e.what());
    }

    return notePrefab;
}

GlobalNamespace::GameNoteController* RedecorateGameNoteController(GlobalNamespace::GameNoteController* prefab, Zenject::DiContainer* container)
{
    GET_CONFIG();
#ifdef CHROMA_EXISTS
    auto colorScheme = gameplayCoreSceneSetupData->colorScheme;
    auto leftColor = colorScheme->saberAColor;
    auto rightColor = colorScheme->saberBColor;

    auto noteCallbackOpt = Chroma::NoteAPI::getNoteChangedColorCallbackSafe();
    if (noteCallbackOpt.has_value())
    {
        INFO("Adding to note color callback!");
        auto& noteCallback = noteCallbackOpt.value().get();
        noteCallback -= &Qosmetics::Notes::CyoobParent::ColorizeSpecific;
        noteCallback += &Qosmetics::Notes::CyoobParent::ColorizeSpecific;
    }

    Qosmetics::Notes::CyoobParent::lastLeftColor = leftColor;
    Qosmetics::Notes::CyoobParent::lastRightColor = rightColor;
    Qosmetics::Notes::CyoobParent::globalRightColor = Chroma::NoteAPI::getGlobalNoteColorSafe(1).value_or(rightColor);
    Qosmetics::Notes::CyoobParent::globalLeftColor = Chroma::NoteAPI::getGlobalNoteColorSafe(0).value_or(leftColor);

    bool chromaColors = noteModelContainer->currentNoteObject && !ghostNotes && !disappearingArrows && !Qosmetics::Notes::Disabling::GetAnyDisabling();
    Chroma::NoteAPI::setNoteColorable(chromaColors);
#endif

    return reinterpret_cast<GlobalNamespace::GameNoteController*>(RedecorateGameNoteController<Qosmetics::Notes::CyoobParent, Qosmetics::Notes::CyoobHandler>(prefab, container, ConstStrings::Notes(), false));
}

REDECORATION_REGISTRATION(normalBasicNotePrefab, 10, true, GlobalNamespace::GameNoteController*, GlobalNamespace::BeatmapObjectsInstaller*)
{
    return RedecorateGameNoteController(normalBasicNotePrefab, container);
}

REDECORATION_REGISTRATION(proModeNotePrefab, 10, true, GlobalNamespace::GameNoteController*, GlobalNamespace::BeatmapObjectsInstaller*)
{
    return RedecorateGameNoteController(proModeNotePrefab, container);
}

REDECORATION_REGISTRATION(mirroredGameNoteControllerPrefab, 10, true, GlobalNamespace::MirroredGameNoteController*, GlobalNamespace::FakeMirrorObjectsInstaller*)
{
    return RedecorateMirroredGameNoteController<Qosmetics::Notes::CyoobParent*, Qosmetics::Notes::CyoobHandler*>(mirroredGameNoteControllerPrefab, container, ConstStrings::MirrorNotes(), false);
}

#pragma endregion

#pragma region chainNotes

REDECORATION_REGISTRATION(burstSliderHeadNotePrefab, 10, true, GlobalNamespace::GameNoteController*, GlobalNamespace::BeatmapObjectsInstaller*)
{

#ifdef CHROMA_EXISTS
    auto gameplayCoreSceneSetupData = container->TryResolve<GlobalNamespace::GameplayCoreSceneSetupData*>();
    auto colorScheme = gameplayCoreSceneSetupData->colorScheme;
    auto leftColor = colorScheme->saberAColor;
    auto rightColor = colorScheme->saberBColor;
    auto noteCallbackOpt = Chroma::NoteAPI::getNoteChangedColorCallbackSafe();
    if (noteCallbackOpt.has_value())
    {
        INFO("Adding to note color callback!");
        auto& noteCallback = noteCallbackOpt.value().get();
        noteCallback -= &Qosmetics::Notes::ChainParent::ColorizeSpecific;
        noteCallback += &Qosmetics::Notes::ChainParent::ColorizeSpecific;
    }

    Qosmetics::Notes::ChainParent::lastLeftColor = leftColor;
    Qosmetics::Notes::ChainParent::lastRightColor = rightColor;
    Qosmetics::Notes::ChainParent::globalRightColor = Chroma::NoteAPI::getGlobalNoteColorSafe(1).value_or(rightColor);
    Qosmetics::Notes::ChainParent::globalLeftColor = Chroma::NoteAPI::getGlobalNoteColorSafe(0).value_or(leftColor);
#endif

    return reinterpret_cast<GlobalNamespace::GameNoteController*>(RedecorateGameNoteController<Qosmetics::Notes::ChainParent, Qosmetics::Notes::ChainHandler>(burstSliderHeadNotePrefab, container, ConstStrings::Chains(), Qosmetics::Notes::Config::get_config().forceDefaultChains));
}

REDECORATION_REGISTRATION(burstSliderNotePrefab, 10, true, GlobalNamespace::BurstSliderGameNoteController*, GlobalNamespace::BeatmapObjectsInstaller*)
{
    return reinterpret_cast<GlobalNamespace::BurstSliderGameNoteController*>(RedecorateGameNoteController<Qosmetics::Notes::ChainParent, Qosmetics::Notes::ChainHandler>(burstSliderNotePrefab, container, ConstStrings::Chains(), Qosmetics::Notes::Config::get_config().forceDefaultChains));
}

REDECORATION_REGISTRATION(mirroredBurstSliderHeadGameNoteControllerPrefab, 10, true, GlobalNamespace::MirroredGameNoteController*, GlobalNamespace::FakeMirrorObjectsInstaller*)
{
    return RedecorateMirroredGameNoteController<Qosmetics::Notes::ChainParent*, Qosmetics::Notes::ChainHandler*>(mirroredBurstSliderHeadGameNoteControllerPrefab, container, ConstStrings::MirrorChains(), Qosmetics::Notes::Config::get_config().forceDefaultChains);
}

REDECORATION_REGISTRATION(mirroredBurstSliderGameNoteControllerPrefab, 10, true, GlobalNamespace::MirroredGameNoteController*, GlobalNamespace::FakeMirrorObjectsInstaller*)
{
    return RedecorateMirroredGameNoteController<Qosmetics::Notes::ChainParent*, Qosmetics::Notes::ChainHandler*>(mirroredBurstSliderGameNoteControllerPrefab, container, ConstStrings::MirrorChains(), Qosmetics::Notes::Config::get_config().forceDefaultChains);
}

#pragma endregion