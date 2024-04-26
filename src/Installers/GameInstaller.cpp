#include "Installers/GameInstaller.hpp"
#include "ConstStrings.hpp"
#include "Disabling.hpp"
#include "config.hpp"
#include "logging.hpp"

#include "GlobalNamespace/BoxCuttableBySaber.hpp"
#include "GlobalNamespace/ColorScheme.hpp"
#include "GlobalNamespace/FakeMirrorObjectsInstaller.hpp"
#include "GlobalNamespace/GameplayModifiers.hpp"
#include "GlobalNamespace/NoteDebrisPoolInstaller.hpp"
#include "GlobalNamespace/PlayerSpecificSettings.hpp"
#include "UnityEngine/MeshFilter.hpp"
#include "UnityEngine/MeshRenderer.hpp"
#include "UnityEngine/SphereCollider.hpp"
#include "UnityEngine/Transform.hpp"
#include "Zenject/ConcreteIdBinderGeneric_1.hpp"
#include "Zenject/DiContainer.hpp"

#include "CustomTypes/BasicNoteScaler.hpp"
#include "CustomTypes/BombParent.hpp"
#include "CustomTypes/ChainHandler.hpp"
#include "CustomTypes/ChainParent.hpp"
#include "CustomTypes/CyoobColorHandler.hpp"
#include "CustomTypes/CyoobHandler.hpp"
#include "CustomTypes/CyoobParent.hpp"
#include "CustomTypes/DebrisParent.hpp"
#include "MaterialUtils.hpp"

#include "lapiz/shared/objects/Beatmap.hpp"
#include "sombrero/shared/FastColor.hpp"
#include "sombrero/shared/FastQuaternion.hpp"
#include "sombrero/shared/FastVector3.hpp"

#if __has_include("chroma/shared/NoteAPI.hpp")
#include "chroma/shared/BombAPI.hpp"
#include "chroma/shared/NoteAPI.hpp"
#ifndef HAS_CHROMA
#define HAS_CHROMA
#endif
#endif

DEFINE_TYPE(Qosmetics::Notes, GameInstaller);

static UnityEngine::Vector3 operator*(UnityEngine::Vector3 vec, float v)
{
    return {
        vec.x * v,
        vec.y * v,
        vec.z * v};
}

static UnityEngine::Vector3 operator/(UnityEngine::Vector3 vec, float v)
{
    return {
        vec.x / v,
        vec.y / v,
        vec.z / v};
}

template <typename T>
    requires(std::is_same_v<T, Qosmetics::Notes::CyoobColorHandler*> || std::is_same_v<T, Qosmetics::Notes::DebrisColorHandler*>)
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
        auto colorHandler = child->get_gameObject()->GetComponent<T>();
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
namespace Qosmetics::Notes
{
    void GameInstaller::ctor(NoteModelContainer* noteModelContainer, GlobalNamespace::GameplayCoreSceneSetupData* gameplayCoreSceneSetupData)
    {
        _noteModelContainer = noteModelContainer;
        _gameplayCoreSceneSetupData = gameplayCoreSceneSetupData;

        _ghostNotes = _gameplayCoreSceneSetupData->gameplayModifiers->get_ghostNotes();
        _disappearingArrows = _gameplayCoreSceneSetupData->gameplayModifiers->get_disappearingArrows();
        _smallCubes = _gameplayCoreSceneSetupData->gameplayModifiers->get_smallCubes();
        _proMode = _gameplayCoreSceneSetupData->gameplayModifiers->get_proMode();
        _reduceDebris = _gameplayCoreSceneSetupData->playerSpecificSettings->get_reduceDebris();
    }

