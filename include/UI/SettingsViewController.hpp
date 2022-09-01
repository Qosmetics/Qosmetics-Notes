#pragma once

#include "HMUI/ViewController.hpp"
#include "UI/PreviewViewController.hpp"
#include "UnityEngine/UI/Toggle.hpp"
#include "custom-types/shared/macros.hpp"
#include "lapiz/shared/macros.hpp"
#include "questui/shared/CustomTypes/Components/Settings/SliderSetting.hpp"

#ifndef DECLARE_OVERRIDE_METHOD_MATCH
#define DECLARE_OVERRIDE_METHOD_MATCH(retval, name, mptr, ...) \
    DECLARE_OVERRIDE_METHOD(retval, name, il2cpp_utils::il2cpp_type_check::MetadataGetter<mptr>::get(), __VA_ARGS__)
#endif

DECLARE_CLASS_CODEGEN(Qosmetics::Notes, SettingsViewController, HMUI::ViewController,
                      DECLARE_INSTANCE_FIELD(UnityEngine::UI::Toggle*, overrideNoteSizeToggle);
                      DECLARE_INSTANCE_FIELD(QuestUI::SliderSetting*, noteSizeSlider);
                      DECLARE_INSTANCE_FIELD(UnityEngine::UI::Toggle*, alsoChangeHitboxesToggle);
                      DECLARE_INSTANCE_FIELD(UnityEngine::UI::Toggle*, forceDefaultBombsToggle);
                      DECLARE_INSTANCE_FIELD(UnityEngine::UI::Toggle*, forceDefaultChainsToggle);
                      DECLARE_INSTANCE_FIELD(UnityEngine::UI::Toggle*, forceDefaultDebrisToggle);
                      DECLARE_INSTANCE_FIELD(UnityEngine::UI::Toggle*, forceDefaultChainDebrisToggle);
                      DECLARE_INSTANCE_FIELD(UnityEngine::UI::Toggle*, disableReflectionsToggle);
                      DECLARE_INSTANCE_FIELD(UnityEngine::UI::Toggle*, keepMissingReflectionsToggle);

                      DECLARE_INSTANCE_FIELD(PreviewViewController*, previewViewController);

                      DECLARE_INJECT_METHOD(void, Inject, PreviewViewController* previewViewController);
                      DECLARE_OVERRIDE_METHOD_MATCH(void, DidActivate, &HMUI::ViewController::DidActivate, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling);
                      DECLARE_DEFAULT_CTOR();
                      public
                      :

                      static bool justChangedProfile;

)
