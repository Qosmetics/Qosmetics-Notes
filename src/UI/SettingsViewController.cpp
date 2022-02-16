#include "UI/SettingsViewController.hpp"
#include "config.hpp"
#include "diglett/shared/Diglett.hpp"
#include "qosmetics-core/shared/ConfigRegister.hpp"
#include "qosmetics-core/shared/Utils/UIUtils.hpp"
#include "questui/shared/BeatSaberUI.hpp"
#include "questui/shared/CustomTypes/Components/ExternalComponents.hpp"
#include "sombrero/shared/FastColor.hpp"
#include "sombrero/shared/FastVector2.hpp"

#include "UnityEngine/RectTransform.hpp"

DEFINE_TYPE(Qosmetics::Notes, SettingsViewController);

using namespace QuestUI::BeatSaberUI;

#define TOGGLE(name, key)                                                              \
    name##Toggle = CreateToggle(containerT, localization->Get(key), globalConfig.name, \
                                [&](auto v)                                            \
                                {                                                      \
                                    Config::get_config().name = v;                     \
                                    Qosmetics::Core::Config::SaveConfig();             \
                                    previewViewController->UpdatePreview(false);       \
                                });                                                    \
    AddHoverHint(name##Toggle, localization->Get(key "HoverHint"))

namespace Qosmetics::Notes
{
    bool SettingsViewController::justChangedProfile = false;

    void SettingsViewController::DidActivate(bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling)
    {
        auto& globalConfig = Config::get_config();
        if (firstActivation)
        {
            auto localization = Localization::GetSelected();
            Qosmetics::Core::UIUtils::AddHeader(get_transform(), localization->Get("QosmeticsCyoobs:Settings:Settings"), Sombrero::FastColor::blue());
            auto container = CreateScrollableSettingsContainer(this);

            auto externalComponents = container->GetComponent<QuestUI::ExternalComponents*>();
            auto scrollTransform = externalComponents->Get<UnityEngine::RectTransform*>();
            scrollTransform->set_sizeDelta(Sombrero::FastVector2::zero());

            auto containerT = container->get_transform();
            TOGGLE(overrideNoteSize, "QosmeticsCyoobs:Settings:OverrideNoteSize");
            noteSizeSlider = CreateSliderSetting(containerT, localization->Get("QosmeticsCyoobs:Settings:NoteSize"), 0.05f, globalConfig.noteSize, 0.05f, 2.0f, 0.2f, [&](auto v)
                                                 {
                                                          Config::get_config().noteSize = v;
                                                          Qosmetics::Core::Config::SaveConfig();
                                                          previewViewController->UpdatePreview(false); });
            noteSizeSlider->FormatString = [](auto v) -> std::string
            {
                return std::to_string(v).substr(0, 4);
            };
            AddHoverHint(noteSizeSlider, localization->Get("QosmeticsCyoobs:Settings:NoteSizeHoverHint"));

            TOGGLE(alsoChangeHitboxes, "QosmeticsCyoobs:Settings:AlsoChangeHitboxes");
            TOGGLE(forceDefaultBombs, "QosmeticsCyoobs:Settings:ForceDefaultBombs");
            TOGGLE(forceDefaultDebris, "QosmeticsCyoobs:Settings:ForceDefaultDebris");
            TOGGLE(disableReflections, "QosmeticsCyoobs:Settings:DisableReflections");
            TOGGLE(keepMissingReflections, "QosmeticsCyoobs:Settings:KeepMissingReflections");
        }
        else if (justChangedProfile)
        {
            justChangedProfile = false;
            overrideNoteSizeToggle->set_isOn(globalConfig.overrideNoteSize);
            noteSizeSlider->set_value(globalConfig.noteSize);
            alsoChangeHitboxesToggle->set_isOn(globalConfig.alsoChangeHitboxes);
            forceDefaultBombsToggle->set_isOn(globalConfig.forceDefaultBombs);
            forceDefaultDebrisToggle->set_isOn(globalConfig.forceDefaultDebris);
            disableReflectionsToggle->set_isOn(globalConfig.disableReflections);
            keepMissingReflectionsToggle->set_isOn(globalConfig.keepMissingReflections);
        }
    }
}