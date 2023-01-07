#pragma once

#include "HMUI/FlowCoordinator.hpp"
#include "HMUI/ViewController.hpp"
#include "custom-types/shared/macros.hpp"
#include "lapiz/shared/macros.hpp"

#include "PreviewViewController.hpp"
#include "SelectionViewController.hpp"
#include "SettingsViewController.hpp"

#include "bsml/shared/BSML/Components/TextGradientUpdater.hpp"
#include "qosmetics-core/shared/UI/QosmeticsBaseFlowCoordinator.hpp"

#ifndef DECLARE_OVERRIDE_METHOD_MATCH
#define DECLARE_OVERRIDE_METHOD_MATCH(retval, name, mptr, ...) \
    DECLARE_OVERRIDE_METHOD(retval, name, il2cpp_utils::il2cpp_type_check::MetadataGetter<mptr>::get(), __VA_ARGS__)
#endif

#include "private_field.hpp"

DECLARE_CLASS_CUSTOM(Qosmetics::Notes, CyoobFlowCoordinator, Qosmetics::Core::QosmeticsBaseFlowCoordinator,
                     DECLARE_INSTANCE_PRIVATE_FIELD(HMUI::ViewController*, settingsViewController);
                     DECLARE_INSTANCE_PRIVATE_FIELD(HMUI::ViewController*, selectionViewController);
                     DECLARE_INSTANCE_PRIVATE_FIELD(HMUI::ViewController*, previewViewController);
                     DECLARE_INSTANCE_FIELD(BSML::TextGradientUpdater*, titleGradientUpdater);

                     DECLARE_INJECT_METHOD(void, Inject, PreviewViewController* previewViewController, SelectionViewController* selectionViewController, SettingsViewController* settingsViewController);

                     DECLARE_INSTANCE_METHOD(void, DidActivate, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling);
                     DECLARE_OVERRIDE_METHOD_MATCH(void, BackButtonWasPressed, &HMUI::FlowCoordinator::BackButtonWasPressed, HMUI::ViewController* topViewController);
                     DECLARE_OVERRIDE_METHOD_MATCH(void, DidDeactivate, &HMUI::FlowCoordinator::DidDeactivate, bool removedFromHierarchy, bool screenSystemDisabling);
                     DECLARE_CTOR(ctor);

)