#include "CustomTypes/CyoobColorHandler.hpp"
#include "MaterialUtils.hpp"
#include "PropertyID.hpp"
#include "logging.hpp"

#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/MaterialPropertyBlock.hpp"
#include "UnityEngine/Renderer.hpp"
#include "UnityEngine/Shader.hpp"
#include "UnityEngine/Transform.hpp"
#include <fmt/format.h>

#include "sombrero/shared/linq_functional.hpp"
using namespace Sombrero::Linq::Functional;

DEFINE_TYPE(Qosmetics::Notes, CyoobColorHandler);

namespace Qosmetics::Notes
{
    void CyoobColorHandler::Awake()
    {
        FetchCCMaterials();
        propertyController = get_gameObject()->GetComponentInParent<GlobalNamespace::MaterialPropertyBlockController*>();
        AddRenderersToPropertyBlockController();
    }

    void CyoobColorHandler::SetColors(Sombrero::FastColor thisColor, Sombrero::FastColor thatColor)
    {
        if (lastThisColor.operator!=(thisColor))
        {
            DEBUG("setting thisColor: {:.2f}, {:.2f}, {:.2f}, {:.2f}", thisColor.r, thisColor.g, thisColor.b, thisColor.a);
            lastThisColor = thisColor;
            for (auto* mat : thisColorMaterials)
                mat->SetColor(PropertyID::_Color(), thisColor);

            if (propertyController)
            {
                propertyController->get_materialPropertyBlock()->SetColor(PropertyID::_Color(), thisColor);
                propertyController->ApplyChanges();
            }
        }

        if (lastThatColor.operator!=(thatColor))
        {
            DEBUG("setting thatColor: {:.2f}, {:.2f}, {:.2f}, {:.2f}", thisColor.r, thisColor.g, thisColor.b, thisColor.a);
            lastThatColor = thatColor;
            for (auto* mat : thatColorMaterials)
                mat->SetColor(PropertyID::_OtherColor(), thatColor);
        }
    }

    void CyoobColorHandler::FetchCCMaterials()
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
                // if CC material, add to CC vec
                if (MaterialUtils::ShouldCC(material))
                {
                    customColorMaterialsVec.push_back(material);
                }
                // if we didn't add this renderer yet, and the material is a replacement one with CC enabled, add it to the replacement renderer vec
                else if (!addedRenderer && MaterialUtils::ShouldReplaceExtraCC(material))
                {
                    addedRenderer = true;
                    materialReplacementRenderersVec.push_back(renderer);
                }
            }
        }
        DEBUG("Found {} custom colors materials", customColorMaterialsVec.size());
        DEBUG("Found {} replacement renderers", materialReplacementRenderersVec.size());
        customColorMaterials = ArrayW<UnityEngine::Material*>(customColorMaterialsVec.size());
        memcpy(customColorMaterials.begin(), customColorMaterialsVec.data(), customColorMaterialsVec.size() * sizeof(UnityEngine::Material*));
        materialReplacementRenderers = ArrayW<UnityEngine::Renderer*>(materialReplacementRenderersVec.size());
        memcpy(materialReplacementRenderers.begin(), materialReplacementRenderersVec.data(), materialReplacementRenderersVec.size() * sizeof(UnityEngine::Renderer*));

        thisColorMaterials = customColorMaterials |
                             Where([](auto x)
                                   { return x->HasProperty(PropertyID::_Color()); }) |
                             ToArray();
        thatColorMaterials = customColorMaterials |
                             Where([](auto x)
                                   { return x->HasProperty(PropertyID::_OtherColor()); }) |
                             ToArray();
    }

    void CyoobColorHandler::AddRenderersToPropertyBlockController()
    {
        if (!propertyController)
            return;
        auto renderers = propertyController->get_renderers();
        DEBUG("Creating new renderers");
        ArrayW<UnityW<UnityEngine::Renderer>> newRenderers(renderers.size() + materialReplacementRenderers.size());

        DEBUG("copying first set of renderers");
        memcpy(newRenderers.begin(), renderers.begin(), renderers.size() * sizeof(UnityEngine::Renderer*));
        memcpy(newRenderers.begin() + renderers.size(), materialReplacementRenderers.begin(), materialReplacementRenderers.size() * sizeof(UnityEngine::Renderer*));
        propertyController->_renderers = newRenderers;
    }
}
