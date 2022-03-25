#include "UI/PreviewViewController.hpp"
#include "ConstStrings.hpp"
#include "CustomTypes/CyoobColorHandler.hpp"
#include "CustomTypes/DebrisColorHandler.hpp"
#include "CustomTypes/NoteModelContainer.hpp"
#include "config.hpp"
#include "diglett/shared/Localization.hpp"
#include "diglett/shared/Util.hpp"
#include "logging.hpp"
#include "qosmetics-core/shared/Utils/UIUtils.hpp"
#include "questui/shared/BeatSaberUI.hpp"
#include "questui/shared/CustomTypes/Components/Backgroundable.hpp"
#include "sombrero/shared/FastColor.hpp"
#include "sombrero/shared/FastVector3.hpp"

#include "UnityEngine/UI/LayoutElement.hpp"

#include "HMUI/CurvedCanvasSettingsHelper.hpp"
#include "HMUI/ImageView.hpp"

DEFINE_TYPE(Qosmetics::Notes, PreviewViewController);

using namespace QuestUI::BeatSaberUI;

namespace Qosmetics::Notes
{
    bool PreviewViewController::justChangedProfile = false;
    void PreviewViewController::DidDeactivate(bool removedFromHierarchy, bool screenSystemDisabling)
    {
        if (currentPrefab)
            currentPrefab->SetActive(false);
    }

