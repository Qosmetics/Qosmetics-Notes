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
#include "pinkcore/shared/API.hpp"
#include "pinkcore/shared/RequirementAPI.hpp"

QOSMETICS_FLOWCOORDINATOR_REGISTER(Cyoobs, Qosmetics::Notes::CyoobFlowCoordinator*)
{
    auto inactive = QuestUI::BeatSaberUI::ArrayToSprite(IncludedAssets::NoteIcon_png);
    auto active = QuestUI::BeatSaberUI::ArrayToSprite(IncludedAssets::NoteIconSelected_png);
    return std::make_pair(inactive, active);
}

ModInfo modInfo = {MOD_ID, VERSION};

extern "C" void setup(ModInfo& info)
{
    info = modInfo;
}

bool useChroma = false;
bool chromaInReqs = false;
bool chromaInSugs = false;

void UpdateUseChromaSug(const std::vector<std::string>& sugs)
{
    auto itr = std::find(sugs.begin(), sugs.end(), "Chroma");
    chromaInSugs = itr != sugs.end();
    useChroma = chromaInSugs || chromaInReqs;
    INFO("Qosmetics Cyoobs useChroma: {}", useChroma);
}

void UpdateUseChromaReq(const std::vector<std::string>& reqs)
{
    auto itr = std::find(reqs.begin(), reqs.end(), "Chroma");
    chromaInReqs = itr != reqs.end();
    useChroma = chromaInSugs || chromaInReqs;
    INFO("Qosmetics Cyoobs useChroma: {}", useChroma);
}

extern "C" void load()
{
    il2cpp_functions::Class_Init(classof(HMUI::ImageView*));
    il2cpp_functions::Class_Init(classof(HMUI::CurvedTextMeshPro*));

    mkpath(cyoob_path);
    auto& logger = Qosmetics::Notes::Logging::getLogger();
    Hooks::InstallHooks(logger);
    custom_types::Register::AutoRegister();

    Diglett::RegisterAsset(static_cast<std::string_view>(IncludedAssets::de_xml), Diglett::Language::GERMAN);
    Diglett::RegisterAsset(static_cast<std::string_view>(IncludedAssets::en_xml), Diglett::Language::ENGLISH);
    Diglett::RegisterAsset(static_cast<std::string_view>(IncludedAssets::es_xml), Diglett::Language::SPANISH);
    Diglett::RegisterAsset(static_cast<std::string_view>(IncludedAssets::fr_xml), Diglett::Language::FRENCH);
    Diglett::RegisterAsset(static_cast<std::string_view>(IncludedAssets::ja_xml), Diglett::Language::JAPANESE);
    Diglett::RegisterAsset(static_cast<std::string_view>(IncludedAssets::ko_xml), Diglett::Language::KOREAN);

    PinkCore::API::GetFoundSuggestionCallbackSafe() += UpdateUseChromaSug;
    PinkCore::API::GetFoundRequirementCallbackSafe() += UpdateUseChromaReq;
}