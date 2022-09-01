#pragma once

#include "CustomTypes/NoteModelContainer.hpp"
#include "HMUI/ViewController.hpp"
#include "UI/PreviewViewController.hpp"
#include "custom-types/shared/macros.hpp"

#include "qosmetics-core/shared/UI/Components/DeletionConfirmationModal.hpp"
#include "qosmetics-core/shared/UI/Components/QosmeticObjectTableCell.hpp"
#include "qosmetics-core/shared/UI/Components/QosmeticObjectTableData.hpp"

#include "lapiz/shared/macros.hpp"

#ifndef DECLARE_OVERRIDE_METHOD_MATCH
#define DECLARE_OVERRIDE_METHOD_MATCH(retval, name, mptr, ...) \
    DECLARE_OVERRIDE_METHOD(retval, name, il2cpp_utils::il2cpp_type_check::MetadataGetter<mptr>::get(), __VA_ARGS__)
#endif

DECLARE_CLASS_CODEGEN(Qosmetics::Notes, SelectionViewController, HMUI::ViewController,
                      DECLARE_INSTANCE_FIELD(NoteModelContainer*, noteModelContainer);
                      DECLARE_INSTANCE_FIELD(PreviewViewController*, previewViewController);
                      DECLARE_INSTANCE_FIELD(Qosmetics::Core::QosmeticObjectTableData*, descriptorList);
                      DECLARE_INSTANCE_FIELD(Qosmetics::Core::DeletionConfirmationModal*, deletionConfirmationModal);

                      DECLARE_INJECT_METHOD(void, Inject, PreviewViewController* previewViewController, NoteModelContainer* noteModelContainer);
                      DECLARE_OVERRIDE_METHOD_MATCH(void, DidActivate, &HMUI::ViewController::DidActivate, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling);

                      DECLARE_DEFAULT_CTOR();
                      public
                      :

                      void OnDeleteCell(Qosmetics::Core::QosmeticObjectTableCell* cell);
                      void OnSelectDescriptor(Qosmetics::Core::QosmeticObjectTableCell* cell);
                      void OnSelectDefault();
                      void ReloadDescriptorList();
                      void OnObjectLoadFinished();
                      void RefreshAfterBloqConversion();

)