    void PreviewViewController::DidActivate(bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling)
    {
        if (firstActivation)
        {
            title = Qosmetics::Core::UIUtils::AddHeader(get_transform(), "", Sombrero::FastColor::blue());

            auto backgroundLayout = CreateVerticalLayoutGroup(this);
            auto layoutElem = backgroundLayout->get_gameObject()->GetComponent<UnityEngine::UI::LayoutElement*>();
            layoutElem->set_preferredWidth(100.0f);

            auto bg = backgroundLayout->get_gameObject()->AddComponent<QuestUI::Backgroundable*>();
            bg->ApplyBackgroundWithAlpha("title-gradient", 1.0f);

            auto imageView = bg->get_gameObject()->GetComponent<HMUI::ImageView*>();
            imageView->set_gradient(true);
            imageView->dyn__gradientDirection() = 1;
            imageView->set_color(Sombrero::FastColor::white());
            auto color = Sombrero::FastColor::get_black();
            color.a = 0.3f;
            imageView->set_color0(color);
            color.a = 0.7f;
            imageView->set_color1(color);
            imageView->dyn__curvedCanvasSettingsHelper()->Reset();

            loadingIndicator = Qosmetics::Core::UIUtils::CreateLoadingIndicator(get_transform());
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
        title->set_text(u"<i>" + text + u"</i>");
    }

    void PreviewViewController::ShowLoading(bool isLoading)
    {
        loadingIndicator->SetActive(isLoading);
        if (isLoading)
        {
            SetTitleText(Diglett::Localization::get_instance()->get("QosmeticsCore:Misc:Loading") + u"...");
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
                SetTitleText(Diglett::Localization::get_instance()->get("QosmeticsCyoobs:Preview:Default"));
                return;
            }

            DEBUG("Getting variables");
            auto noteModelContainer = NoteModelContainer::get_instance();
            auto config = noteModelContainer->GetNoteConfig();
            auto& globalConfig = Config::get_config();

            auto& descriptor = noteModelContainer->GetDescriptor();
            auto name = descriptor.get_name();
            SetTitleText(name);

            float noteSizeFactor = globalConfig.overrideNoteSize ? globalConfig.noteSize : 1.0f;
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

            DEBUG("Getting lots of transforms");
            auto leftArrowT = notesT->Find(ConstStrings::LeftArrow());
            auto rightArrowT = notesT->Find(ConstStrings::RightArrow());
            auto leftDotT = notesT->Find(ConstStrings::LeftDot());
            auto rightDotT = notesT->Find(ConstStrings::RightDot());

            auto leftHeadT = chainsT->Find(ConstStrings::LeftHead());
            auto rightHeadT = chainsT->Find(ConstStrings::RightHead());
            auto leftLinkT = chainsT->Find(ConstStrings::LeftLink());
            auto rightLinkT = chainsT->Find(ConstStrings::RightLink());

            // TODO: fetch colors from proper place somewhere
            Sombrero::FastColor leftColor(1.0f, 0.0f, 0.0f, 1.0f);
            Sombrero::FastColor rightColor(0.0f, 0.0f, 1.0f, 1.0f);

            float distance = 0.4f * (noteSizeFactor > 1.0f ? noteSizeFactor : 1.0f);
            float offset = 0.15f;
            float unit = distance + offset;
            DEBUG("Setting size & colors for normal notes");
            leftArrowT->set_localScale(Sombrero::FastVector3::one() * noteSize);
            leftArrowT->set_localPosition(Sombrero::FastVector3(0.0f, unit, 0.0f));
            auto leftArrowColorHandler = leftArrowT->get_gameObject()->GetComponent<CyoobColorHandler*>();
            leftArrowColorHandler->FetchCCMaterials();
            leftArrowColorHandler->SetColors(leftColor, rightColor);

            rightArrowT->set_localScale(Sombrero::FastVector3::one() * noteSize);
            rightArrowT->set_localPosition(Sombrero::FastVector3(unit, unit, 0.0f));
            auto rightArrowColorHandler = rightArrowT->get_gameObject()->GetComponent<CyoobColorHandler*>();
            rightArrowColorHandler->FetchCCMaterials();
            rightArrowColorHandler->SetColors(rightColor, leftColor);

            leftDotT->set_localScale(Sombrero::FastVector3::one() * noteSize);
            leftDotT->set_localPosition(Sombrero::FastVector3(0.0f, 0.0f, 0.0f));
            auto leftDotColorHandler = leftDotT->get_gameObject()->GetComponent<CyoobColorHandler*>();
            leftDotColorHandler->FetchCCMaterials();
            leftDotColorHandler->SetColors(leftColor, rightColor);

            rightDotT->set_localScale(Sombrero::FastVector3::one() * noteSize);
            rightDotT->set_localPosition(Sombrero::FastVector3(unit, 0.0f, 0.0f));
            auto rightDotColorHandler = rightDotT->get_gameObject()->GetComponent<CyoobColorHandler*>();
            rightDotColorHandler->FetchCCMaterials();
            rightDotColorHandler->SetColors(rightColor, leftColor);

            DEBUG("Setting size & colors for chain notes");

            leftHeadT->set_localScale(Sombrero::FastVector3::one() * noteSize);
            leftHeadT->set_localPosition(Sombrero::FastVector3(-unit * 2, unit, 0.0f));
            auto leftHeadColorHandler = leftHeadT->get_gameObject()->GetComponent<CyoobColorHandler*>();
            leftHeadColorHandler->FetchCCMaterials();
            leftHeadColorHandler->SetColors(leftColor, rightColor);

            rightHeadT->set_localScale(Sombrero::FastVector3::one() * noteSize);
            rightHeadT->set_localPosition(Sombrero::FastVector3(-unit, unit, 0.0f));
            auto rightHeadColorHandler = rightHeadT->get_gameObject()->GetComponent<CyoobColorHandler*>();
            rightHeadColorHandler->FetchCCMaterials();
            rightHeadColorHandler->SetColors(rightColor, leftColor);

            // TODO duplicate links to make 3
            leftLinkT->set_localScale(Sombrero::FastVector3::one() * noteSize);
            leftLinkT->set_localPosition(Sombrero::FastVector3(-unit * 2, 0.0f, 0.0f));
            auto leftLinkColorHandler = leftLinkT->get_gameObject()->GetComponent<CyoobColorHandler*>();
            leftLinkColorHandler->FetchCCMaterials();
            leftLinkColorHandler->SetColors(leftColor, rightColor);

            auto leftLinkCopy1 = chainsT->Find(ConstStrings::LeftLinkCopy1());
            if (!leftLinkCopy1)
            {
                leftLinkCopy1 = Object::Instantiate(leftLinkT->get_gameObject(), chainsT)->get_transform();
                leftLinkCopy1->set_name(ConstStrings::LeftLinkCopy1());
            }

            leftLinkCopy1->set_localScale(Sombrero::FastVector3::one() * noteSize);
            leftLinkCopy1->set_localPosition(Sombrero::FastVector3(-unit * 2, distance / 3.0f, 0.0f));
            auto leftLinkCopy1ColorHandler = leftLinkCopy1->get_gameObject()->GetComponent<CyoobColorHandler*>();
            leftLinkCopy1ColorHandler->FetchCCMaterials();
            leftLinkCopy1ColorHandler->SetColors(leftColor, rightColor);

            auto leftLinkCopy2 = chainsT->Find(ConstStrings::LeftLinkCopy2());
            if (!leftLinkCopy2)
            {
                leftLinkCopy2 = Object::Instantiate(leftLinkT->get_gameObject(), chainsT)->get_transform();
                leftLinkCopy2->set_name(ConstStrings::LeftLinkCopy2());
            }

            leftLinkCopy2->set_localScale(Sombrero::FastVector3::one() * noteSize);
            leftLinkCopy2->set_localPosition(Sombrero::FastVector3(-unit * 2, -distance / 3.0f, 0.0f));
            auto leftLinkCopy2ColorHandler = leftLinkCopy2->get_gameObject()->GetComponent<CyoobColorHandler*>();
            leftLinkCopy2ColorHandler->FetchCCMaterials();
            leftLinkCopy2ColorHandler->SetColors(leftColor, rightColor);

            rightLinkT->set_localScale(Sombrero::FastVector3::one() * noteSize);
            rightLinkT->set_localPosition(Sombrero::FastVector3(-unit, 0.0f, 0.0f));
            auto rightLinkColorHandler = rightLinkT->get_gameObject()->GetComponent<CyoobColorHandler*>();
            rightLinkColorHandler->FetchCCMaterials();
            rightLinkColorHandler->SetColors(rightColor, leftColor);

            auto rightLinkCopy1 = chainsT->Find(ConstStrings::RightLinkCopy1());
            if (!rightLinkCopy1)
            {
                rightLinkCopy1 = Object::Instantiate(rightLinkT->get_gameObject(), chainsT)->get_transform();
                rightLinkCopy1->set_name(ConstStrings::RightLinkCopy1());
            }

            rightLinkCopy1->set_localScale(Sombrero::FastVector3::one() * noteSize);
            rightLinkCopy1->set_localPosition(Sombrero::FastVector3(-unit, distance / 3.0f, 0.0f));
            auto rightLinkCopy1ColorHandler = rightLinkCopy1->get_gameObject()->GetComponent<CyoobColorHandler*>();
            rightLinkCopy1ColorHandler->FetchCCMaterials();
            rightLinkCopy1ColorHandler->SetColors(rightColor, leftColor);

            auto rightLinkCopy2 = chainsT->Find(ConstStrings::RightLinkCopy2());
            if (!rightLinkCopy2)
            {
                rightLinkCopy2 = Object::Instantiate(rightLinkT->get_gameObject(), chainsT)->get_transform();
                rightLinkCopy2->set_name(ConstStrings::RightLinkCopy2());
            }

            rightLinkCopy2->set_localScale(Sombrero::FastVector3::one() * noteSize);
            rightLinkCopy2->set_localPosition(Sombrero::FastVector3(-unit, -distance / 3.0f, 0.0f));
            auto rightLinkCopy2ColorHandler = rightLinkCopy2->get_gameObject()->GetComponent<CyoobColorHandler*>();
            rightLinkCopy2ColorHandler->FetchCCMaterials();
            rightLinkCopy2ColorHandler->SetColors(rightColor, leftColor);

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
                {
                    leftDebrisT->set_localScale(Sombrero::FastVector3::one() * noteSize);
                    leftDebrisT->set_localPosition(Sombrero::FastVector3(0.0f, -unit, 0.0f));
                    auto leftDebrisColorHandler = leftDebrisT->get_gameObject()->GetComponent<DebrisColorHandler*>();
                    leftDebrisColorHandler->FetchCCMaterials();
                    leftDebrisColorHandler->SetColors(leftColor, rightColor);
                }

                if (rightDebrisT)
                {
                    rightDebrisT->set_localScale(Sombrero::FastVector3::one() * noteSize);
                    rightDebrisT->set_localPosition(Sombrero::FastVector3(0.0f, -unit, 0.0f));
                    auto rightDebrisColorHandler = rightDebrisT->get_gameObject()->GetComponent<DebrisColorHandler*>();
                    rightDebrisColorHandler->FetchCCMaterials();
                    rightDebrisColorHandler->SetColors(rightColor, leftColor);
                }

                debrisT->get_gameObject()->SetActive(!globalConfig.forceDefaultDebris);
            }

            if (chainHeadDebrisT)
            {
                DEBUG("Setting size & colors for chain head debris");
                chainHeadDebrisT->set_localPosition(Sombrero::FastVector3::zero());

                auto leftDebrisT = chainHeadDebrisT->Find(ConstStrings::LeftDebris());
                auto rightDebrisT = chainHeadDebrisT->Find(ConstStrings::RightDebris());

                if (leftDebrisT)
                {
                    leftDebrisT->set_localScale(Sombrero::FastVector3::one() * noteSize);
                    leftDebrisT->set_localPosition(Sombrero::FastVector3(-unit * 2, -unit, 0.0f));
                    auto leftDebrisColorHandler = leftDebrisT->get_gameObject()->GetComponent<DebrisColorHandler*>();
                    leftDebrisColorHandler->FetchCCMaterials();
                    leftDebrisColorHandler->SetColors(leftColor, rightColor);
                }

                if (rightDebrisT)
                {
                    rightDebrisT->set_localScale(Sombrero::FastVector3::one() * noteSize);
                    rightDebrisT->set_localPosition(Sombrero::FastVector3(-unit, -unit, 0.0f));
                    auto rightDebrisColorHandler = rightDebrisT->get_gameObject()->GetComponent<DebrisColorHandler*>();
                    rightDebrisColorHandler->FetchCCMaterials();
                    rightDebrisColorHandler->SetColors(rightColor, leftColor);
                }

                chainHeadDebrisT->get_gameObject()->SetActive(!globalConfig.forceDefaultChainDebris);
            }

            if (chainLinkDebrisT)
            {
                DEBUG("Setting size & colors for chain link debris");
                chainLinkDebrisT->set_localPosition(Sombrero::FastVector3::zero());

                auto leftDebrisT = chainLinkDebrisT->Find(ConstStrings::LeftDebris());
                auto rightDebrisT = chainLinkDebrisT->Find(ConstStrings::RightDebris());

                if (leftDebrisT)
                {
                    leftDebrisT->set_localScale(Sombrero::FastVector3::one() * noteSize);
                    leftDebrisT->set_localPosition(Sombrero::FastVector3(-unit * 2, -unit * 2, 0.0f));
                    auto leftDebrisColorHandler = leftDebrisT->get_gameObject()->GetComponent<DebrisColorHandler*>();
                    leftDebrisColorHandler->FetchCCMaterials();
                    leftDebrisColorHandler->SetColors(leftColor, rightColor);
                }

                if (rightDebrisT)
                {
                    rightDebrisT->set_localScale(Sombrero::FastVector3::one() * noteSize);
                    rightDebrisT->set_localPosition(Sombrero::FastVector3(-unit, -unit * 2, 0.0f));
                    auto rightDebrisColorHandler = rightDebrisT->get_gameObject()->GetComponent<DebrisColorHandler*>();
                    rightDebrisColorHandler->FetchCCMaterials();
                    rightDebrisColorHandler->SetColors(rightColor, leftColor);
                }

                chainLinkDebrisT->get_gameObject()->SetActive(!globalConfig.forceDefaultChainDebris);
            }
        }
        catch (const il2cpp_utils::RunMethodException& e)
        {
            ERROR("{}", e.what());
        }
    }

    void PreviewViewController::InstantiatePrefab()
    {
        auto noteModelContainer = NoteModelContainer::get_instance();
        if (noteModelContainer->currentNoteObject)
        {
            DEBUG("Found a new note object, instantiating it! name: {}", noteModelContainer->currentNoteObject->get_name());
            currentPrefab = UnityEngine::Object::Instantiate(noteModelContainer->currentNoteObject, get_transform());
            currentPrefab->SetActive(true);
            auto t = currentPrefab->get_transform();
            t->set_localScale(Sombrero::FastVector3::one() * 30.0f);
            t->set_localPosition(Sombrero::FastVector3(-30.0f, 0.0f, -75.0f));
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