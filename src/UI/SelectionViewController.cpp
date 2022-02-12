#include "UI/SelectionViewController.hpp"
#include "CustomTypes/NoteModelContainer.hpp"
#include "config.hpp"
#include "diglett/shared/Diglett.hpp"
#include "logging.hpp"
#include "qosmetics-core/shared/ConfigRegister.hpp"
#include "qosmetics-core/shared/Utils/FileUtils.hpp"
#include "qosmetics-core/shared/Utils/ZipUtils.hpp"
#include "questui/shared/BeatSaberUI.hpp"
#include "questui/shared/CustomTypes/Components/List/QuestUITableView.hpp"
#include "static-defines.hpp"

#include "HMUI/TableView.hpp"
#include "HMUI/TableView_ScrollPositionType.hpp"

#include <algorithm>

DEFINE_TYPE(Qosmetics::Notes, SelectionViewController);

using namespace QuestUI::BeatSaberUI;

namespace Qosmetics::Notes
{
    void SelectionViewController::DidActivate(bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling)
    {
        if (firstActivation)
        {
            auto vertical = CreateVerticalLayoutGroup(get_transform());
            auto buttonHorizontal = CreateHorizontalLayoutGroup(vertical->get_transform());

            auto localization = Localization::GetSelected();
            auto defaultObjectBtn = CreateUIButton(buttonHorizontal->get_transform(), localization->Get("QosmeticsCore:QosmeticsTable:Default"), std::bind(&SelectionViewController::OnSelectDefault, this));
            auto refreshBtn = CreateUIButton(buttonHorizontal->get_transform(), localization->Get("QosmeticsCore:QosmeticsTable:Refresh"), std::bind(&SelectionViewController::ReloadDescriptorList, this));

            deletionConfirmationModal = Qosmetics::Core::DeletionConfirmationModal::Create(get_transform());
            descriptorList = CreateScrollableCustomSourceList<Qosmetics::Core::QosmeticObjectTableData*>(vertical->get_transform(), UnityEngine::Vector2(0.0f, 0.0f), UnityEngine::Vector2(100.0f, 80.0f), nullptr);
            descriptorList->deletionConfirmationModal = deletionConfirmationModal;
            descriptorList->onSelect = std::bind(reinterpret_cast<void (SelectionViewController::*)(HMUI::TableCell*)>(&SelectionViewController::OnSelectDescriptor), this, std::placeholders::_1);
            ReloadDescriptorList();
        }
    }

    void SelectionViewController::ReloadDescriptorList()
    {
        std::vector<std::string> cyoobs = {};
        Qosmetics::Core::FileUtils::GetFilesInFolderPath("cyoob", cyoob_path, cyoobs);
        auto tableView = reinterpret_cast<QuestUI::TableView*>(descriptorList->tableView);
        int scrolledRow = tableView->get_scrolledRow();

        auto& descriptorSet = descriptorList->objectDescriptors;
        int current = 0;
        for (auto& cyoob : cyoobs)
        {
            current++;
            std::string filePath = fmt::format("{}/{}", cyoob_path, cyoob);
            auto orig = std::find_if(descriptorSet.begin(), descriptorSet.end(), [filePath](auto& d)
                                     { return d.get_filePath() == filePath; });
            // check if the cyoob was already parsed
            if (orig != descriptorSet.end())
                continue;

            // get bytes from the zip file that a cyoob is
            std::vector<uint8_t> packageData;
            if (Qosmetics::Core::ZipUtils::GetBytesFromZipFile(filePath, "package.json", packageData))
            {
                DEBUG("Got package: {}", std::string(packageData.begin(), packageData.end()).c_str());
                rapidjson::Document doc;
                doc.Parse(std::string(packageData.begin(), packageData.end()));
                // add to the set
                try
                {
                    descriptorSet.emplace(doc["descriptor"], filePath);
                }
                catch (const std::runtime_error& e)
                {
                    ERROR("error thrown while parsing descriptor from package: {}", e.what());
                }
            }
        }

        // check each descriptor to see if it still exists on disk, if not it should be removed from the list
        for (auto it = descriptorSet.begin(); it != descriptorSet.end(); /* nothing */)
        {
            auto itr = std::find(cyoobs.begin(), cyoobs.end(), Qosmetics::Core::FileUtils::GetFileName(it->get_filePath()));
            if (itr == cyoobs.end())
                it = descriptorSet.erase(it);
            else
                it++;
        }

        tableView->ReloadData();
        tableView->RefreshCells(true, true);
        tableView->ScrollToCellWithIdx(std::clamp(scrolledRow, 0, (int)descriptorSet.size()), HMUI::TableView::ScrollPositionType::Beginning, true);
    }

    void SelectionViewController::OnSelectDefault()
    {
        auto noteModelContainer = NoteModelContainer::get_instance();
        noteModelContainer->Default();

        OnObjectLoadFinished();
    }

    void SelectionViewController::OnSelectDescriptor(Qosmetics::Core::QosmeticObjectTableCell* cell)
    {
        if (cell)
        {
            auto& descriptor = cell->descriptor;
            if (NoteModelContainer::get_instance()->LoadObject(descriptor, std::bind(&SelectionViewController::OnObjectLoadFinished, this)))
            {
                previewViewController->ClearPrefab();
                previewViewController->ShowLoading(true);
            }
            else
            {
                previewViewController->UpdatePreview(false);
            }
        }
    }

    void SelectionViewController::OnObjectLoadFinished()
    {
        // something to do after we changed the object, like update preview
        auto noteModelContainer = NoteModelContainer::get_instance();

        Qosmetics::Notes::Config::get_config().lastUsedCyoob = noteModelContainer->GetNoteConfig().get_isDefault() ? "" : Qosmetics::Core::FileUtils::GetFileName(noteModelContainer->GetDescriptor().get_filePath(), true);
        Qosmetics::Core::Config::SaveConfig();
        previewViewController->UpdatePreview(true);
    }
}