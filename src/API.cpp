#include "API.hpp"
#include "ConstStrings.hpp"
#include "Disabling.hpp"
#include "conditional-dependencies/shared/main.hpp"
#include "config.hpp"
#include "qosmetics-core/shared/Data/Descriptor.hpp"
#include "static-defines.hpp"
#include <fmt/core.h>

#include "CustomTypes/NoteModelContainer.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/Transform.hpp"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreturn-type-c-linkage"

#define GET_NOTEMODELCONTAINER() auto noteModelContainer = Qosmetics::Notes::NoteModelContainer::get_instance()
// TODO: Implement methods here lol
EXPOSE_API(GetActiveDescriptor, Qosmetics::Core::Descriptor)
{
    return Qosmetics::Notes::NoteModelContainer::get_instance()->GetDescriptor();
}

EXPOSE_API(GetConfig, Qosmetics::Notes::NotesConfig*)
{
    return reinterpret_cast<Qosmetics::Notes::NotesConfig*>(&Qosmetics::Notes::Config::get_config());
}

EXPOSE_API(SetDefault, void)
{
    Qosmetics::Notes::NoteModelContainer::get_instance()->Default();
}

EXPOSE_API(SetActiveFromFilePath, bool, std::string filePath)
{
    if (!fileexists(filePath))
        return false;
    auto manifest = Qosmetics::Notes::NoteModelContainer::Manifest(filePath);
    return Qosmetics::Notes::NoteModelContainer::get_instance()->LoadObject(manifest, nullptr);
}

EXPOSE_API(SetActive, bool, std::string fileName)
{
    std::string filePath = fmt::format("{}/{}", cyoob_path, fileName);
    if (!fileexists(filePath))
        return false;
    auto manifest = Qosmetics::Notes::NoteModelContainer::Manifest(filePath);
    return Qosmetics::Notes::NoteModelContainer::get_instance()->LoadObject(manifest, nullptr);
}

EXPOSE_API(GetNoteIsCustom, bool)
{
    return Qosmetics::Notes::NoteModelContainer::get_instance()->currentNoteObject != nullptr;
}

EXPOSE_API(GetBomb, UnityEngine::GameObject*)
{
    GET_NOTEMODELCONTAINER();

    if (!noteModelContainer->currentNoteObject)
        return nullptr;
    auto& config = noteModelContainer->GetNoteConfig();
    if (!config.get_hasBomb())
        return nullptr;
    return UnityEngine::Object::Instantiate(noteModelContainer->currentNoteObject->get_transform()->Find(ConstStrings::Bomb())->get_gameObject());
}

EXPOSE_API(GetDebris, UnityEngine::GameObject*, bool right)
{
    GET_NOTEMODELCONTAINER();

    if (!noteModelContainer->currentNoteObject)
        return nullptr;
    auto& config = noteModelContainer->GetNoteConfig();
    if (!config.get_hasDebris())
        return nullptr;
    auto debrisT = noteModelContainer->currentNoteObject->get_transform()->Find(ConstStrings::Debris());
    return UnityEngine::Object::Instantiate(debrisT->Find(right ? ConstStrings::RightDebris() : ConstStrings::LeftDebris())->get_gameObject());
}

EXPOSE_API(GetNote, UnityEngine::GameObject*, int type)
{
    GET_NOTEMODELCONTAINER();

    if (!noteModelContainer->currentNoteObject)
        return nullptr;
    auto t = noteModelContainer->currentNoteObject->get_transform();
    switch (type)
    {
    case 0:
        return UnityEngine::Object::Instantiate(t->Find(ConstStrings::LeftArrow())->get_gameObject());
    case 1:
        return UnityEngine::Object::Instantiate(t->Find(ConstStrings::RightArrow())->get_gameObject());
    case 2:
        return UnityEngine::Object::Instantiate(t->Find(ConstStrings::LeftDot())->get_gameObject());
    case 3:
        return UnityEngine::Object::Instantiate(t->Find(ConstStrings::RightDot())->get_gameObject());
    }
    return nullptr;
}

EXPOSE_API(GetPrefabClone, UnityEngine::GameObject*)
{
    GET_NOTEMODELCONTAINER();

    if (!noteModelContainer->currentNoteObject)
        return nullptr;
    return UnityEngine::Object::Instantiate(noteModelContainer->currentNoteObject);
}

EXPOSE_API(GetPrefab, UnityEngine::GameObject*)
{
    return Qosmetics::Notes::NoteModelContainer::get_instance()->currentNoteObject;
}

EXPOSE_API(GetNotesDisabled, bool)
{
    return Qosmetics::Notes::Disabling::GetAnyDisabling();
}

EXPOSE_API(UnregisterNoteDisablingInfo, void, ModInfo info)
{
    Qosmetics::Notes::Disabling::UnregisterDisablingModInfo(info);
}

EXPOSE_API(RegisterNoteDisablingInfo, void, ModInfo info)
{
    Qosmetics::Notes::Disabling::RegisterDisablingModInfo(info);
}

EXPOSE_API(GetNoteFolder, std::string)
{
    return cyoob_path;
}
#pragma GCC diagnostic pop
