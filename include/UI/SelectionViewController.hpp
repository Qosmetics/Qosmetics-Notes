#pragma once

#include "HMUI/ViewController.hpp"
#include "custom-types/shared/macros.hpp"
#include "qosmetics-core/shared/UI/Components/DeletionConfirmationModal.hpp"
#include "qosmetics-core/shared/UI/Components/QosmeticObjectTableData.hpp"

#ifndef DECLARE_OVERRIDE_METHOD_MATCH
#define DECLARE_OVERRIDE_METHOD_MATCH(retval, name, mptr, ...) \
    DECLARE_OVERRIDE_METHOD(retval, name, il2cpp_utils::il2cpp_type_check::MetadataGetter<mptr>::get(), __VA_ARGS__)
#endif

DECLARE_CLASS_CODEGEN(Qosmetics::Notes, SelectionViewController, HMUI::ViewController,

                      DECLARE_INSTANCE_FIELD(Qosmetics::Core::QosmeticObjectTableData*, descriptorList);
                      DECLARE_INSTANCE_FIELD(Qosmetics::Core::DeletionConfirmationModal*, deletionConfirmationModal);

                      DECLARE_OVERRIDE_METHOD_MATCH(void, DidActivate, &HMUI::ViewController::DidActivate, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling);

                      public
                      :

                      void OnSelectDescriptor(int idx);
                      void ReloadDescriptorList();

)
