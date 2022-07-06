#include "CustomTypes/BombColorHandler.hpp"
#include "MaterialUtils.hpp"
#include "PropertyID.hpp"
#include "logging.hpp"

#include "UnityEngine/MaterialPropertyBlock.hpp"
#include "UnityEngine/Renderer.hpp"
#include "UnityEngine/Shader.hpp"

DEFINE_TYPE(Qosmetics::Notes, BombColorHandler);

namespace Qosmetics::Notes
{
    void BombColorHandler::Awake()
    {
        FetchCCMaterials();
        propertyController = get_gameObject()->GetComponentInParent<GlobalNamespace::MaterialPropertyBlockController*>();
        AddRenderersToPropertyBlockController();
    }

    void BombColorHandler::SetColor(Sombrero::FastColor color)
    {
        if (lastColor.operator==(color))
            return;

        lastColor = color;

        for (auto* mat : customColorMaterials)
        {
            if (mat->HasProperty(PropertyID::_Color()))
                mat->SetColor(PropertyID::_Color(), color);
        }

        if (propertyController)
        {
            propertyController->get_materialPropertyBlock()->SetColor(PropertyID::_Color(), color);
            propertyController->ApplyChanges();
        }
    }

    void BombColorHandler::FetchCCMaterials()
    {
        if (customColorMaterials)
            return;
        auto renderers = GetComponentsInChildren<UnityEngine::Renderer*>(true);

        std::vector<UnityEngine::Material*> customColorMaterialsVec = {};
        std::vector<UnityEngine::Renderer*> materialReplacementRenderersVec = {};
        for (auto renderer : renderers)
        {
            auto materials = renderer->get_materials();
            bool addedRenderer = false;

            for (auto material : materials)
            {
                if (MaterialUtils::ShouldCC(material))
                    customColorMaterialsVec.push_back(material);
                else if (!addedRenderer && MaterialUtils::ShouldReplaceExtraCC(material))
                {
                    addedRenderer = true;
                    materialReplacementRenderersVec.push_back(renderer);
                }
            }
        }
        DEBUG("Found {} custom colors materials", customColorMaterialsVec.size());
        DEBUG("Found {} replacement renderers", materialReplacementRenderersVec.size());
        customColorMaterials = il2cpp_utils::vectorToArray(customColorMaterialsVec);
        materialReplacementRenderers = il2cpp_utils::vectorToArray(materialReplacementRenderersVec);
    }

    void BombColorHandler::AddRenderersToPropertyBlockController()
    {
        if (!propertyController)
            return;
        auto renderers = propertyController->get_renderers();
        DEBUG("Creating new renderers");
        ArrayW<UnityEngine::Renderer*> newRenderers(renderers.Length() + materialReplacementRenderers.Length());

        DEBUG("copying first set of renderers");
        memcpy(newRenderers.begin(), renderers.begin(), renderers.Length() * sizeof(UnityEngine::Renderer*));
        memcpy(newRenderers.begin() + renderers.Length(), materialReplacementRenderers.begin(), materialReplacementRenderers.Length() * sizeof(UnityEngine::Renderer*));
        propertyController->renderers = newRenderers;
    }

}