    void GameInstaller::InstallBindings()
    {
        if (Disabling::GetAnyDisabling())
            return;

        auto& config = _noteModelContainer->GetNoteConfig();
#ifdef HAS_CHROMA
        auto& globalConfig = Qosmetics::Notes::Config::get_config();
        bool hasCustoms = _noteModelContainer->CurrentNoteObject;

        if (hasCustoms)
        {
            bool replaceBombs = config.get_hasBomb() && !(_ghostNotes || _disappearingArrows) && !globalConfig.forceDefaultBombs;
            Chroma::BombAPI::setBombColorable(replaceBombs);
            Chroma::NoteAPI::setNoteColorable(!(_ghostNotes || _disappearingArrows));
        }
        else
        {
            Chroma::BombAPI::setBombColorable(false);
            Chroma::NoteAPI::setNoteColorable(false);
        }
#endif

        auto container = get_Container();

        if (_proMode)
            Lapiz::Objects::Beatmap::ProModeNoteRegistration::New_ctor(std::bind(&GameInstaller::DecorateNote, this, std::placeholders::_1), DECORATION_PRIORITY)->RegisterRedecorator(container);
        else
            Lapiz::Objects::Beatmap::BasicNoteRegistration::New_ctor(std::bind(&GameInstaller::DecorateNote, this, std::placeholders::_1), DECORATION_PRIORITY)->RegisterRedecorator(container);

        Lapiz::Objects::Beatmap::BombNoteRegistration::New_ctor(std::bind(&GameInstaller::DecorateBombs, this, std::placeholders::_1), DECORATION_PRIORITY)->RegisterRedecorator(container);

        Lapiz::Objects::Beatmap::BurstSliderNoteRegistration::New_ctor(std::bind(&GameInstaller::DecorateBurstSliderElement, this, std::placeholders::_1), DECORATION_PRIORITY)->RegisterRedecorator(container);
        Lapiz::Objects::Beatmap::BurstSliderHeadNoteRegistration::New_ctor(std::bind(&GameInstaller::DecorateBurstSliderHead, this, std::placeholders::_1), DECORATION_PRIORITY)->RegisterRedecorator(container);

        // if reduce debris is used, don't even bother redecorating the debris
        if (!_reduceDebris)
        {
            Lapiz::Objects::Registration<GlobalNamespace::NoteDebris*, GlobalNamespace::NoteDebrisPoolInstaller*>::New_ctor("_normalNoteDebrisHDPrefab", std::bind(&GameInstaller::DecorateNoteDebris, this, std::placeholders::_1), DECORATION_PRIORITY)->RegisterRedecorator(container);
            Lapiz::Objects::Registration<GlobalNamespace::NoteDebris*, GlobalNamespace::NoteDebrisPoolInstaller*>::New_ctor("_burstSliderHeadNoteDebrisHDPrefab", std::bind(&GameInstaller::DecorateHeadNoteDebris, this, std::placeholders::_1), DECORATION_PRIORITY)->RegisterRedecorator(container);
            Lapiz::Objects::Registration<GlobalNamespace::NoteDebris*, GlobalNamespace::NoteDebrisPoolInstaller*>::New_ctor("_burstSliderElementNoteHDPrefab", std::bind(&GameInstaller::DecorateLinkNoteDebris, this, std::placeholders::_1), DECORATION_PRIORITY)->RegisterRedecorator(container);

            Lapiz::Objects::Registration<GlobalNamespace::NoteDebris*, GlobalNamespace::NoteDebrisPoolInstaller*>::New_ctor("_normalNoteDebrisLWPrefab", std::bind(&GameInstaller::DecorateNoteDebris, this, std::placeholders::_1), DECORATION_PRIORITY)->RegisterRedecorator(container);
            Lapiz::Objects::Registration<GlobalNamespace::NoteDebris*, GlobalNamespace::NoteDebrisPoolInstaller*>::New_ctor("_burstSliderHeadNoteDebrisLWPrefab", std::bind(&GameInstaller::DecorateHeadNoteDebris, this, std::placeholders::_1), DECORATION_PRIORITY)->RegisterRedecorator(container);
            Lapiz::Objects::Registration<GlobalNamespace::NoteDebris*, GlobalNamespace::NoteDebrisPoolInstaller*>::New_ctor("_burstSliderElementNoteLWPrefab", std::bind(&GameInstaller::DecorateLinkNoteDebris, this, std::placeholders::_1), DECORATION_PRIORITY)->RegisterRedecorator(container);
        }

        Lapiz::Objects::Registration<GlobalNamespace::MirroredGameNoteController*, GlobalNamespace::FakeMirrorObjectsInstaller*>::New_ctor("_mirroredGameNoteControllerPrefab", std::bind(&GameInstaller::DecorateMirrorNote, this, std::placeholders::_1), DECORATION_PRIORITY)->RegisterRedecorator(container);
        Lapiz::Objects::Registration<GlobalNamespace::MirroredGameNoteController*, GlobalNamespace::FakeMirrorObjectsInstaller*>::New_ctor("_mirroredBurstSliderHeadGameNoteControllerPrefab", std::bind(&GameInstaller::DecorateMirrorBurstSliderHead, this, std::placeholders::_1), DECORATION_PRIORITY)->RegisterRedecorator(container);
        Lapiz::Objects::Registration<GlobalNamespace::MirroredGameNoteController*, GlobalNamespace::FakeMirrorObjectsInstaller*>::New_ctor("_mirroredBurstSliderGameNoteControllerPrefab", std::bind(&GameInstaller::DecorateMirrorBurstSliderElement, this, std::placeholders::_1), DECORATION_PRIORITY)->RegisterRedecorator(container);
        Lapiz::Objects::Registration<GlobalNamespace::MirroredBombNoteController*, GlobalNamespace::FakeMirrorObjectsInstaller*>::New_ctor("_mirroredBombNoteControllerPrefab", std::bind(&GameInstaller::DecorateMirrorBomb, this, std::placeholders::_1), DECORATION_PRIORITY)->RegisterRedecorator(container);
    }

