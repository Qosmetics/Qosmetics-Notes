#pragma once
#include "NotesConfig.hpp"
#include "conditional-dependencies/shared/main.hpp"
#include <optional>

#if __has_include("qosmetics-core/shared/Data/Descriptor.hpp")
#include "qosmetics-core/shared/Data/Descriptor.hpp"
#ifndef QOSMETICS_CORE
#define QOSMETICS_CORE
#endif
#endif

#if __has_include("UnityEngine/GameObject.hpp")
#include "UnityEngine/GameObject.hpp"
#else
namespace UnityEngine
{
    typedef Il2CppObject GameObject;
}
#endif

#define QOSMETICS_NOTES "qosmetics-notes"

namespace Qosmetics::Notes::API
{
// only available if you also have qosmetics core as a dep
#ifdef QOSMETICS_CORE
    /// @brief optionally get the active note descriptor
    /// @return std::nullopt if not installed, descriptor if installed (check name to see if it's default)
    std::optional<Qosmetics::Core::Descriptor> GetActiveDescriptor()
    {
        static auto function = CondDeps::Find<Qosmetics::Core::Descriptor>(QOSMETICS_NOTES, "GetActiveDescriptor");
        if (function)
            return function.value()();
        return std::nullopt;
    }
#endif

    /// @brief optionally get a const reference to the Qosmetics Config
    /// @return std::nullopt if not installed, const reference wrapper otherwise
    std::optional<std::reference_wrapper<const Qosmetics::Notes::NotesConfig>> GetConfig()
    {
        static auto function = CondDeps::Find<Qosmetics::Notes::NotesConfig*>(QOSMETICS_NOTES, "GetConfig");
        if (function)
            return *function.value()();
        return std::nullopt;
    }

    /// @brief Set the default note as being active
    void SetDefault()
    {
        static auto function = CondDeps::Find<void>(QOSMETICS_NOTES, "SetDefault");
        if (function)
            function.value()();
    }

    /// @brief Set a specific note as being active, by passing the path of the object
    /// @param filePath the path of the object
    /// @return std::nullopt if not installed, false if no new object got loaded, true if a new object got loaded
    std::optional<bool> SetActiveFromFilePath(std::string filePath)
    {
        static auto function = CondDeps::Find<bool, std::string>(QOSMETICS_NOTES, "SetActiveFromFilePath");
        if (function)
            return function.value()(filePath);
        return std::nullopt;
    }

    /// @brief Set a specific note as being active, by passing the filename of the object (with extension!)
    /// @param obj the filename of the object
    /// @return std::nullopt if not installed, false if no new object got loaded, true if a new object got loaded
    std::optional<bool> SetActive(std::string fileName)
    {
        static auto function = CondDeps::Find<bool, std::string>(QOSMETICS_NOTES, "SetActive");
        if (function)
            return function.value()(fileName);
        return std::nullopt;
    }

    /// @brief checks if the current note is a custom one or default one
    /// @return std::nullopt if not installed, false if not custom, true if custom
    std::optional<bool> GetNoteIsCustom()
    {
        static auto function = CondDeps::Find<bool>(QOSMETICS_NOTES, "GetNoteIsCustom");
        if (function)
            return function.value()();
        return std::nullopt;
    }

    /// @brief optionally get a clone of the bomb
    /// @return std::nullopt if not installed, nullptr if no bomb on object, if there is a bomb a gameobject pointer to a cloned bomb object
    std::optional<UnityEngine::GameObject*> GetBomb()
    {
        static auto function = CondDeps::Find<UnityEngine::GameObject*>(QOSMETICS_NOTES, "GetBomb");
        if (function)
            return function.value()();
        return std::nullopt;
    }

    /// @brief optionally get a clone of a debris type
    /// @param right pass false for left, true for right
    /// @return std::nullopt if not installed, nullptr if no debris on object, if there is debris a gameobject pointer to a cloned debris object
    std::optional<UnityEngine::GameObject*> GetDebris(bool right)
    {
        static auto function = CondDeps::Find<UnityEngine::GameObject*, bool>(QOSMETICS_NOTES, "GetDebris");
        if (function)
            return function.value()(right);
        return std::nullopt;
    }

    /// @brief optionally get a clone of a note type
    /// @param type the type to get, 0 - leftArrow, 1 - rightArrow, 2 - leftDot, 3 - rightDot
    /// @return std::nullopt if not installed, nullptr if no notes on object, if there is notes a gameobject pointer to a cloned note object
    std::optional<UnityEngine::GameObject*> GetNote(int type)
    {
        static auto function = CondDeps::Find<UnityEngine::GameObject*, int>(QOSMETICS_NOTES, "GetNote");
        if (function)
            return function.value()(type);
        return std::nullopt;
    }

    /// @brief optionally get a clone of a note type
    /// @param right whether to get right (true) or left (false)
    /// @param dot whether to get arrows (false) or dot (true)
    /// @return std::nullopt if not installed, nullptr if no notes on object, if there is notes a gameobject pointer to a cloned note object
    std::optional<UnityEngine::GameObject*> GetNote(bool right, bool dot)
    {
        return GetNote(right + dot * 2);
    }

    /// @brief optionally get a clone of the entire prefab
    /// @return std::nullopt if not installed, nullptr if default, if there is debris a gameobject pointer to a cloned note object
    std::optional<UnityEngine::GameObject*> GetPrefabClone()
    {
        static auto function = CondDeps::Find<UnityEngine::GameObject*>(QOSMETICS_NOTES, "GetPrefabClone");
        if (function)
            return function.value()();
        return std::nullopt;
    }

    /// @brief optionally get the entire prefab (Please don't edit to break things!)
    /// @return std::nullopt if not installed, nullptr if default, if there is something selected a pointer to the original object
    std::optional<UnityEngine::GameObject*> GetPrefab()
    {
        static auto function = CondDeps::Find<UnityEngine::GameObject*>(QOSMETICS_NOTES, "GetPrefab");
        if (function)
            return function.value()();
        return std::nullopt;
    }

    /// @brief checks if notes are disabled due to a mod registering itself as disabling
    /// @return std::nullopt if not installed, false if not disabled, true if disabled
    std::optional<bool> GetNotesDisabled()
    {
        static auto function = CondDeps::Find<bool>(QOSMETICS_NOTES, "GetNotesDisabled");
        if (function)
            return function.value()();
        return std::nullopt;
    }

    /// @brief Unregister your mod as disabling Notes replacements
    /// @param info your mod info
    void UnregisterNoteDisablingInfo(ModInfo info)
    {
        static auto function = CondDeps::Find<void, ModInfo>(QOSMETICS_NOTES, "UnregisterNoteDisablingInfo");
        if (function)
            function.value()(info);
    }

    /// @brief register your mod as disabling Notes replacements
    /// @param info your mod info
    void RegisterNoteDisablingInfo(ModInfo info)
    {
        static auto function = CondDeps::Find<void, ModInfo>(QOSMETICS_NOTES, "RegisterNoteDisablingInfo");
        if (function)
            function.value()(info);
    }

    /// @brief get the note folder
    /// @return std::nullopt if not installed, else folder path for note folder
    std::optional<std::string> GetNoteFolder()
    {
        static auto function = CondDeps::Find<std::string>(QOSMETICS_NOTES, "GetNoteFolder");
        if (function)
            return function.value()();
        return std::nullopt;
    }
}