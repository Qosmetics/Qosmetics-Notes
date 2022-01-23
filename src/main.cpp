#include "custom-types/shared/register.hpp"
#include "hooks.hpp"
#include "logging.hpp"
#include "modloader/shared/modloader.hpp"

#include "HMUI/CurvedTextMeshPro.hpp"
#include "HMUI/ImageView.hpp"

#include "qosmetics-core/shared/RedecorationRegister.hpp"

#include "GlobalNamespace/BeatmapObjectsInstaller.hpp"
#include "GlobalNamespace/GameNoteController.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/Vector3.hpp"

#include "CustomTypes/Cyoob.hpp"

REDECORATION_REGISTRATION(normalBasicNotePrefab, 10, true, GlobalNamespace::GameNoteController*, GlobalNamespace::BeatmapObjectsInstaller*)
{
    normalBasicNotePrefab->get_gameObject()->AddComponent<Qosmetics::Notes::Cyoob*>();
    return normalBasicNotePrefab;
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