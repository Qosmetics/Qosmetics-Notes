#include "UI/CyoobFlowCoordinator.hpp"
#include "UI/PreviewViewController.hpp"
#include "UI/SelectionViewController.hpp"
#include "UI/SettingsViewController.hpp"
#include "questui/shared/BeatSaberUI.hpp"

#include "HMUI/TitleViewController.hpp"
#include "HMUI/ViewController_AnimationDirection.hpp"
#include "HMUI/ViewController_AnimationType.hpp"

#include "qosmetics-core/shared/Utils/UIUtils.hpp"

DEFINE_TYPE(Qosmetics::Notes, CyoobFlowCoordinator)

using namespace QuestUI::BeatSaberUI;

namespace Qosmetics::Notes
{
    void CyoobFlowCoordinator::DidActivate(bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling)
    {
        if (firstActivation)
        {
            settingsViewController = CreateViewController<Qosmetics::Notes::SettingsViewController*>();
            selectionViewController = CreateViewController<Qosmetics::Notes::SelectionViewController*>();
            previewViewController = CreateViewController<Qosmetics::Notes::PreviewViewController*>();
            ProvideInitialViewControllers(selectionViewController, settingsViewController, previewViewController, nullptr, nullptr);

            set_showBackButton(true);
            SetTitle(il2cpp_utils::newcsstr("Cyoobs"), HMUI::ViewController::AnimationType::In);
        }

        HMUI::TitleViewController* titleView = Object::FindObjectOfType<HMUI::TitleViewController*>();
        Qosmetics::Core::UIUtils::SetTitleColor(titleView, UnityEngine::Color::get_blue());
    }

    void CyoobFlowCoordinator::BackButtonWasPressed(HMUI::ViewController* topViewController)
    {
        this->parentFlowCoordinator->DismissFlowCoordinator(this, HMUI::ViewController::AnimationDirection::Horizontal, nullptr, false);
    }

}