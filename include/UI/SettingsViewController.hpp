#pragma once

#include "HMUI/ViewController.hpp"
#include "UI/PreviewViewController.hpp"
#include "custom-types/shared/macros.hpp"
#include "lapiz/shared/macros.hpp"

#include "bsml/shared/BSML/Components/Settings/SliderSetting.hpp"
#include "bsml/shared/BSML/Components/Settings/ToggleSetting.hpp"
#include "bsml/shared/macros.hpp"

#include "private_field.hpp"

DECLARE_CLASS_CODEGEN(Qosmetics::Notes, SettingsViewController, HMUI::ViewController,
                      DECLARE_INSTANCE_PRIVATE_FIELD(BSML::ToggleSetting*, overrideNoteSizeToggle);
                      DECLARE_INSTANCE_PRIVATE_FIELD(BSML::SliderSetting*, noteSizeSlider);
                      DECLARE_INSTANCE_PRIVATE_FIELD(BSML::ToggleSetting*, alsoChangeHitboxesToggle);
                      DECLARE_INSTANCE_PRIVATE_FIELD(BSML::ToggleSetting*, forceDefaultBombsToggle);
                      DECLARE_INSTANCE_PRIVATE_FIELD(BSML::ToggleSetting*, forceDefaultChainsToggle);
                      DECLARE_INSTANCE_PRIVATE_FIELD(BSML::ToggleSetting*, forceDefaultDebrisToggle);
                      DECLARE_INSTANCE_PRIVATE_FIELD(BSML::ToggleSetting*, forceDefaultChainDebrisToggle);
                      DECLARE_INSTANCE_PRIVATE_FIELD(BSML::ToggleSetting*, disableReflectionsToggle);
                      DECLARE_INSTANCE_PRIVATE_FIELD(BSML::ToggleSetting*, keepMissingReflectionsToggle);

                      DECLARE_INSTANCE_PRIVATE_FIELD(PreviewViewController*, previewViewController);

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
                      DECLARE_OVERRIDE_METHOD(void, DidActivate, il2cpp_utils::il2cpp_type_check::MetadataGetter<&HMUI::ViewController::DidActivate>::get(), bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling);
                      DECLARE_DEFAULT_CTOR();
                      public
                      :

                      static bool justChangedProfile;

)
