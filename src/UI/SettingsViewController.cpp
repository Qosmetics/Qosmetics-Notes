#include "UI/SettingsViewController.hpp"
#include "questui/shared/BeatSaberUI.hpp"

DEFINE_TYPE(Qosmetics::Notes, SettingsViewController);

using namespace QuestUI::BeatSaberUI;

namespace Qosmetics::Notes
{
    void SettingsViewController::DidActivate(bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling)
    {
        if (firstActivation)
        {
            CreateText(get_transform(), "SettingsViewController");
        }
    }
}