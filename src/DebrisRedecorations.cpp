#include "Disabling.hpp"
#include "GlobalNamespace/ColorScheme.hpp"
#include "GlobalNamespace/GameplayCoreSceneSetupData.hpp"
#include "GlobalNamespace/GameplayModifiers.hpp"
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
#include "CustomTypes/DebrisColorHandler.hpp"
#include "CustomTypes/DebrisHandler.hpp"
#include "CustomTypes/DebrisParent.hpp"
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
        auto colorHandler = child->get_gameObject()->GetComponent<Qosmetics::Notes::DebrisColorHandler*>();
        if (colorHandler)
        {
            colorHandler->FetchCCMaterials();
            if (child->get_name().starts_with("Left"))
                colorHandler->SetColors(leftColor, rightColor);
            else
                colorHandler->SetColors(rightColor, leftColor);
        }
    }
}

GlobalNamespace::NoteDebris* RedecorateNoteDebris(GlobalNamespace::NoteDebris* noteDebrisPrefab, Zenject::DiContainer* container)
{
    if (Qosmetics::Notes::Disabling::GetAnyDisabling())
        return noteDebrisPrefab;
    try
    {
        GET_CONFIG();
        bool addCustomPrefab = noteModelContainer->currentNoteObject && !ghostNotes && !disappearingArrows;

        // if we are adding our own prefab, we have debris, we are not reducing debris, and not forcing default, replace debris
        if (addCustomPrefab && config.get_hasDebris() && !gameplayCoreSceneSetupData->dyn_playerSpecificSettings()->get_reduceDebris() && !globalConfig.forceDefaultDebris)
        {
            auto noteDebrisParent = noteDebrisPrefab->get_gameObject()->AddComponent<Qosmetics::Notes::DebrisParent*>();

            auto meshTransform = noteDebrisPrefab->dyn__meshTransform();
            auto actualDebris = noteModelContainer->currentNoteObject->get_transform()->Find(ConstStrings::Debris());
            auto debris = UnityEngine::Object::Instantiate(actualDebris->get_gameObject(), meshTransform);
            Qosmetics::Notes::MaterialUtils::ReplaceMaterialsForGameObject(debris);
            debris->set_name(ConstStrings::Debris());
            debris->get_transform()->set_localPosition(Sombrero::FastVector3::zero());
            debris->get_transform()->set_localScale(Sombrero::FastVector3::one() * noteSizeFactor * 0.4f);

            auto colorScheme = gameplayCoreSceneSetupData->dyn_colorScheme();
            auto leftColor = colorScheme->dyn__saberAColor();
            auto rightColor = colorScheme->dyn__saberBColor();

            SetAndFixObjectChildren(debris->get_transform(), leftColor, rightColor);

            meshTransform->get_gameObject()->GetComponent<UnityEngine::MeshFilter*>()->set_mesh(nullptr);
        }
        else if (globalConfig.overrideNoteSize)
        {
            auto t = noteDebrisPrefab->get_transform();
            t->set_localScale(t->get_localScale() * noteSizeFactor);
        }
    }
    catch (il2cpp_utils::RunMethodException const& e)
    {
        ERROR("{}", e.what());
    }
    return noteDebrisPrefab;
}

GlobalNamespace::NoteDebris* RedecorateHeadNoteDebris(GlobalNamespace::NoteDebris* noteDebrisPrefab, Zenject::DiContainer* container)
{
    if (Qosmetics::Notes::Disabling::GetAnyDisabling())
        return noteDebrisPrefab;
    try
    {
        GET_CONFIG();
        bool addCustomPrefab = noteModelContainer->currentNoteObject && !ghostNotes && !disappearingArrows;

        // if we are adding our own prefab, we have debris, we are not reducing debris, and not forcing default, replace debris
        if (addCustomPrefab && config.get_hasDebris() && !gameplayCoreSceneSetupData->dyn_playerSpecificSettings()->get_reduceDebris() && !globalConfig.forceDefaultDebris)
        {
            auto noteDebrisParent = noteDebrisPrefab->get_gameObject()->AddComponent<Qosmetics::Notes::DebrisParent*>();

            auto meshTransform = noteDebrisPrefab->dyn__meshTransform();
            auto actualDebris = noteModelContainer->currentNoteObject->get_transform()->Find(ConstStrings::ChainHeadDebris());
            auto debris = UnityEngine::Object::Instantiate(actualDebris->get_gameObject(), meshTransform);
            Qosmetics::Notes::MaterialUtils::ReplaceMaterialsForGameObject(debris);
            debris->set_name(ConstStrings::ChainHeadDebris());
            debris->get_transform()->set_localPosition(Sombrero::FastVector3::zero());
            debris->get_transform()->set_localScale(Sombrero::FastVector3::one() * noteSizeFactor * 0.4f);

            auto colorScheme = gameplayCoreSceneSetupData->dyn_colorScheme();
            auto leftColor = colorScheme->dyn__saberAColor();
            auto rightColor = colorScheme->dyn__saberBColor();

            SetAndFixObjectChildren(debris->get_transform(), leftColor, rightColor);

            meshTransform->get_gameObject()->GetComponent<UnityEngine::MeshFilter*>()->set_mesh(nullptr);
        }
        else if (globalConfig.overrideNoteSize)
        {
            auto t = noteDebrisPrefab->get_transform();
            t->set_localScale(t->get_localScale() * noteSizeFactor);
        }
    }
    catch (il2cpp_utils::RunMethodException const& e)
    {
        ERROR("{}", e.what());
    }
    return noteDebrisPrefab;
}

