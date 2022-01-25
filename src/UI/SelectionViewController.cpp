#include "UI/SelectionViewController.hpp"
#include "qosmetics-core/shared/Utils/FileUtils.hpp"
#include "qosmetics-core/shared/Utils/ZipUtils.hpp"
#include "questui/shared/BeatSaberUI.hpp"
#include "static-defines.hpp"

#include "HMUI/TableView.hpp"

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
            descriptorList = CreateScrollableCustomSourceList<Qosmetics::Core::QosmeticObjectTableData*>(vertical->get_transform(), std::bind(&Qosmetics::Notes::SelectionViewController::OnSelectDescriptor, this, std::placeholders::_1));
            descriptorList->deletionConfirmationModal = deletionConfirmationModal;

            ReloadDescriptorList();
        }
    }

    void SelectionViewController::ReloadDescriptorList()
    {
        std::vector<std::string> cyoobs = {};
        Qosmetics::Core::FileUtils::GetFilesInFolderPath("cyoob", cyoob_path, cyoobs);

        auto& descriptorSet = descriptorList->objectDescriptors;
        for (auto& cyoob : cyoobs)
        {
            std::string filePath = string_format("%s/%s", cyoob_path, cyoob.c_str());
            auto orig = std::find_if(descriptorSet.begin(), descriptorSet.end(), [filePath](auto& d)
                                     { return d.get_filePath() == filePath; });
            if (orig != descriptorSet.end())
                continue;

            std::vector<uint8_t> descriptorSet;
            if (Qosmetics::Core::ZipUtils::GetBytesFromZipFile(filePath, "package.json", descriptorSet))
            {
                rapidjson::Document doc;
                doc.Parse(reinterpret_cast<char*>(descriptorSet.data()));
                descriptorSet.emplace_back(doc["descriptor"], filePath);
            }
        }

        for (auto it = descriptorSet.begin(); it != descriptorSet.end(); /* nothing */)
        {
            auto itr = std::find(cyoobs.begin(), cyoobs.end(), Qosmetics::Core::FileUtils::GetFileName(it->get_filePath()));
            if (itr == cyoobs.end())
                it = descriptorSet.erase(it);
            else
                it++;
        }

        descriptorList->tableView->ReloadData();
        descriptorList->tableView->RefreshCells(true, true);
    }

    void SelectionViewController::OnSelectDescriptor(int idx)
    {
        if (descriptorList->objectDescriptors.size() < idx)
            return;
        auto& descriptor = *std::next(descriptorList->objectDescriptors.begin(), idx);
    }
}