    /// @brief Makes it so all hitbox changes are unified in 1 method, making it easier to edit what gets done
    /// @param globalConfig the config used
    /// @param original the original object
    /// @param noteCubeTransform the notecube of the object
    /// @param notesUniformScale the uniform scale of notes (small / normal notes?)
    template <typename T>
    void SetupForNoteHitboxChanges(const Qosmetics::Notes::Config& globalConfig, T original, UnityEngine::Transform* noteCubeTransform, float notesUniformScale)
    {
        DEBUG("Setting up hitbox changes for type {}", classof(T)->name);
        auto noteScaler = noteCubeTransform->get_gameObject()->AddComponent<Qosmetics::Notes::BasicNoteScaler*>();

        float overrideNoteSize = globalConfig.get_overrideNoteSize() ? globalConfig.noteSize : 1.0f;
        noteScaler->noteSize = Sombrero::FastVector3::one() * overrideNoteSize * notesUniformScale;

        // if we don't want to change hitbox sizes, scale the cuttable hitboxes to make them proper size
        if (!globalConfig.get_alsoChangeHitboxes())
        {
            DEBUG("note Size: {}, scale: {}", overrideNoteSize, notesUniformScale);
            for (auto cuttable : original->_bigCuttableBySaberList)
            {
                auto sz = cuttable->get_colliderSize();
                DEBUG("Big Collider size: {}, {}, {}", sz.x, sz.y, sz.z);
                cuttable->set_colliderSize((cuttable->get_colliderSize() / overrideNoteSize) * notesUniformScale);
            }

            for (auto cuttable : original->_smallCuttableBySaberList)
            {
                auto sz = cuttable->get_colliderSize();
                DEBUG("Small Collider size: {}, {}, {}", sz.x, sz.y, sz.z);
                cuttable->set_colliderSize((cuttable->get_colliderSize() / overrideNoteSize) * notesUniformScale);
            }
        }
    }

    void SetupForBombHitboxChanges(const Qosmetics::Notes::Config& globalConfig, GlobalNamespace::BombNoteController* original, UnityEngine::Transform* mesh, float notesUniformScale)
    {
        float overrideNoteSize = globalConfig.get_overrideNoteSize() ? globalConfig.noteSize : 1.0f;
        mesh->set_localScale(mesh->localScale * overrideNoteSize);

        if (!globalConfig.get_alsoChangeHitboxes())
        {
            auto sphereCollider = mesh->get_gameObject()->GetComponent<UnityEngine::SphereCollider*>();
            sphereCollider->set_radius(sphereCollider->get_radius() / overrideNoteSize);
        }
    }

    /* regular dot / arrow notes */
    GlobalNamespace::GameNoteController* GameInstaller::DecorateNote(GlobalNamespace::GameNoteController* original)
    {
        auto colorScheme = _gameplayCoreSceneSetupData->colorScheme;
        auto leftColor = colorScheme->saberAColor;
        auto rightColor = colorScheme->saberBColor;

#ifdef HAS_CHROMA
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
        auto noteCubeTransform = original->get_transform()->Find(ConstStrings::NoteCube());
        auto currentNoteObject = _noteModelContainer->CurrentNoteObject;

        auto& config = _noteModelContainer->GetNoteConfig();
        auto& globalConfig = Qosmetics::Notes::Config::get_config();
        float noteSizeFactor = (globalConfig.get_overrideNoteSize() ? globalConfig.noteSize : 1.0f) * _gameplayCoreSceneSetupData->gameplayModifiers->get_notesUniformScale();

        if (!(_ghostNotes || _disappearingArrows) && currentNoteObject)
        {
            auto objectName = ConstStrings::Notes();
            auto objectParent = original->get_gameObject()->AddComponent<CyoobParent*>();
            auto notesToUse = currentNoteObject->get_transform()->Find(objectName);

            auto notes = UnityEngine::Object::Instantiate(notesToUse->get_gameObject(), noteCubeTransform);
            Qosmetics::Notes::MaterialUtils::ReplaceMaterialsForGameObject(notes);
            notes->set_name(objectName);
            notes->get_transform()->set_localPosition(Sombrero::FastVector3::zero());
            notes->get_transform()->set_localScale(Sombrero::FastVector3::one() * 0.4f);

            objectParent->Handler = notes->GetComponent<CyoobHandler*>();
            int childCount = notes->get_transform()->get_childCount();
            SetAndFixObjectChildren<CyoobColorHandler*>(notes->get_transform(), leftColor, rightColor);

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

            noteCubeTransform->get_gameObject()->GetComponent<UnityEngine::MeshFilter*>()->set_mesh(nullptr);
        }

        if (globalConfig.get_overrideNoteSize())
        {
            auto noteScaler = noteCubeTransform->get_gameObject()->AddComponent<Qosmetics::Notes::BasicNoteScaler*>();
            noteScaler->notesUniformScale = _gameplayCoreSceneSetupData->gameplayModifiers->get_notesUniformScale();

            // if we don't want to change hitbox sizes, scale the cuttable hitboxes to make them proper size
            if (!globalConfig.get_alsoChangeHitboxes())
            {
                for (auto bigCuttable : original->bigCuttableBySaberList)
                    bigCuttable->set_colliderSize(bigCuttable->get_colliderSize() / noteSizeFactor);
                for (auto smallCuttable : original->smallCuttableBySaberList)
                    smallCuttable->set_colliderSize(smallCuttable->get_colliderSize() / noteSizeFactor);
            }
        }

        return original;
    }

