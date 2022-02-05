#include "custom-types/shared/register.hpp"
#include "hooks.hpp"
#include "logging.hpp"
#include "modloader/shared/modloader.hpp"

#include "HMUI/CurvedTextMeshPro.hpp"
#include "HMUI/ImageView.hpp"

#include "qosmetics-core/shared/FlowCoordinatorRegister.hpp"
#include "questui/shared/BeatSaberUI.hpp"

#include "CustomTypes/NoteModelContainer.hpp"
#include "UI/CyoobFlowCoordinator.hpp"
#include "assets.hpp"

#include <vector>

#include "GlobalNamespace/MainFlowCoordinator.hpp"

QOSMETICS_FLOWCOORDINATOR_REGISTER(Cyoobs, Qosmetics::Notes::CyoobFlowCoordinator*)
{
    auto inactive_data = NoteIcon_png::getData();
    auto inactive = QuestUI::BeatSaberUI::VectorToSprite(std::vector<uint8_t>(inactive_data, inactive_data + NoteIcon_png::getLength()));
    auto active_data = NoteIconSelected_png::getData();
    auto active = QuestUI::BeatSaberUI::VectorToSprite(std::vector<uint8_t>(active_data, active_data + NoteIconSelected_png::getLength()));
    return std::make_pair(inactive, active);
}

ModInfo modInfo = {ID, VERSION};

extern "C" void setup(ModInfo& info)
{
    info = modInfo;
}

extern "C" void load()
{
    il2cpp_functions::Class_Init(classof(HMUI::ImageView*));
    il2cpp_functions::Class_Init(classof(HMUI::CurvedTextMeshPro*));

    Hooks::InstallHooks(Qosmetics::Notes::Logging::getLogger());
    custom_types::Register::AutoRegister();
}