#include "UI/SettingsViewController.hpp"
#include "config.hpp"
#include "diglett/shared/Diglett.hpp"
#include "qosmetics-core/shared/ConfigRegister.hpp"
#include "qosmetics-core/shared/Utils/UIUtils.hpp"
#include "questui/shared/BeatSaberUI.hpp"
#include "sombrero/shared/FastColor.hpp"

DEFINE_TYPE(Qosmetics::Notes, SettingsViewController);

using namespace QuestUI::BeatSaberUI;

#define TOGGLE(name, key)                                                                   \
    auto name##Toggle = CreateToggle(containerT, localization->Get(key), globalConfig.name, \
                                     [](auto v)                                             \
                                     {                                                      \
                                         Config::get_config().name = v;                     \
                                         Qosmetics::Core::Config::SaveConfig();             \
                                     });                                                    \
    AddHoverHint(name##Toggle, localization->Get(key "HoverHint"))

namespace Qosmetics::Notes
{
    void SettingsViewController::DidActivate(bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling)
    {
        if (firstActivation)
        {
            auto localization = Localization::GetSelected();
            Qosmetics::Core::UIUtils::AddHeader(get_transform(), localization->Get("QosmeticsCyoobs:Settings:Settings"), Sombrero::FastColor::blue());
            auto container = CreateScrollableSettingsContainer(this);
            auto containerT = container->get_transform();
            auto& globalConfig = Config::get_config();
            TOGGLE(overrideNoteSize, "QosmeticsCyoobs:Settings:OverrideNoteSize");
            auto noteSizeSlider = CreateSliderSetting(containerT, localization->Get("QosmeticsCyoobs:Settings:NoteSize"), 0.05f, globalConfig.noteSize, 0.05f, 5.0f, [](auto v)
                                                      {
                                                          Config::get_config().noteSize = v;
                                                          Qosmetics::Core::Config::SaveConfig(); });
            AddHoverHint(noteSizeSlider, localization->Get("QosmeticsCyoobs:Settings:NoteSizeHoverHint"));

            TOGGLE(alsoChangeHitboxes, "QosmeticsCyoobs:Settings:AlsoChangeHitboxes");
            TOGGLE(forceDefaultBombs, "QosmeticsCyoobs:Settings:ForceDefaultBombs");
            TOGGLE(forceDefaultDebris, "QosmeticsCyoobs:Settings:ForceDefaultDebris");
            TOGGLE(disableReflections, "QosmeticsCyoobs:Settings:DisableReflections");
            TOGGLE(keepMissingReflections, "QosmeticsCyoobs:Settings:KeepMissingReflections");
        }
    }
}