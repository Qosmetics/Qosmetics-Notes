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

#define GET_CONFIG()                                                                                                                    \
    auto noteModelContainer = Qosmetics::Notes::NoteModelContainer::get_instance();                                                     \
    auto& config = noteModelContainer->GetNoteConfig();                                                                                 \
    auto& globalConfig = Qosmetics::Notes::Config::get_config();                                                                        \
    auto gameplayCoreSceneSetupData = container->TryResolve<GlobalNamespace::GameplayCoreSceneSetupData*>();                            \
    auto gameplayModifiers = gameplayCoreSceneSetupData->dyn_gameplayModifiers();                                                       \
    float noteSizeFactor = (globalConfig.overrideNoteSize ? globalConfig.noteSize : 1.0f) * gameplayModifiers->get_notesUniformScale(); \
    bool ghostNotes = gameplayModifiers->get_ghostNotes();                                                                              \
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

#pragma region normalNotes
GlobalNamespace::GameNoteController* RedecorateGameNoteController(GlobalNamespace::GameNoteController* notePrefab, Zenject::DiContainer* container)
{
    if (Qosmetics::Notes::Disabling::GetAnyDisabling())
        return notePrefab;
    try
    {
        GET_CONFIG()
        // get the notecubetransform to edit it's children
        auto noteCubeTransform = notePrefab->get_transform()->Find("NoteCube");
        // if a custom note even exists
        bool addCustomPrefab = noteModelContainer->currentNoteObject && !ghostNotes && !disappearingArrows;
#ifdef CHROMA_EXISTS
        // set the note colorable by chroma to the opposite of if we are replacing the entire contents of the prefab
        Chroma::NoteAPI::setNoteColorable(addCustomPrefab);
#endif
        if (addCustomPrefab)
        {
            auto colorScheme = gameplayCoreSceneSetupData->dyn_colorScheme();
            auto leftColor = colorScheme->dyn__saberAColor();
            auto rightColor = colorScheme->dyn__saberBColor();

#ifdef CHROMA_EXISTS
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
#endif
            auto cyoobParent = notePrefab->get_gameObject()->AddComponent<Qosmetics::Notes::CyoobParent*>();
            auto actualNotes = noteModelContainer->currentNoteObject->get_transform()->Find(ConstStrings::Notes());
            // instantiate the notes prefab
            auto notes = UnityEngine::Object::Instantiate(actualNotes->get_gameObject(), noteCubeTransform);
            Qosmetics::Notes::MaterialUtils::ReplaceMaterialsForGameObject(notes);
            notes->set_name("Notes");
            notes->get_transform()->set_localScale(Sombrero::FastVector3::one() * 0.4f);

            cyoobParent->cyoobHandler = notes->GetComponent<Qosmetics::Notes::CyoobHandler*>();
            int childCount = notes->get_transform()->get_childCount();

            SetAndFixObjectChildren(notes->get_transform(), leftColor, rightColor);

            // Update note sizes
            if (globalConfig.overrideNoteSize)
            {
                noteCubeTransform->get_gameObject()->AddComponent<Qosmetics::Notes::BasicNoteScaler*>();

                if (!globalConfig.alsoChangeHitboxes)
                {
                    for (auto bigCuttable : notePrefab->dyn__bigCuttableBySaberList())
                        bigCuttable->set_colliderSize(bigCuttable->get_colliderSize() / noteSizeFactor);
                    for (auto smallCuttable : notePrefab->dyn__smallCuttableBySaberList())
                        smallCuttable->set_colliderSize(smallCuttable->get_colliderSize() / noteSizeFactor);
                }
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
            auto localScale = noteCubeTransform->get_localScale() * noteSizeFactor;
            DEBUG("Setting default note local scale to {:.2f}, {:.2f}, {:.2f}", localScale.x, localScale.y, localScale.z);
            noteCubeTransform->get_gameObject()->AddComponent<Qosmetics::Notes::BasicNoteScaler*>();

            // if we don't want to change hitbox sizes, scale the cuttable hitboxes to make them proper size
            if (!globalConfig.alsoChangeHitboxes)
            {
                for (auto bigCuttable : notePrefab->dyn__bigCuttableBySaberList())
                    bigCuttable->set_colliderSize(bigCuttable->get_colliderSize() / noteSizeFactor);
                for (auto smallCuttable : notePrefab->dyn__smallCuttableBySaberList())
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
    if (Qosmetics::Notes::Disabling::GetAnyDisabling())
        return mirroredGameNoteControllerPrefab;
    try
    {
        GET_CONFIG();
        auto mirroredNoteCubeTransform = mirroredGameNoteControllerPrefab->get_transform()->Find("NoteCube");
        bool replacePrefab = noteModelContainer->currentNoteObject && config.get_isMirrorable() && !globalConfig.disableReflections && !ghostNotes && !disappearingArrows;
        // if obj exists, and is mirrorable, and we are not disabling reflections
        if (replacePrefab)
        {
            auto cyoobParent = mirroredGameNoteControllerPrefab->get_gameObject()->AddComponent<Qosmetics::Notes::CyoobParent*>();
            // get the notecubetransform to edit it's children
            auto actualNotes = noteModelContainer->currentNoteObject->get_transform()->Find(ConstStrings::MirrorNotes());

            // instantiate the notes prefab
            auto mirroredNotes = UnityEngine::Object::Instantiate(actualNotes->get_gameObject(), mirroredNoteCubeTransform);
            Qosmetics::Notes::MaterialUtils::ReplaceMaterialsForGameObject(mirroredNotes);
            mirroredNotes->set_name("Notes");
            mirroredNotes->get_transform()->set_localScale(Sombrero::FastVector3::one() * 0.4f);

            cyoobParent->cyoobHandler = mirroredNotes->GetComponent<Qosmetics::Notes::CyoobHandler*>();

            auto gameplayCoreSceneSetupData = container->TryResolve<GlobalNamespace::GameplayCoreSceneSetupData*>();
            auto colorScheme = gameplayCoreSceneSetupData->dyn_colorScheme();
            auto leftColor = colorScheme->dyn__saberAColor();
            auto rightColor = colorScheme->dyn__saberBColor();

            SetAndFixObjectChildren(mirroredNotes->get_transform(), leftColor, rightColor);

            // Update note sizes
            if (globalConfig.overrideNoteSize)
                mirroredNoteCubeTransform->get_gameObject()->AddComponent<Qosmetics::Notes::BasicNoteScaler*>();

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
        // we want to remove existing reflections when:
        // disable reflectiosn is on OR
        // we have an object, and our object is not mirrorable && we don't want to keep the missing reflections
        else if (globalConfig.disableReflections || (noteModelContainer->currentNoteObject && !config.get_isMirrorable() && !globalConfig.keepMissingReflections))
        {
            mirroredNoteCubeTransform->get_gameObject()->GetComponent<UnityEngine::MeshFilter*>()->set_mesh(nullptr);
            mirroredNoteCubeTransform->Find(ConstStrings::NoteArrow())->get_gameObject()->SetActive(false);
            mirroredNoteCubeTransform->Find(ConstStrings::NoteCircleGlow())->get_gameObject()->SetActive(false);

            if (globalConfig.overrideNoteSize)
                mirroredNoteCubeTransform->get_gameObject()->AddComponent<Qosmetics::Notes::BasicNoteScaler*>();
        }
        else if (globalConfig.overrideNoteSize)
        {
            mirroredNoteCubeTransform->get_gameObject()->AddComponent<Qosmetics::Notes::BasicNoteScaler*>();
        }
    }
    catch (il2cpp_utils::RunMethodException const& e)
    {
        ERROR("{}", e.what());
    }
    return mirroredGameNoteControllerPrefab;
}

#pragma endregion

#pragma region chainNotes

REDECORATION_REGISTRATION(burstSliderHeadNotePrefab, 10, true, GlobalNamespace::GameNoteController*, GlobalNamespace::BeatmapObjectsInstaller*)
{
    if (Qosmetics::Notes::Disabling::GetAnyDisabling())
        return burstSliderHeadNotePrefab;

    try
    {
        GET_CONFIG()
        // get the notecubetransform to edit it's children
        auto noteCubeTransform = burstSliderHeadNotePrefab->get_transform()->Find("NoteCube");
        // if a custom note even exists
        bool addCustomPrefab = noteModelContainer->currentNoteObject && !ghostNotes && !disappearingArrows;
#ifdef CHROMA_EXISTS
        // set the note colorable by chroma to the opposite of if we are replacing the entire contents of the prefab
        Chroma::NoteAPI::setNoteColorable(addCustomPrefab);
#endif
        if (addCustomPrefab)
        {

            auto colorScheme = gameplayCoreSceneSetupData->dyn_colorScheme();
            auto leftColor = colorScheme->dyn__saberAColor();
            auto rightColor = colorScheme->dyn__saberBColor();

#ifdef CHROMA_EXISTS
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
            auto chainParent = burstSliderHeadNotePrefab->get_gameObject()->AddComponent<Qosmetics::Notes::ChainParent*>();

            auto actualChains = noteModelContainer->currentNoteObject->get_transform()->Find(ConstStrings::Chains());
            // instantiate the notes prefab
            auto chains = UnityEngine::Object::Instantiate(actualChains->get_gameObject(), noteCubeTransform);
            Qosmetics::Notes::MaterialUtils::ReplaceMaterialsForGameObject(chains);
            chains->set_name("Chains");
            chains->get_transform()->set_localScale(Sombrero::FastVector3(1.0f, config.get_hasSlider() ? 1.0f : 0.75f, 1.0f) * 0.4f);
            chains->get_transform()->set_localPosition(Sombrero::FastVector3::zero());

            chainParent->chainHandler = chains->GetComponent<Qosmetics::Notes::ChainHandler*>();
            int childCount = chains->get_transform()->get_childCount();

            SetAndFixObjectChildren(chains->get_transform(), leftColor, rightColor);

            if (globalConfig.overrideNoteSize)
            {
                noteCubeTransform->get_gameObject()->AddComponent<Qosmetics::Notes::BasicNoteScaler*>();

                if (!globalConfig.alsoChangeHitboxes)
                {
                    for (auto bigCuttable : burstSliderHeadNotePrefab->dyn__bigCuttableBySaberList())
                        bigCuttable->set_colliderSize(bigCuttable->get_colliderSize() / noteSizeFactor);
                    for (auto smallCuttable : burstSliderHeadNotePrefab->dyn__smallCuttableBySaberList())
                        smallCuttable->set_colliderSize(smallCuttable->get_colliderSize() / noteSizeFactor);
                }
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
            noteCubeTransform->get_gameObject()->AddComponent<Qosmetics::Notes::BasicNoteScaler*>();

            // if we don't want to change hitbox sizes, scale the cuttable hitboxes to make them proper size
            if (!globalConfig.alsoChangeHitboxes)
            {
                for (auto bigCuttable : burstSliderHeadNotePrefab->dyn__bigCuttableBySaberList())
                    bigCuttable->set_colliderSize(bigCuttable->get_colliderSize() / noteSizeFactor);
                for (auto smallCuttable : burstSliderHeadNotePrefab->dyn__smallCuttableBySaberList())
                    smallCuttable->set_colliderSize(smallCuttable->get_colliderSize() / noteSizeFactor);
            }
        }
    }
    catch (il2cpp_utils::RunMethodException const& e)
    {
        ERROR("{}", e.what());
    }
    return burstSliderHeadNotePrefab;
}

REDECORATION_REGISTRATION(burstSliderNotePrefab, 10, true, GlobalNamespace::BurstSliderGameNoteController*, GlobalNamespace::BeatmapObjectsInstaller*)
{
    if (Qosmetics::Notes::Disabling::GetAnyDisabling())
        return burstSliderNotePrefab;

    try
    {
        GET_CONFIG()
        // get the notecubetransform to edit it's children
        auto noteCubeTransform = burstSliderNotePrefab->get_transform()->Find("NoteCube");
        // if a custom note even exists
        bool addCustomPrefab = noteModelContainer->currentNoteObject && !ghostNotes && !disappearingArrows;

        if (addCustomPrefab)
        {
            auto chainParent = burstSliderNotePrefab->get_gameObject()->AddComponent<Qosmetics::Notes::ChainParent*>();

            auto actualChains = noteModelContainer->currentNoteObject->get_transform()->Find(ConstStrings::Chains());
            // instantiate the notes prefab
            auto chains = UnityEngine::Object::Instantiate(actualChains->get_gameObject(), noteCubeTransform);
            Qosmetics::Notes::MaterialUtils::ReplaceMaterialsForGameObject(chains);
            chains->set_name("Chains");
            chains->get_transform()->set_localScale(Sombrero::FastVector3(1.0f, config.get_hasSlider() ? 1.0f : 0.2f, 1.0f) * 0.4f);
            chains->get_transform()->set_localPosition(Sombrero::FastVector3::zero());

            chainParent->chainHandler = chains->GetComponent<Qosmetics::Notes::ChainHandler*>();

            auto colorScheme = gameplayCoreSceneSetupData->dyn_colorScheme();
            auto leftColor = colorScheme->dyn__saberAColor();
            auto rightColor = colorScheme->dyn__saberBColor();

            SetAndFixObjectChildren(chains->get_transform(), leftColor, rightColor);

            // Update note sizes
            if (globalConfig.overrideNoteSize)
            {
                noteCubeTransform->get_gameObject()->AddComponent<Qosmetics::Notes::BasicNoteScaler*>();

                if (!globalConfig.alsoChangeHitboxes)
                {
                    for (auto bigCuttable : burstSliderNotePrefab->dyn__bigCuttableBySaberList())
                        bigCuttable->set_colliderSize(bigCuttable->get_colliderSize() / noteSizeFactor);
                    for (auto smallCuttable : burstSliderNotePrefab->dyn__smallCuttableBySaberList())
                        smallCuttable->set_colliderSize(smallCuttable->get_colliderSize() / noteSizeFactor);
                }
            }

            // if we don't want to show arrows, disable the arrow gameobjects
            if (!config.get_showArrows())
            {
                noteCubeTransform->Find(ConstStrings::Circle())->get_gameObject()->SetActive(false);
            }

            /// don't show the normal mesh of the note
            noteCubeTransform->get_gameObject()->GetComponent<UnityEngine::MeshFilter*>()->set_mesh(nullptr);
        }
        // note didn't exist, but we do want to change note size
        else if (globalConfig.overrideNoteSize)
        {
            noteCubeTransform->get_gameObject()->AddComponent<Qosmetics::Notes::BasicNoteScaler*>();

            // if we don't want to change hitbox sizes, scale the cuttable hitboxes to make them proper size
            if (!globalConfig.alsoChangeHitboxes)
            {
                for (auto bigCuttable : burstSliderNotePrefab->dyn__bigCuttableBySaberList())
                    bigCuttable->set_colliderSize(bigCuttable->get_colliderSize() / noteSizeFactor);
                for (auto smallCuttable : burstSliderNotePrefab->dyn__smallCuttableBySaberList())
                    smallCuttable->set_colliderSize(smallCuttable->get_colliderSize() / noteSizeFactor);
            }
        }
    }
    catch (il2cpp_utils::RunMethodException const& e)
    {
        ERROR("{}", e.what());
    }
    return burstSliderNotePrefab;
}

#pragma endregion