GlobalNamespace::NoteDebris* RedecorateElementNoteDebris(GlobalNamespace::NoteDebris* noteDebrisPrefab, Zenject::DiContainer* container)
{
    if (Qosmetics::Notes::Disabling::GetAnyDisabling())
        return noteDebrisPrefab;
    try
    {
        GET_CONFIG();
        bool addCustomPrefab = noteModelContainer->currentNoteObject && !ghostNotes && !disappearingArrows;

        // if we are adding our own prefab, we have debris, we are not reducing debris, and not forcing default, replace debris
        if (addCustomPrefab && config.get_hasDebris() && !gameplayCoreSceneSetupData->dyn_playerSpecificSettings()->get_reduceDebris() && !globalConfig.forceDefaultChainDebris)
        {
            auto noteDebrisParent = noteDebrisPrefab->get_gameObject()->AddComponent<Qosmetics::Notes::DebrisParent*>();

            auto meshTransform = noteDebrisPrefab->dyn__meshTransform();
            auto actualDebris = noteModelContainer->currentNoteObject->get_transform()->Find(ConstStrings::ChainLinkDebris());
            auto debris = UnityEngine::Object::Instantiate(actualDebris->get_gameObject(), meshTransform);
            Qosmetics::Notes::MaterialUtils::ReplaceMaterialsForGameObject(debris);
            debris->set_name(ConstStrings::ChainLinkDebris());
            debris->get_transform()->set_localScale(Sombrero::FastVector3::one() * noteSizeFactor * 0.4f);

            auto colorScheme = gameplayCoreSceneSetupData->dyn_colorScheme();
            auto leftColor = colorScheme->dyn__saberAColor();
            auto rightColor = colorScheme->dyn__saberBColor();

            SetAndFixObjectChildren(debris->get_transform(), leftColor, rightColor);

            meshTransform->get_gameObject()->GetComponent<UnityEngine::MeshFilter*>()->set_mesh(nullptr);
        }
        else if (globalConfig.overrideNoteSize)
        {
            auto t = noteDebrisPrefab->get_transform();
            t->set_localScale(t->get_localScale() * noteSizeFactor);
        }
    }
    catch (il2cpp_utils::RunMethodException const& e)
    {
        ERROR("{}", e.what());
    }
    return noteDebrisPrefab;
}

REDECORATION_REGISTRATION(normalNoteDebrisHDPrefab, 10, true, GlobalNamespace::NoteDebris*, GlobalNamespace::NoteDebrisPoolInstaller*)
{
    return RedecorateNoteDebris(normalNoteDebrisHDPrefab, container);
}

REDECORATION_REGISTRATION(normalNoteDebrisLWPrefab, 10, true, GlobalNamespace::NoteDebris*, GlobalNamespace::NoteDebrisPoolInstaller*)
{
    return RedecorateNoteDebris(normalNoteDebrisLWPrefab, container);
}

REDECORATION_REGISTRATION(burstSliderHeadNoteDebrisHDPrefab, 10, true, GlobalNamespace::NoteDebris*, GlobalNamespace::NoteDebrisPoolInstaller*)
{
    return RedecorateHeadNoteDebris(burstSliderHeadNoteDebrisHDPrefab, container);
}

REDECORATION_REGISTRATION(burstSliderHeadNoteDebrisLWPrefab, 10, true, GlobalNamespace::NoteDebris*, GlobalNamespace::NoteDebrisPoolInstaller*)
{
    return RedecorateHeadNoteDebris(burstSliderHeadNoteDebrisLWPrefab, container);
}

REDECORATION_REGISTRATION(burstSliderElementNoteHDPrefab, 10, true, GlobalNamespace::NoteDebris*, GlobalNamespace::NoteDebrisPoolInstaller*)
{
    return RedecorateElementNoteDebris(burstSliderElementNoteHDPrefab, container);
}

REDECORATION_REGISTRATION(burstSliderElementNoteLWPrefab, 10, true, GlobalNamespace::NoteDebris*, GlobalNamespace::NoteDebrisPoolInstaller*)
{
    return RedecorateElementNoteDebris(burstSliderElementNoteLWPrefab, container);
}