    /* bombs */
    GlobalNamespace::BombNoteController* GameInstaller::DecorateBombs(GlobalNamespace::BombNoteController* original)
    {
        auto mesh = original->get_transform()->Find(ConstStrings::Mesh());
        auto currentNoteObject = _noteModelContainer->CurrentNoteObject;

        auto& config = _noteModelContainer->GetNoteConfig();
        auto& globalConfig = Qosmetics::Notes::Config::get_config();
        float noteSizeFactor = (globalConfig.get_overrideNoteSize() ? globalConfig.noteSize : 1.0f) * _gameplayCoreSceneSetupData->gameplayModifiers->get_notesUniformScale();

        // if we have a note object, and a bomb exists on it, and we don't force default
        if (!(_ghostNotes || _disappearingArrows || globalConfig.forceDefaultBombs) && currentNoteObject && config.get_hasBomb())
        {
#ifdef HAS_CHROMA
            auto bombCallbackOpt = Chroma::BombAPI::getBombChangedColorCallbackSafe();
            if (bombCallbackOpt)
            {
                INFO("Adding to note color callback!");
                auto& bombCallback = bombCallbackOpt.value().get();
                bombCallback -= &Qosmetics::Notes::BombParent::ColorizeSpecific;
                bombCallback += &Qosmetics::Notes::BombParent::ColorizeSpecific;
            }
#endif
            original->get_gameObject()->AddComponent<Qosmetics::Notes::BombParent*>();

            auto bombPrefab = currentNoteObject->get_transform()->Find(ConstStrings::Bomb());
            auto bomb = UnityEngine::Object::Instantiate(bombPrefab->get_gameObject(), mesh);
            Qosmetics::Notes::MaterialUtils::ReplaceMaterialsForGameObject(bomb);

            bomb->AddComponent<Qosmetics::Notes::BombColorHandler*>();
            bomb->set_name(ConstStrings::Bomb());
            bomb->get_transform()->set_localScale(Sombrero::FastVector3::one() * 0.4f);
            bomb->get_transform()->set_localPosition(Sombrero::FastVector3::zero());
            bomb->get_transform()->set_localRotation(Sombrero::FastQuaternion::identity());
            auto meshRenderer = mesh->get_gameObject()->GetComponent<UnityEngine::MeshRenderer*>();
            meshRenderer->set_enabled(false);
        }
        // change note size
        if (globalConfig.get_overrideNoteSize())
        {
            mesh->set_localScale(mesh->get_localScale() * noteSizeFactor);
            if (!globalConfig.get_alsoChangeHitboxes())
            {
                auto sphereCollider = mesh->get_gameObject()->GetComponent<UnityEngine::SphereCollider*>();
                sphereCollider->set_radius(sphereCollider->get_radius() / noteSizeFactor);
            }
        }
        return original;
    }

