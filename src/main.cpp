#include "CustomTypes/CyoobParent.hpp"
#include "CustomTypes/NoteModelContainer.hpp"
#include "UI/CyoobFlowCoordinator.hpp"
#include "assets.hpp"
#include "custom-types/shared/register.hpp"
#include "diglett/shared/Register.hpp"
#include "hooks.hpp"
#include "logging.hpp"
#include "modloader/shared/modloader.hpp"
#include "static-defines.hpp"

#include "HMUI/CurvedTextMeshPro.hpp"
#include "HMUI/ImageView.hpp"

#include "qosmetics-core/shared/FlowCoordinatorRegister.hpp"
#include "questui/shared/BeatSaberUI.hpp"

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

    Diglett::Register::RegisterLocales<Diglett::Languages::German>(logger, ASSET_TO_XML(de_xml));
    Diglett::Register::RegisterLocales<Diglett::Languages::English>(logger, ASSET_TO_XML(en_xml));
    Diglett::Register::RegisterLocales<Diglett::Languages::Spanish>(logger, ASSET_TO_XML(es_xml));
    Diglett::Register::RegisterLocales<Diglett::Languages::French>(logger, ASSET_TO_XML(fr_xml));
    Diglett::Register::RegisterLocales<Diglett::Languages::Japanese>(logger, ASSET_TO_XML(ja_xml));
    Diglett::Register::RegisterLocales<Diglett::Languages::Korean>(logger, ASSET_TO_XML(ko_xml));
}