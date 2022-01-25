#include "UI/PreviewViewController.hpp"
#include "questui/shared/BeatSaberUI.hpp"

DEFINE_TYPE(Qosmetics::Notes, PreviewViewController);

using namespace QuestUI::BeatSaberUI;

namespace Qosmetics::Notes
{
    void PreviewViewController::DidActivate(bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling)
    {
        if (firstActivation)
        {
            CreateText(get_transform(), "PreviewViewController");
        }
    }
}