    /* chain link */
    GlobalNamespace::BurstSliderGameNoteController* GameInstaller::DecorateBurstSliderElement(GlobalNamespace::BurstSliderGameNoteController* original)
    {
        auto colorScheme = _gameplayCoreSceneSetupData->colorScheme;
        auto leftColor = colorScheme->saberAColor;
        auto rightColor = colorScheme->saberBColor;

        auto noteCubeTransform = original->get_transform()->Find(ConstStrings::NoteCube());
        auto currentNoteObject = _noteModelContainer->CurrentNoteObject;

        auto& config = _noteModelContainer->GetNoteConfig();
        auto& globalConfig = Qosmetics::Notes::Config::get_config();
        float noteSizeFactor = (globalConfig.get_overrideNoteSize() ? globalConfig.noteSize : 1.0f) * _gameplayCoreSceneSetupData->gameplayModifiers->get_notesUniformScale();

        if (!(_ghostNotes || _disappearingArrows) && currentNoteObject && !globalConfig.forceDefaultChains)
        {
            auto objectName = ConstStrings::Chains();
            auto objectParent = original->get_gameObject()->AddComponent<ChainParent*>();
            auto notesToUse = currentNoteObject->get_transform()->Find(objectName);

            auto notes = UnityEngine::Object::Instantiate(notesToUse->get_gameObject(), noteCubeTransform);
            Qosmetics::Notes::MaterialUtils::ReplaceMaterialsForGameObject(notes);
            notes->set_name(objectName);
            notes->get_transform()->set_localPosition(Sombrero::FastVector3::zero());
            notes->get_transform()->set_localScale(Sombrero::FastVector3::one() * 0.4f);

            objectParent->Handler = notes->GetComponent<ChainHandler*>();
            int childCount = notes->get_transform()->get_childCount();
            SetAndFixObjectChildren<CyoobColorHandler*>(notes->get_transform(), leftColor, rightColor);

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

            noteCubeTransform->get_gameObject()->GetComponent<UnityEngine::MeshFilter*>()->set_mesh(nullptr);
        }

        if (globalConfig.get_overrideNoteSize())
        {
            auto noteScaler = noteCubeTransform->get_gameObject()->AddComponent<Qosmetics::Notes::BasicNoteScaler*>();
            noteScaler->notesUniformScale = _gameplayCoreSceneSetupData->gameplayModifiers->get_notesUniformScale();

            // if we don't want to change hitbox sizes, scale the cuttable hitboxes to make them proper size
            if (!globalConfig.get_alsoChangeHitboxes())
            {
                for (auto bigCuttable : original->bigCuttableBySaberList)
                    bigCuttable->set_colliderSize(bigCuttable->get_colliderSize() / noteSizeFactor);
                for (auto smallCuttable : original->smallCuttableBySaberList)
                    smallCuttable->set_colliderSize(smallCuttable->get_colliderSize() / noteSizeFactor);
            }
        }

        return original;
    }

    /* chain head */
    GlobalNamespace::GameNoteController* GameInstaller::DecorateBurstSliderHead(GlobalNamespace::GameNoteController* original)
    {
        auto colorScheme = _gameplayCoreSceneSetupData->colorScheme;
        auto leftColor = colorScheme->saberAColor;
        auto rightColor = colorScheme->saberBColor;

#ifdef HAS_CHROMA
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
        auto noteCubeTransform = original->get_transform()->Find(ConstStrings::NoteCube());
        auto currentNoteObject = _noteModelContainer->CurrentNoteObject;

        auto& config = _noteModelContainer->GetNoteConfig();
        auto& globalConfig = Qosmetics::Notes::Config::get_config();
        float noteSizeFactor = (globalConfig.get_overrideNoteSize() ? globalConfig.noteSize : 1.0f) * _gameplayCoreSceneSetupData->gameplayModifiers->get_notesUniformScale();

        if (!(_ghostNotes || _disappearingArrows) && currentNoteObject && !globalConfig.forceDefaultChains)
        {
            auto objectName = ConstStrings::Chains();
            auto objectParent = original->get_gameObject()->AddComponent<ChainParent*>();
            auto notesToUse = currentNoteObject->get_transform()->Find(objectName);

            auto notes = UnityEngine::Object::Instantiate(notesToUse->get_gameObject(), noteCubeTransform);
            Qosmetics::Notes::MaterialUtils::ReplaceMaterialsForGameObject(notes);
            notes->set_name(objectName);
            notes->get_transform()->set_localPosition(Sombrero::FastVector3::zero());
            notes->get_transform()->set_localScale(Sombrero::FastVector3::one() * 0.4f);

            objectParent->Handler = notes->GetComponent<ChainHandler*>();
            int childCount = notes->get_transform()->get_childCount();
            SetAndFixObjectChildren<CyoobColorHandler*>(notes->get_transform(), leftColor, rightColor);

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

            noteCubeTransform->get_gameObject()->GetComponent<UnityEngine::MeshFilter*>()->set_mesh(nullptr);
        }

        if (globalConfig.get_overrideNoteSize())
        {
            auto noteScaler = noteCubeTransform->get_gameObject()->AddComponent<Qosmetics::Notes::BasicNoteScaler*>();
            noteScaler->notesUniformScale = _gameplayCoreSceneSetupData->gameplayModifiers->get_notesUniformScale();

            // if we don't want to change hitbox sizes, scale the cuttable hitboxes to make them proper size
            if (!globalConfig.get_alsoChangeHitboxes())
            {
                for (auto bigCuttable : original->bigCuttableBySaberList)
                    bigCuttable->set_colliderSize(bigCuttable->get_colliderSize() / noteSizeFactor);
                for (auto smallCuttable : original->smallCuttableBySaberList)
                    smallCuttable->set_colliderSize(smallCuttable->get_colliderSize() / noteSizeFactor);
            }
        }

        return original;
    }

