#include "UI/SelectionViewController.hpp"
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

            auto defaultObjectBtn = CreateUIButton(buttonHorizontal->get_transform(), "Default", []() {});
            auto refreshBtn = CreateUIButton(buttonHorizontal->get_transform(), "Refresh", []() {});

            deletionConfirmationModal = Qosmetics::Core::DeletionConfirmationModal::Create(get_transform());
            descriptorList = CreateScrollableCustomSourceList<Qosmetics::Core::QosmeticObjectTableData*>(vertical->get_transform(), UnityEngine::Vector2(0.0f, 0.0f), UnityEngine::Vector2(100.0f, 80.0f), std::bind(&Qosmetics::Notes::SelectionViewController::OnSelectDescriptor, this, std::placeholders::_1));
            descriptorList->deletionConfirmationModal = deletionConfirmationModal;

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
            std::string filePath = string_format("%s/%s", cyoob_path, cyoob.c_str());
            auto orig = std::find_if(descriptorSet.begin(), descriptorSet.end(), [filePath](auto& d)
                                     { return d.get_filePath() == filePath; });
            // check if the cyoob was already parsed
            if (orig != descriptorSet.end())
                continue;

            // get bytes from the zip file that a cyoob is
            std::vector<uint8_t> packageData;
            if (Qosmetics::Core::ZipUtils::GetBytesFromZipFile(filePath, "package.json", packageData))
            {
                rapidjson::Document doc;
                doc.Parse(reinterpret_cast<char*>(packageData.data()));
                // add to the set
                descriptorSet.emplace(doc["descriptor"], filePath);
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

    void SelectionViewController::OnSelectDescriptor(int idx)
    {
        if (descriptorList->objectDescriptors.size() < idx)
            return;
        auto& descriptor = *std::next(descriptorList->objectDescriptors.begin(), idx);
    }
}