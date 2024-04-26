#include "UI/PreviewViewController.hpp"
#include "ConstStrings.hpp"
#include "CustomTypes/CyoobColorHandler.hpp"
#include "CustomTypes/DebrisColorHandler.hpp"
#include "assets.hpp"
#include "config.hpp"
#include "logging.hpp"

#include "qosmetics-core/shared/Utils/DateUtils.hpp"
#include "qosmetics-core/shared/Utils/RainbowUtils.hpp"
#include "sombrero/shared/FastColor.hpp"
#include "sombrero/shared/FastVector3.hpp"

#include "GlobalNamespace/ColorManager.hpp"
#include "GlobalNamespace/ColorScheme.hpp"
#include "GlobalNamespace/ColorSchemesSettings.hpp"
#include "GlobalNamespace/ColorType.hpp"
#include "GlobalNamespace/PlayerData.hpp"
#include "HMUI/CurvedCanvasSettingsHelper.hpp"
#include "System/Collections/Generic/Dictionary_2.hpp"

#include "bsml/shared/BSML.hpp"
#include "bsml/shared/BSML/Components/Backgroundable.hpp"

DEFINE_TYPE(Qosmetics::Notes, PreviewViewController);

/// @brief Set object size, position and color
/// @tparam the color handler to do stuff with
/// @param obj the object to set stuff on
/// @param objectSize the size the object will be
/// @param localPosition the localPosition to set
/// @param thisColor the color applied to _Color
/// @param thatColor the color applied to _OtherColor
template <typename T>
    requires(std::is_convertible_v<T, Qosmetics::Notes::CyoobColorHandler*> || std::is_convertible_v<T, Qosmetics::Notes::DebrisColorHandler*>)
void SetObjectSizePosColor(UnityEngine::Transform* obj, float objectSize, const Sombrero::FastVector3& localPosition, const Sombrero::FastColor& thisColor, const Sombrero::FastColor& thatColor)
{
    obj->set_localScale(Sombrero::FastVector3::one() * objectSize);
    obj->set_localPosition(localPosition);
    auto colorHandler = obj->get_gameObject()->GetComponent<T>();
    if (colorHandler)
    {
        colorHandler->FetchCCMaterials();
        colorHandler->SetColors(thisColor, thatColor);
    }
}

/// @brief Intended to get or duplicate a given link object
/// @param parent the parent object to check in
/// @param original the original in case we need to duplicate
/// @param name the name of the object
static UnityEngine::Transform* GetOrDuplicateLink(UnityEngine::Transform* parent, UnityEngine::Transform* original, StringW name)
{
    auto link = parent->Find(name);
    if (!link)
    {
        link = UnityEngine::Object::Instantiate(original->get_gameObject(), parent)->get_transform();
        link->set_name(name);
    }

    return link;
}
namespace Qosmetics::Notes
{
    bool PreviewViewController::justChangedProfile = false;

    void PreviewViewController::Inject(Qosmetics::Notes::NoteModelContainer* noteModelContainer, GlobalNamespace::PlayerDataModel* playerDataModel)
    {
        this->noteModelContainer = noteModelContainer;
        this->playerDataModel = playerDataModel;
    }

    void PreviewViewController::DidDeactivate(bool removedFromHierarchy, bool screenSystemDisabling)
    {
        if (currentPrefab)
            currentPrefab->SetActive(false);
    }

    StringW PreviewViewController::get_gaydient() { return Qosmetics::Core::RainbowUtils::randomGradient(); }
    bool PreviewViewController::get_gay() { return Qosmetics::Core::DateUtils::isMonth(6); }

    void PreviewViewController::DidActivate(bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling)
    {
        if (firstActivation)
        {
            auto parser = BSML::parse_and_construct(Assets::Views::PreviewView_bsml, get_transform(), this);
            auto params = parser->parserParams.get();
            auto objectBG = params->GetObjectsWithTag("objectBG").at(0)->GetComponent<BSML::Backgroundable*>();
            auto imageView = objectBG->background;
            imageView->_skew = 0;
            imageView->set_gradient(true);
            imageView->_gradientDirection = 1;
            imageView->set_color(Sombrero::FastColor::white());
            auto color = Sombrero::FastColor::get_black();
            color.a = 0.3f;
            imageView->set_color0(color);
            color.a = 0.7f;
            imageView->set_color1(color);
            imageView->_curvedCanvasSettingsHelper->Reset();

            ShowLoading(true);
            UpdatePreview(true);
        }
        else
        {
            if (currentPrefab)
                currentPrefab->SetActive(true);
            DEBUG("Reactivating preview view controller with justChangedProfile: {}", justChangedProfile ? "true" : "false");
            UpdatePreview(justChangedProfile);

            justChangedProfile = false;
        }
    }