    /* regular debris */
    GlobalNamespace::NoteDebris* GameInstaller::DecorateNoteDebris(GlobalNamespace::NoteDebris* original)
    {
        return GeneralDecorateDebris(original, ConstStrings::Debris(), _noteModelContainer->GetNoteConfig().get_hasDebris(), Qosmetics::Notes::Config::get_config().forceDefaultDebris);
    }

    /* chain head debris */
    GlobalNamespace::NoteDebris* GameInstaller::DecorateHeadNoteDebris(GlobalNamespace::NoteDebris* original)
    {
        auto& globalConfig = Qosmetics::Notes::Config::get_config();
        return GeneralDecorateDebris(original, ConstStrings::ChainHeadDebris(), !globalConfig.forceDefaultChains && _noteModelContainer->GetNoteConfig().get_hasDebris(), globalConfig.forceDefaultChainDebris || globalConfig.forceDefaultDebris);
    }

    /* chain link debris */
    GlobalNamespace::NoteDebris* GameInstaller::DecorateLinkNoteDebris(GlobalNamespace::NoteDebris* original)
    {
        auto& globalConfig = Qosmetics::Notes::Config::get_config();
        return GeneralDecorateDebris(original, ConstStrings::ChainLinkDebris(), !globalConfig.forceDefaultChains && _noteModelContainer->GetNoteConfig().get_hasDebris(), globalConfig.forceDefaultChainDebris || globalConfig.forceDefaultDebris);
    }

    /* For each debris we do the same thing anyways, so a general method for it suffices */
    GlobalNamespace::NoteDebris* GameInstaller::GeneralDecorateDebris(GlobalNamespace::NoteDebris* original, StringW targetName, bool useDebris, bool forceDefaultDebris)
    {
        auto colorScheme = _gameplayCoreSceneSetupData->colorScheme;
        auto leftColor = colorScheme->saberAColor;
        auto rightColor = colorScheme->saberBColor;

        auto currentNoteObject = _noteModelContainer->CurrentNoteObject;
        bool addCustomPrefab = !(_ghostNotes || _disappearingArrows) && useDebris && !forceDefaultDebris;

        auto& config = _noteModelContainer->GetNoteConfig();
        auto& globalConfig = Qosmetics::Notes::Config::get_config();
        float noteSizeFactor = (globalConfig.get_overrideNoteSize() ? globalConfig.noteSize : 1.0f) * _gameplayCoreSceneSetupData->gameplayModifiers->get_notesUniformScale();

        // if we are adding our own prefab, we have debris, we are not reducing debris, and not forcing default, replace debris
        if (currentNoteObject && useDebris)
        {
            auto noteDebrisParent = original->get_gameObject()->AddComponent<Qosmetics::Notes::DebrisParent*>();

            auto meshTransform = original->_meshTransform;
            auto actualDebris = currentNoteObject->get_transform()->Find(targetName);
            auto debris = UnityEngine::Object::Instantiate(actualDebris->get_gameObject(), meshTransform);
            Qosmetics::Notes::MaterialUtils::ReplaceMaterialsForGameObject(debris);
            debris->set_name(targetName);
            debris->get_transform()->set_localPosition(Sombrero::FastVector3::zero());
            debris->get_transform()->set_localScale(Sombrero::FastVector3::one() * noteSizeFactor * 0.4f);

            SetAndFixObjectChildren<DebrisColorHandler*>(debris->get_transform(), leftColor, rightColor);

            meshTransform->get_gameObject()->GetComponent<UnityEngine::MeshFilter*>()->set_mesh(nullptr);
        }
        else if (globalConfig.get_overrideNoteSize())
        {
            auto t = original->get_transform();
            t->set_localScale(t->get_localScale() * noteSizeFactor);
        }

        return original;
    }

