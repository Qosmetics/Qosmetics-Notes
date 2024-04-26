#pragma once

#include "HMUI/ViewController.hpp"
#include "UI/PreviewViewController.hpp"
#include "custom-types/shared/macros.hpp"
#include "lapiz/shared/macros.hpp"

#include "bsml/shared/BSML/Components/Settings/SliderSetting.hpp"
#include "bsml/shared/BSML/Components/Settings/ToggleSetting.hpp"
#include "bsml/shared/macros.hpp"

DECLARE_CLASS_CODEGEN(Qosmetics::Notes, SettingsViewController, HMUI::ViewController,
                      DECLARE_INSTANCE_FIELD_PRIVATE(BSML::ToggleSetting*, overrideNoteSizeToggle);
                      DECLARE_INSTANCE_FIELD_PRIVATE(BSML::SliderSetting*, noteSizeSlider);
                      DECLARE_INSTANCE_FIELD_PRIVATE(BSML::ToggleSetting*, alsoChangeHitboxesToggle);
                      DECLARE_INSTANCE_FIELD_PRIVATE(BSML::ToggleSetting*, forceDefaultBombsToggle);
                      DECLARE_INSTANCE_FIELD_PRIVATE(BSML::ToggleSetting*, forceDefaultChainsToggle);
                      DECLARE_INSTANCE_FIELD_PRIVATE(BSML::ToggleSetting*, forceDefaultDebrisToggle);
                      DECLARE_INSTANCE_FIELD_PRIVATE(BSML::ToggleSetting*, forceDefaultChainDebrisToggle);
                      DECLARE_INSTANCE_FIELD_PRIVATE(BSML::ToggleSetting*, disableReflectionsToggle);
                      DECLARE_INSTANCE_FIELD_PRIVATE(BSML::ToggleSetting*, keepMissingReflectionsToggle);

                      DECLARE_INSTANCE_FIELD_PRIVATE(PreviewViewController*, previewViewController);

                      DECLARE_BSML_PROPERTY(bool, overrideNoteSize);
                      DECLARE_BSML_PROPERTY(float, noteSize);
                      DECLARE_BSML_PROPERTY(bool, alsoChangeHitboxes);
                      DECLARE_BSML_PROPERTY(bool, forceDefaultBombs);
                      DECLARE_BSML_PROPERTY(bool, forceDefaultChains);
                      DECLARE_BSML_PROPERTY(bool, forceDefaultDebris);
                      DECLARE_BSML_PROPERTY(bool, forceDefaultChainDebris);
                      DECLARE_BSML_PROPERTY(bool, disableReflections);
                      DECLARE_BSML_PROPERTY(bool, keepMissingReflections);

                      DECLARE_INJECT_METHOD(void, Inject, PreviewViewController* previewViewController);
                      DECLARE_INSTANCE_METHOD(StringW, get_gaydient);
                      DECLARE_INSTANCE_METHOD(bool, get_gay);
                      DECLARE_OVERRIDE_METHOD_MATCH(void, DidActivate, &HMUI::ViewController::DidActivate, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling);
                      DECLARE_DEFAULT_CTOR();
                      public
                      :

                      static bool justChangedProfile;

)
