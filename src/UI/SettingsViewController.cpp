#include "UI/SettingsViewController.hpp"
#include "assets.hpp"
#include "config.hpp"
#include "logging.hpp"

#include "qosmetics-core/shared/ConfigRegister.hpp"
#include "qosmetics-core/shared/Utils/DateUtils.hpp"
#include "qosmetics-core/shared/Utils/RainbowUtils.hpp"

#include "sombrero/shared/FastColor.hpp"
#include "sombrero/shared/FastVector2.hpp"

#include "bsml/shared/BSML.hpp"

DEFINE_TYPE(Qosmetics::Notes, SettingsViewController);

namespace Qosmetics::Notes
{
    bool SettingsViewController::justChangedProfile = false;

    void SettingsViewController::Inject(PreviewViewController* previewViewController)
    {
        this->previewViewController = previewViewController;
    }

    StringW SettingsViewController::get_gaydient() { return Qosmetics::Core::RainbowUtils::randomGradient(); }
    bool SettingsViewController::get_gay() { return Qosmetics::Core::DateUtils::isMonth(6); }

    void SettingsViewController::DidActivate(bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling)
    {
        auto& globalConfig = Config::get_config();
        if (firstActivation)
            BSML::parse_and_construct(IncludedAssets::SettingsView_bsml, get_transform(), this);
        else if (justChangedProfile)
        {
            justChangedProfile = false;

            auto settings = GetComponentsInChildren<BSML::BaseSetting*>();
            for (auto setting : settings)
            {
                il2cpp_utils::RunMethod(setting, "ReceiveValue");
            }
        }
    }

    bool SettingsViewController::get_overrideNoteSize() { return Config::get_config().get_overrideNoteSize(); }
    void SettingsViewController::set_overrideNoteSize(bool value)
    {
        Config::get_config().set_overrideNoteSize(value);
        Qosmetics::Core::Config::SaveConfig();
        previewViewController->UpdatePreview(false);
    }

    float SettingsViewController::get_noteSize() { return Config::get_config().noteSize; }
    void SettingsViewController::set_noteSize(float value)
    {
        Config::get_config().noteSize = value;
        Qosmetics::Core::Config::SaveConfig();
        previewViewController->UpdatePreview(false);
    }

    bool SettingsViewController::get_alsoChangeHitboxes() { return Config::get_config().get_alsoChangeHitboxes(); }
    void SettingsViewController::set_alsoChangeHitboxes(bool value)
    {
        Config::get_config().set_alsoChangeHitboxes(value);
        Qosmetics::Core::Config::SaveConfig();
        previewViewController->UpdatePreview(false);
    }

    bool SettingsViewController::get_forceDefaultBombs() { return Config::get_config().forceDefaultBombs; }
    void SettingsViewController::set_forceDefaultBombs(bool value)
    {
        Config::get_config().forceDefaultBombs = value;
        Qosmetics::Core::Config::SaveConfig();
        previewViewController->UpdatePreview(false);
    }

    bool SettingsViewController::get_forceDefaultChains() { return Config::get_config().forceDefaultChains; }
    void SettingsViewController::set_forceDefaultChains(bool value)
    {
        Config::get_config().forceDefaultChains = value;
        Qosmetics::Core::Config::SaveConfig();
        previewViewController->UpdatePreview(false);
    }

    bool SettingsViewController::get_forceDefaultDebris() { return Config::get_config().forceDefaultDebris; }
    void SettingsViewController::set_forceDefaultDebris(bool value)
    {
        Config::get_config().forceDefaultDebris = value;
        Qosmetics::Core::Config::SaveConfig();
        previewViewController->UpdatePreview(false);
    }

    bool SettingsViewController::get_forceDefaultChainDebris() { return Config::get_config().forceDefaultChainDebris; }
    void SettingsViewController::set_forceDefaultChainDebris(bool value)
    {
        Config::get_config().forceDefaultChainDebris = value;
        Qosmetics::Core::Config::SaveConfig();
        previewViewController->UpdatePreview(false);
    }

    bool SettingsViewController::get_disableReflections() { return Config::get_config().disableReflections; }
    void SettingsViewController::set_disableReflections(bool value)
    {
        Config::get_config().disableReflections = value;
        Qosmetics::Core::Config::SaveConfig();
        previewViewController->UpdatePreview(false);
    }

    bool SettingsViewController::get_keepMissingReflections() { return Config::get_config().keepMissingReflections; }
    void SettingsViewController::set_keepMissingReflections(bool value)
    {
        Config::get_config().keepMissingReflections = value;
        Qosmetics::Core::Config::SaveConfig();
        previewViewController->UpdatePreview(false);
    }

}