    /* Mirror note */
    GlobalNamespace::MirroredGameNoteController* GameInstaller::DecorateMirrorNote(GlobalNamespace::MirroredGameNoteController* original)
    {
        DEBUG("DecorateMirrorNote!");
        auto colorScheme = _gameplayCoreSceneSetupData->colorScheme;
        auto leftColor = colorScheme->saberAColor;
        auto rightColor = colorScheme->saberBColor;

        auto noteCubeTransform = original->get_transform()->Find(ConstStrings::NoteCube());
        auto currentNoteObject = _noteModelContainer->CurrentNoteObject;

        auto& config = _noteModelContainer->GetNoteConfig();
        auto& globalConfig = Qosmetics::Notes::Config::get_config();
        float noteSizeFactor = (globalConfig.get_overrideNoteSize() ? globalConfig.noteSize : 1.0f) * _gameplayCoreSceneSetupData->gameplayModifiers->get_notesUniformScale();

        if (!(_ghostNotes || _disappearingArrows) && currentNoteObject && config.get_isMirrorable() && !globalConfig.disableReflections)
        {
            DEBUG("Replacing Mirrorable Prefab!");
            auto objectName = ConstStrings::MirrorNotes();
            auto objectParent = original->get_gameObject()->AddComponent<CyoobParent*>();
            auto notesToUse = currentNoteObject->get_transform()->Find(objectName);

            auto notes = UnityEngine::Object::Instantiate(notesToUse->get_gameObject(), noteCubeTransform);
            Qosmetics::Notes::MaterialUtils::ReplaceMaterialsForGameObject(notes);
            notes->set_name(objectName);
            notes->get_transform()->set_localPosition(Sombrero::FastVector3::zero());
            notes->get_transform()->set_localScale(Sombrero::FastVector3::one() * 0.4f);

            objectParent->Handler = notes->GetComponent<CyoobHandler*>();
            SetAndFixObjectChildren<CyoobColorHandler*>(notes->get_transform(), leftColor, rightColor);

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

            noteCubeTransform->get_gameObject()->GetComponent<UnityEngine::MeshFilter*>()->set_mesh(nullptr);
        }
        else if (globalConfig.disableReflections || (currentNoteObject && !config.get_isMirrorable() && !globalConfig.keepMissingReflections))
        {
            noteCubeTransform->get_gameObject()->GetComponent<UnityEngine::MeshFilter*>()->set_mesh(nullptr);
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

        if (globalConfig.get_overrideNoteSize())
        {
            auto noteScaler = noteCubeTransform->get_gameObject()->AddComponent<Qosmetics::Notes::BasicNoteScaler*>();
            noteScaler->notesUniformScale = _gameplayCoreSceneSetupData->gameplayModifiers->get_notesUniformScale();
        }

        return original;
    }

    /* Mirror chain head */
    GlobalNamespace::MirroredGameNoteController* GameInstaller::DecorateMirrorBurstSliderHead(GlobalNamespace::MirroredGameNoteController* original)
    {
        auto colorScheme = _gameplayCoreSceneSetupData->colorScheme;
        auto leftColor = colorScheme->saberAColor;
        auto rightColor = colorScheme->saberBColor;

        auto noteCubeTransform = original->get_transform()->Find(ConstStrings::NoteCube());
        auto currentNoteObject = _noteModelContainer->CurrentNoteObject;

        auto& config = _noteModelContainer->GetNoteConfig();
        auto& globalConfig = Qosmetics::Notes::Config::get_config();
        float noteSizeFactor = (globalConfig.get_overrideNoteSize() ? globalConfig.noteSize : 1.0f) * _gameplayCoreSceneSetupData->gameplayModifiers->get_notesUniformScale();

        if (!(_ghostNotes || _disappearingArrows) && currentNoteObject && config.get_isMirrorable() && !globalConfig.disableReflections && !globalConfig.forceDefaultChains)
        {
            auto objectName = ConstStrings::MirrorChains();
            auto objectParent = original->get_gameObject()->AddComponent<ChainParent*>();
            auto notesToUse = currentNoteObject->get_transform()->Find(objectName);

            auto notes = UnityEngine::Object::Instantiate(notesToUse->get_gameObject(), noteCubeTransform);
            Qosmetics::Notes::MaterialUtils::ReplaceMaterialsForGameObject(notes);
            notes->set_name(objectName);
            notes->get_transform()->set_localPosition(Sombrero::FastVector3::zero());
            notes->get_transform()->set_localScale(Sombrero::FastVector3::one() * 0.4f);

            objectParent->Handler = notes->GetComponent<ChainHandler*>();
            SetAndFixObjectChildren<CyoobColorHandler*>(notes->get_transform(), leftColor, rightColor);

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

            noteCubeTransform->get_gameObject()->GetComponent<UnityEngine::MeshFilter*>()->set_mesh(nullptr);
        }
        else if (globalConfig.disableReflections || (currentNoteObject && !config.get_isMirrorable() && !globalConfig.keepMissingReflections))
        {
            noteCubeTransform->get_gameObject()->GetComponent<UnityEngine::MeshFilter*>()->set_mesh(nullptr);
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

        if (globalConfig.get_overrideNoteSize())
        {
            auto noteScaler = noteCubeTransform->get_gameObject()->AddComponent<Qosmetics::Notes::BasicNoteScaler*>();
            noteScaler->notesUniformScale = _gameplayCoreSceneSetupData->gameplayModifiers->get_notesUniformScale();
        }

        return original;
    }

    /* Mirror chain link */
    GlobalNamespace::MirroredGameNoteController* GameInstaller::DecorateMirrorBurstSliderElement(GlobalNamespace::MirroredGameNoteController* original)
    {
        auto colorScheme = _gameplayCoreSceneSetupData->colorScheme;
        auto leftColor = colorScheme->saberAColor;
        auto rightColor = colorScheme->saberBColor;

        auto noteCubeTransform = original->get_transform()->Find(ConstStrings::NoteCube());
        auto currentNoteObject = _noteModelContainer->CurrentNoteObject;

        auto& config = _noteModelContainer->GetNoteConfig();
        auto& globalConfig = Qosmetics::Notes::Config::get_config();
        float noteSizeFactor = (globalConfig.get_overrideNoteSize() ? globalConfig.noteSize : 1.0f) * _gameplayCoreSceneSetupData->gameplayModifiers->get_notesUniformScale();

        if (!(_ghostNotes || _disappearingArrows) && currentNoteObject && config.get_isMirrorable() && !globalConfig.disableReflections && !globalConfig.forceDefaultChains)
        {
            auto objectName = ConstStrings::MirrorChains();
            auto objectParent = original->get_gameObject()->AddComponent<ChainParent*>();
            auto notesToUse = currentNoteObject->get_transform()->Find(objectName);

            auto notes = UnityEngine::Object::Instantiate(notesToUse->get_gameObject(), noteCubeTransform);
            Qosmetics::Notes::MaterialUtils::ReplaceMaterialsForGameObject(notes);
            notes->set_name(objectName);
            notes->get_transform()->set_localPosition(Sombrero::FastVector3::zero());
            notes->get_transform()->set_localScale(Sombrero::FastVector3::one() * 0.4f);

            objectParent->Handler = notes->GetComponent<ChainHandler*>();
            SetAndFixObjectChildren<CyoobColorHandler*>(notes->get_transform(), leftColor, rightColor);

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

            noteCubeTransform->get_gameObject()->GetComponent<UnityEngine::MeshFilter*>()->set_mesh(nullptr);
        }
        else if (globalConfig.disableReflections || (currentNoteObject && !config.get_isMirrorable() && !globalConfig.keepMissingReflections))
        {
            noteCubeTransform->get_gameObject()->GetComponent<UnityEngine::MeshFilter*>()->set_mesh(nullptr);
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

        if (globalConfig.get_overrideNoteSize())
        {
            auto noteScaler = noteCubeTransform->get_gameObject()->AddComponent<Qosmetics::Notes::BasicNoteScaler*>();
            noteScaler->notesUniformScale = _gameplayCoreSceneSetupData->gameplayModifiers->get_notesUniformScale();
        }

        return original;
    }

    /* Mirror bomb */
    GlobalNamespace::MirroredBombNoteController* GameInstaller::DecorateMirrorBomb(GlobalNamespace::MirroredBombNoteController* original)
    {
        auto mesh = original->get_transform()->Find(ConstStrings::Mesh());
        auto meshRenderer = mesh->get_gameObject()->GetComponent<UnityEngine::MeshRenderer*>();
        auto currentNoteObject = _noteModelContainer->CurrentNoteObject;

        auto& config = _noteModelContainer->GetNoteConfig();
        auto& globalConfig = Qosmetics::Notes::Config::get_config();
        float noteSizeFactor = (globalConfig.get_overrideNoteSize() ? globalConfig.noteSize : 1.0f) * _gameplayCoreSceneSetupData->gameplayModifiers->get_notesUniformScale();
        if (!(_ghostNotes || _disappearingArrows || globalConfig.forceDefaultBombs || globalConfig.disableReflections) && currentNoteObject && config.get_hasBomb() && config.get_isMirrorable())
        {
            meshRenderer->set_enabled(false);
            auto bombPrefab = currentNoteObject->get_transform()->Find(ConstStrings::MirrorBomb());
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
            if (globalConfig.get_overrideNoteSize())
            {
                mesh->set_localScale(mesh->get_localScale() * noteSizeFactor);
            }
        }
        else if (globalConfig.get_overrideNoteSize())
        {
            mesh->set_localScale(mesh->get_localScale() * noteSizeFactor);
        }

        return original;
    }

}
