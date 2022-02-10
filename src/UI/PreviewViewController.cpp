#include "UI/PreviewViewController.hpp"
#include "ConstStrings.hpp"
#include "CustomTypes/NoteModelContainer.hpp"
#include "config.hpp"
#include "diglett/shared/Diglett.hpp"
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
    void PreviewViewController::DidActivate(bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling)
    {
        if (firstActivation)
        {
            auto localization = Localization::GetSelected();
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
    }

    void PreviewViewController::SetTitleText(StringW text)
    {
        title->set_text(u"<i>" + text + u"</i>");
    }
    void PreviewViewController::ShowLoading(bool isLoading)
    {
        auto localization = Localization::GetSelected();
        loadingIndicator->SetActive(isLoading);
        if (isLoading)
        {
            SetTitleText(localization->Get("QosmeticsCore:Misc:Loading") + u"...");
        }
    }

    void PreviewViewController::UpdatePreview(bool reinstantiate)
    {
        if (!currentPrefab || reinstantiate)
        {
            ClearPrefab();
            InstantiatePrefab();
        }

        if (!currentPrefab)
        {
            SetTitleText(Localization::GetSelected()->Get("QosmeticsCyoobs:Preview:Default"));
            return;
        }

        ShowLoading(false);

        auto noteModelContainer = NoteModelContainer::get_instance();
        auto config = noteModelContainer->GetNoteConfig();
        auto& globalConfig = Config::get_config();

        auto& descriptor = noteModelContainer->GetDescriptor();
        auto name = descriptor.get_name();
        SetTitleText(name);

        float noteSizeFactor = globalConfig.overrideNoteSize ? globalConfig.noteSize : 1.0f;
        float noteSize = noteSizeFactor * 0.4f;
        auto notesT = currentPrefab->get_transform()->Find(ConstStrings::Notes());
        auto mirrorNotesT = currentPrefab->get_transform()->Find(ConstStrings::MirrorNotes());
        if (mirrorNotesT)
            UnityEngine::Object::DestroyImmediate(mirrorNotesT->get_gameObject());

        // TODO: Do some funky size and position offsets if the notes are scaled a certain way
        auto bombT = currentPrefab->get_transform()->Find(ConstStrings::Bomb());
        auto mirrorBombT = currentPrefab->get_transform()->Find(ConstStrings::MirrorBomb());
        if (mirrorBombT)
            UnityEngine::Object::DestroyImmediate(mirrorBombT->get_gameObject());

        auto debrisT = currentPrefab->get_transform()->Find(ConstStrings::Debris());

        auto leftArrowT = notesT->Find(ConstStrings::LeftArrow());
        auto rightArrowT = notesT->Find(ConstStrings::RightArrow());
        auto leftDotT = notesT->Find(ConstStrings::LeftDot());
        auto rightDotT = notesT->Find(ConstStrings::RightDot());

        leftArrowT->set_localScale(Sombrero::FastVector3::one() * noteSize);
        leftArrowT->set_localPosition(Sombrero::FastVector3(0.5f, 1.0f, 0.0f));

        rightArrowT->set_localScale(Sombrero::FastVector3::one() * noteSize);
        rightArrowT->set_localPosition(Sombrero::FastVector3(-0.5f, 1.0f, 0.0f));

        leftDotT->set_localScale(Sombrero::FastVector3::one() * noteSize);
        leftDotT->set_localPosition(Sombrero::FastVector3(0.5f, 0.0f, 0.0f));

        rightDotT->set_localScale(Sombrero::FastVector3::one() * noteSize);
        rightDotT->set_localPosition(Sombrero::FastVector3(-0.5f, 0.0f, 0.0f));

        if (bombT)
        {
            bombT->set_localScale(Sombrero::FastVector3::one() * noteSize);
            bombT->set_localPosition(Sombrero::FastVector3(1.0f, 0.0f, 0.0f));

            bombT->get_gameObject()->SetActive(!globalConfig.forceDefaultBombs);
        }

        if (debrisT)
        {
            auto leftDebrisT = debrisT->Find(ConstStrings::LeftDebris());
            auto rightDebrisT = debrisT->Find(ConstStrings::RightDebris());

            if (leftDebrisT)
            {
                leftDebrisT->set_localScale(Sombrero::FastVector3::one() * noteSize);
                leftDebrisT->set_localPosition(Sombrero::FastVector3(0.5f, -1.0f, 0.0f));
            }

            if (rightDebrisT)
            {
                rightDebrisT->set_localScale(Sombrero::FastVector3::one() * noteSize);
                rightDebrisT->set_localPosition(Sombrero::FastVector3(-0.5f, -1.0f, 0.0f));
            }

            debrisT->get_gameObject()->SetActive(!globalConfig.forceDefaultDebris);
        }
    }

    void PreviewViewController::InstantiatePrefab()
    {
        auto noteModelContainer = NoteModelContainer::get_instance();
        if (noteModelContainer->currentNoteObject)
        {
            // TODO: improve default values as they are not 100% right
            currentPrefab = UnityEngine::Object::Instantiate(noteModelContainer->currentNoteObject, get_transform());
            currentPrefab->SetActive(true);
            auto t = currentPrefab->get_transform();
            t->set_localScale(Sombrero::FastVector3::one() * 12.5f);
            t->set_localPosition(Sombrero::FastVector3(-30.0f, 0.0f, -75.0f));
            t->set_localEulerAngles(Sombrero::FastVector3(0.0f, 60.0f, 0.0f));
        }
        else
        {
            ERROR("noteModelContainer->currentNoteObject was null but we tried using it!");
        }
    }

    void PreviewViewController::ClearPrefab()
    {
        if (currentPrefab)
            UnityEngine::Object::DestroyImmediate(currentPrefab);
        currentPrefab = nullptr;
    }

}