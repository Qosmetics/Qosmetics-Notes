#pragma once

#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/Material.hpp"
#include "UnityEngine/Renderer.hpp"

namespace Qosmetics::Notes::MaterialUtils
{
    bool ShouldCC(UnityEngine::Material* material);
    bool ShouldReplaceExtraCC(UnityEngine::Material* material);

    void ReplaceMaterialsForGameObject(UnityEngine::GameObject* object);
    void ReplaceMaterialForGameObjectChildren(UnityEngine::GameObject* gameObject, UnityEngine::Material* material, std::u16string_view materialToReplaceName);
    void ReplaceMaterialForRenderer(UnityEngine::Renderer* renderer, UnityEngine::Material* replacingMaterial, std::u16string_view materialToReplaceName);
}