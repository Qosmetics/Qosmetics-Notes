#include "CustomTypes/CyoobParent.hpp"
#include "CustomTypes/NoteModelContainer.hpp"
#include "UI/CyoobFlowCoordinator.hpp"
#include "assets.hpp"
#include "custom-types/shared/register.hpp"
#include "hooking.hpp"
#include "logging.hpp"
#include "modloader/shared/modloader.hpp"
#include "static-defines.hpp"

#include "HMUI/CurvedTextMeshPro.hpp"
#include "HMUI/ImageView.hpp"

#include "qosmetics-core/shared/FlowCoordinatorRegister.hpp"
#include "questui/shared/BeatSaberUI.hpp"

#include "diglett/shared/Conversions.hpp"
#include "diglett/shared/Register.hpp"

#include <vector>

#include "GlobalNamespace/MainFlowCoordinator.hpp"

bool useChroma = false;

QOSMETICS_FLOWCOORDINATOR_REGISTER(Cyoobs, Qosmetics::Notes::CyoobFlowCoordinator*)
{
    auto inactive_data = NoteIcon_png::getData();
    auto inactive = QuestUI::BeatSaberUI::VectorToSprite(std::vector<uint8_t>(inactive_data, inactive_data + NoteIcon_png::getLength()));
    auto active_data = NoteIconSelected_png::getData();
    auto active = QuestUI::BeatSaberUI::VectorToSprite(std::vector<uint8_t>(active_data, active_data + NoteIconSelected_png::getLength()));
    return std::make_pair(inactive, active);
}

ModInfo modInfo = {MOD_ID, VERSION};

extern "C" void setup(ModInfo& info)
{
    info = modInfo;
}

extern "C" void load()
{
    il2cpp_functions::Class_Init(classof(HMUI::ImageView*));
    il2cpp_functions::Class_Init(classof(HMUI::CurvedTextMeshPro*));

    mkpath(cyoob_path);
    auto& logger = Qosmetics::Notes::Logging::getLogger();
    Hooks::InstallHooks(logger);
    custom_types::Register::AutoRegister();

    // check if chroma exists
    useChroma = Modloader::requireMod("Chroma");

    Diglett::RegisterAsset(ASSET_TO_STR(de_xml), Diglett::Language::GERMAN);
    Diglett::RegisterAsset(ASSET_TO_STR(en_xml), Diglett::Language::ENGLISH);
    Diglett::RegisterAsset(ASSET_TO_STR(es_xml), Diglett::Language::SPANISH);
    Diglett::RegisterAsset(ASSET_TO_STR(fr_xml), Diglett::Language::FRENCH);
    Diglett::RegisterAsset(ASSET_TO_STR(ja_xml), Diglett::Language::JAPANESE);
    Diglett::RegisterAsset(ASSET_TO_STR(ko_xml), Diglett::Language::KOREAN);
}