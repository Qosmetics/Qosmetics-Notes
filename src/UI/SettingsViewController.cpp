#include "UI/SettingsViewController.hpp"
#include "config.hpp"
#include "diglett/shared/Localization.hpp"
#include "diglett/shared/Util.hpp"
#include "logging.hpp"
#include "qosmetics-core/shared/ConfigRegister.hpp"
#include "qosmetics-core/shared/Utils/DateUtils.hpp"
#include "qosmetics-core/shared/Utils/RainbowUtils.hpp"
#include "qosmetics-core/shared/Utils/UIUtils.hpp"
#include "questui/shared/BeatSaberUI.hpp"
#include "questui/shared/CustomTypes/Components/ExternalComponents.hpp"
#include "sombrero/shared/FastColor.hpp"
#include "sombrero/shared/FastVector2.hpp"

DEFINE_TYPE(Qosmetics::Notes, SettingsViewController);

using namespace QuestUI::BeatSaberUI;

#define TOGGLE(name, key)                                                              \
    name##Toggle = CreateToggle(containerT, localization->get(key), globalConfig.name, \
                                [&](auto v)                                            \
                                {                                                      \
                                    Config::get_config().name = v;                     \
                                    Qosmetics::Core::Config::SaveConfig();             \
                                    previewViewController->UpdatePreview(false);       \
                                });                                                    \
    AddHoverHint(name##Toggle, localization->get(key "HoverHint"))

namespace Qosmetics::Notes
{
    bool SettingsViewController::justChangedProfile = false;

    void SettingsViewController::Inject(PreviewViewController* previewViewController)
    {
        this->previewViewController = previewViewController;
    }

    void SettingsViewController::DidActivate(bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling)
    {
        auto& globalConfig = Config::get_config();
        if (firstActivation)
        {
            auto localization = Diglett::Localization::get_instance();
            if (Qosmetics::Core::DateUtils::isMonth(6))
            {
                Qosmetics::Core::UIUtils::AddHeader(get_transform(), Qosmetics::Core::RainbowUtils::gayify(static_cast<std::string>(localization->get("QosmeticsCyoobs:Settings:Settings"))), Sombrero::FastColor::blue());
            }
            else
                Qosmetics::Core::UIUtils::AddHeader(get_transform(), localization->get("QosmeticsCyoobs:Settings:Settings"), Sombrero::FastColor::blue());
            auto container = CreateScrollableSettingsContainer(this);

            auto externalComponents = container->GetComponent<QuestUI::ExternalComponents*>();
            auto scrollTransform = externalComponents->Get<UnityEngine::RectTransform*>();
            scrollTransform->set_sizeDelta(Sombrero::FastVector2::zero());

            auto containerT = container->get_transform();
            overrideNoteSizeToggle = CreateToggle(containerT, localization->get("QosmeticsCyoobs:Settings:OverrideNoteSize"), globalConfig.get_overrideNoteSize(),
                                                  [&](auto v)
                                                  {
                                                      auto& config = Config::get_config();
                                                      config.set_overrideNoteSize(v);

                                                      Qosmetics::Core::Config::SaveConfig();
                                                      previewViewController->UpdatePreview(false);
                                                  });
            AddHoverHint(overrideNoteSizeToggle, localization->get("QosmeticsCyoobs:Settings:OverrideNoteSizeHoverHint"));

            noteSizeSlider = CreateSliderSetting(containerT, localization->get("QosmeticsCyoobs:Settings:NoteSize"), 0.05f, globalConfig.noteSize, 0.05f, 2.0f, 0.2f,
                                                 [&](auto v)
                                                 {
                                                     Config::get_config().noteSize = v;
                                                     Qosmetics::Core::Config::SaveConfig();
                                                     previewViewController->UpdatePreview(false);
                                                 });

            noteSizeSlider->FormatString = [](auto v) -> std::string
            {
                return std::to_string(v).substr(0, 4);
            };

            AddHoverHint(noteSizeSlider, localization->get("QosmeticsCyoobs:Settings:NoteSizeHoverHint"));

            alsoChangeHitboxesToggle = CreateToggle(containerT, localization->get("QosmeticsCyoobs:Settings:AlsoChangeHitboxes"), globalConfig.get_alsoChangeHitboxes(),
                                                    [&](auto v)
                                                    {
                                                        auto& config = Config::get_config();
                                                        config.set_alsoChangeHitboxes(v);
                                                        Qosmetics::Core::Config::SaveConfig();
                                                        previewViewController->UpdatePreview(false);
                                                    });
            AddHoverHint(alsoChangeHitboxesToggle, localization->get("QosmeticsCyoobs:Settings:AlsoChangeHitboxesHint"));

            TOGGLE(forceDefaultBombs, "QosmeticsCyoobs:Settings:ForceDefaultBombs");
            TOGGLE(forceDefaultChains, "QosmeticsCyoobs:Settings:ForceDefaultChains");
            TOGGLE(forceDefaultDebris, "QosmeticsCyoobs:Settings:ForceDefaultDebris");
            TOGGLE(forceDefaultChainDebris, "QosmeticsCyoobs:Settings:ForceDefaultChainDebris");
            TOGGLE(disableReflections, "QosmeticsCyoobs:Settings:DisableReflections");
            TOGGLE(keepMissingReflections, "QosmeticsCyoobs:Settings:KeepMissingReflections");
        }
        else if (justChangedProfile)
        {
            justChangedProfile = false;
            overrideNoteSizeToggle->set_isOn(globalConfig.get_overrideNoteSize());
            noteSizeSlider->set_value(globalConfig.noteSize);
            alsoChangeHitboxesToggle->set_isOn(globalConfig.get_alsoChangeHitboxes());
            forceDefaultChainsToggle->set_isOn(globalConfig.forceDefaultChains);
            forceDefaultBombsToggle->set_isOn(globalConfig.forceDefaultBombs);
            forceDefaultDebrisToggle->set_isOn(globalConfig.forceDefaultDebris);
            forceDefaultChainDebrisToggle->set_isOn(globalConfig.forceDefaultChainDebris);
            disableReflectionsToggle->set_isOn(globalConfig.disableReflections);
            keepMissingReflectionsToggle->set_isOn(globalConfig.keepMissingReflections);
        }
    }
}