    void PreviewViewController::SetTitleText(StringW text)
    {
        if (!(title && title->m_CachedPtr))
            return;
        if (Qosmetics::Core::DateUtils::isMonth(6))
        {
            text = "<i>" + Qosmetics::Core::RainbowUtils::gayify(static_cast<std::string>(text)) + "</i>";
            title->set_text(text);
        }
        else
            title->set_text(u"<i>" + text + u"</i>");
    }

    void PreviewViewController::ShowLoading(bool isLoading)
    {
        if (!(loadingIndicator && loadingIndicator->m_CachedPtr))
            return;

        loadingIndicator->get_gameObject()->SetActive(isLoading);
        if (isLoading)
        {
            SetTitleText("Loading...");
        }
    }

    void PreviewViewController::UpdatePreview(bool reinstantiate)
    {
        try
        {
            if (!currentPrefab || reinstantiate)
            {
                DEBUG("Reinstantiating prefab");
                ClearPrefab();
                InstantiatePrefab();
            }

            ShowLoading(false);
            if (!currentPrefab)
            {
                DEBUG("No prefab found, must be default!");
                SetTitleText("Default Cyoob (no preview)");
                return;
            }

            DEBUG("Getting variables");
            auto config = noteModelContainer->GetNoteConfig();
            auto& globalConfig = Config::get_config();

            auto& descriptor = noteModelContainer->GetDescriptor();
            auto name = descriptor.get_name();
            SetTitleText(name);

            float noteSizeFactor = globalConfig.get_overrideNoteSize() ? globalConfig.noteSize : 1.0f;
            float noteSize = noteSizeFactor * 0.4f;

            DEBUG("Cleaning up note mirrorables");
            auto notesT = currentPrefab->get_transform()->Find(ConstStrings::Notes());
            notesT->set_localPosition(Sombrero::FastVector3::zero());
            auto mirrorNotesT = currentPrefab->get_transform()->Find(ConstStrings::MirrorNotes());
            if (mirrorNotesT)
                UnityEngine::Object::DestroyImmediate(mirrorNotesT->get_gameObject());

            DEBUG("Cleaning up bomb mirrorables");
            auto bombT = currentPrefab->get_transform()->Find(ConstStrings::Bomb());
            auto mirrorBombT = currentPrefab->get_transform()->Find(ConstStrings::MirrorBomb());
            if (mirrorBombT)
                UnityEngine::Object::DestroyImmediate(mirrorBombT->get_gameObject());

            DEBUG("Cleaning up chain mirrorables");
            auto chainsT = currentPrefab->get_transform()->Find(ConstStrings::Chains());
            chainsT->set_localPosition(Sombrero::FastVector3::zero());
            auto mirrorChainT = currentPrefab->get_transform()->Find(ConstStrings::MirrorChains());
            if (mirrorChainT)
                UnityEngine::Object::DestroyImmediate(mirrorChainT->get_gameObject());

            DEBUG("Getting notes transforms");
            auto leftArrowT = notesT->Find(ConstStrings::LeftArrow());
            auto rightArrowT = notesT->Find(ConstStrings::RightArrow());
            auto leftDotT = notesT->Find(ConstStrings::LeftDot());
            auto rightDotT = notesT->Find(ConstStrings::RightDot());

            DEBUG("chains transforms");
            auto leftHeadT = chainsT->Find(ConstStrings::LeftHead());
            auto rightHeadT = chainsT->Find(ConstStrings::RightHead());
            auto leftLinkT = chainsT->Find(ConstStrings::LeftLink());
            auto rightLinkT = chainsT->Find(ConstStrings::RightLink());

            Sombrero::FastColor leftColor(1.0f, 0.0f, 0.0f, 1.0f);
            Sombrero::FastColor rightColor(0.0f, 0.0f, 1.0f, 1.0f);
            DEBUG("Attempting to get the selected color scheme");
            try
            {
                auto overrideColorScheme = playerDataModel->playerData->colorSchemesSettings->GetOverrideColorScheme();
                if (!overrideColorScheme)
                    playerDataModel->playerData->colorSchemesSettings->_colorSchemesDict->TryGetValue("TheFirst", ByRef<GlobalNamespace::ColorScheme*>(overrideColorScheme));

                if (overrideColorScheme)
                {
                    leftColor = overrideColorScheme->saberAColor;
                    rightColor = overrideColorScheme->saberBColor;
                }
            }
            catch (il2cpp_utils::RunMethodException& e)
            {
                ERROR("Failed to get colors from colorManager: {}", e.what());
            }

            float distance = 0.4f * (noteSizeFactor > 1.0f ? noteSizeFactor : 1.0f);
            float offset = 0.15f;
            float unit = distance + offset;

            DEBUG("Setting size & colors for normal notes");
            SetObjectSizePosColor<CyoobColorHandler*>(leftArrowT, noteSize, Sombrero::FastVector3(0.0f, unit, 0.0f), leftColor, rightColor);
            SetObjectSizePosColor<CyoobColorHandler*>(leftDotT, noteSize, Sombrero::FastVector3(0.0f, 0.0f, 0.0f), leftColor, rightColor);
            SetObjectSizePosColor<CyoobColorHandler*>(rightArrowT, noteSize, Sombrero::FastVector3(unit, unit, 0.0f), rightColor, leftColor);
            SetObjectSizePosColor<CyoobColorHandler*>(rightDotT, noteSize, Sombrero::FastVector3(unit, 0.0f, 0.0f), rightColor, leftColor);

            DEBUG("Setting size & colors for chain notes");
            SetObjectSizePosColor<CyoobColorHandler*>(leftHeadT, noteSize, Sombrero::FastVector3(-unit * 2, unit, 0.0f), leftColor, rightColor);
            SetObjectSizePosColor<CyoobColorHandler*>(leftLinkT, noteSize, Sombrero::FastVector3(-unit * 2, 0.0f, 0.0f), leftColor, rightColor);
            SetObjectSizePosColor<CyoobColorHandler*>(GetOrDuplicateLink(chainsT, leftLinkT, ConstStrings::LeftLinkCopy1()), noteSize, Sombrero::FastVector3(-unit * 2, distance / 3.0f, 0.0f), leftColor, rightColor);
            SetObjectSizePosColor<CyoobColorHandler*>(GetOrDuplicateLink(chainsT, leftLinkT, ConstStrings::LeftLinkCopy2()), noteSize, Sombrero::FastVector3(-unit * 2, -distance / 3.0f, 0.0f), leftColor, rightColor);

            SetObjectSizePosColor<CyoobColorHandler*>(rightHeadT, noteSize, Sombrero::FastVector3(-unit, unit, 0.0f), rightColor, leftColor);
            SetObjectSizePosColor<CyoobColorHandler*>(rightLinkT, noteSize, Sombrero::FastVector3(-unit, 0.0f, 0.0f), rightColor, leftColor);
            SetObjectSizePosColor<CyoobColorHandler*>(GetOrDuplicateLink(chainsT, rightLinkT, ConstStrings::RightLinkCopy1()), noteSize, Sombrero::FastVector3(-unit, distance / 3.0f, 0.0f), rightColor, leftColor);
            SetObjectSizePosColor<CyoobColorHandler*>(GetOrDuplicateLink(chainsT, rightLinkT, ConstStrings::RightLinkCopy2()), noteSize, Sombrero::FastVector3(-unit, -distance / 3.0f, 0.0f), rightColor, leftColor);

            chainsT->get_gameObject()->SetActive(!globalConfig.forceDefaultChains);

            if (bombT)
            {
                DEBUG("Setting size & colors for bombs");
                bombT->set_localPosition(Sombrero::FastVector3::zero());

                bombT->set_localScale(Sombrero::FastVector3::one() * noteSize);
                bombT->set_localPosition(Sombrero::FastVector3(unit * 2, 0.0f, 0.0f));

                bombT->get_gameObject()->SetActive(!globalConfig.forceDefaultBombs);
            }

            DEBUG("Getting lots of Debris transforms");
            auto debrisT = currentPrefab->get_transform()->Find(ConstStrings::Debris());
            auto chainHeadDebrisT = currentPrefab->get_transform()->Find(ConstStrings::ChainHeadDebris());
            auto chainLinkDebrisT = currentPrefab->get_transform()->Find(ConstStrings::ChainLinkDebris());

            if (debrisT)
            {
                DEBUG("Setting size & colors for regular debris");
                debrisT->set_localPosition(Sombrero::FastVector3::zero());

                auto leftDebrisT = debrisT->Find(ConstStrings::LeftDebris());
                auto rightDebrisT = debrisT->Find(ConstStrings::RightDebris());

                if (leftDebrisT)
                    SetObjectSizePosColor<DebrisColorHandler*>(leftDebrisT, noteSize, Sombrero::FastVector3(0.0f, -unit, 0.0f), leftColor, rightColor);

                if (rightDebrisT)
                    SetObjectSizePosColor<DebrisColorHandler*>(rightDebrisT, noteSize, Sombrero::FastVector3(unit, -unit, 0.0f), rightColor, leftColor);

                debrisT->get_gameObject()->SetActive(!globalConfig.forceDefaultDebris);
            }

            if (chainHeadDebrisT)
            {
                DEBUG("Setting size & colors for chain head debris");
                chainHeadDebrisT->set_localPosition(Sombrero::FastVector3::zero());

                auto leftDebrisT = chainHeadDebrisT->Find(ConstStrings::LeftDebris());
                auto rightDebrisT = chainHeadDebrisT->Find(ConstStrings::RightDebris());

                if (leftDebrisT)
                    SetObjectSizePosColor<DebrisColorHandler*>(leftDebrisT, noteSize, Sombrero::FastVector3(-unit * 2, -unit, 0.0f), leftColor, rightColor);

                if (rightDebrisT)
                    SetObjectSizePosColor<DebrisColorHandler*>(rightDebrisT, noteSize, Sombrero::FastVector3(-unit, -unit, 0.0f), rightColor, leftColor);

                chainHeadDebrisT->get_gameObject()->SetActive(!globalConfig.forceDefaultChainDebris && !globalConfig.forceDefaultChains);
            }

            if (chainLinkDebrisT)
            {
                DEBUG("Setting size & colors for chain link debris");
                chainLinkDebrisT->set_localPosition(Sombrero::FastVector3::zero());

                auto leftDebrisT = chainLinkDebrisT->Find(ConstStrings::LeftDebris());
                auto rightDebrisT = chainLinkDebrisT->Find(ConstStrings::RightDebris());

                if (leftDebrisT)
                    SetObjectSizePosColor<DebrisColorHandler*>(leftDebrisT, noteSize, Sombrero::FastVector3(-unit * 2, -unit * 2, 0.0f), leftColor, rightColor);

                if (rightDebrisT)
                    SetObjectSizePosColor<DebrisColorHandler*>(rightDebrisT, noteSize, Sombrero::FastVector3(-unit, -unit * 2, 0.0f), rightColor, leftColor);

                chainLinkDebrisT->get_gameObject()->SetActive(!globalConfig.forceDefaultChainDebris && !globalConfig.forceDefaultChains);
            }
        }
        catch (const il2cpp_utils::RunMethodException& e)
        {
            ERROR("{}", e.what());
        }
    }

    void PreviewViewController::InstantiatePrefab()
    {
        if (noteModelContainer->CurrentNoteObject)
        {
            DEBUG("Found a new note object, instantiating it! name: {}", noteModelContainer->CurrentNoteObject->get_name());
            currentPrefab = UnityEngine::Object::Instantiate(noteModelContainer->CurrentNoteObject, get_transform());
            currentPrefab->SetActive(true);
            auto t = currentPrefab->get_transform();
            t->set_localScale(Sombrero::FastVector3::one() * 30.0f);
            t->set_localPosition(Sombrero::FastVector3(-30.0f, 7.5f, -75.0f));
            t->set_localEulerAngles(Sombrero::FastVector3(0.0f, 60.0f, 0.0f));
            DEBUG("Instantiated and inited new prefab!");
        }
        else
        {
            ERROR("noteModelContainer->currentNoteObject was null but we tried using it!");
        }
    }

    void PreviewViewController::ClearPrefab()
    {
        DEBUG("Clearing Prefab...");
        if (currentPrefab)
        {
            DEBUG("Prefab existed!");
            UnityEngine::Object::DestroyImmediate(currentPrefab);
        }
        currentPrefab = nullptr;
    }
}
