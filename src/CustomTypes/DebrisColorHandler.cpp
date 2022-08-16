#include "CustomTypes/DebrisColorHandler.hpp"
#include "MaterialUtils.hpp"
#include "PropertyID.hpp"
#include "logging.hpp"

#include "UnityEngine/MaterialPropertyBlock.hpp"
#include "UnityEngine/Shader.hpp"
#include <fmt/format.h>

#include "sombrero/shared/linq_functional.hpp"
using namespace Sombrero::Linq::Functional;

DEFINE_TYPE(Qosmetics::Notes, DebrisColorHandler);

namespace Qosmetics::Notes
{
    void DebrisColorHandler::Awake()
    {
        FetchCCMaterials();
        propertyController = get_gameObject()->GetComponentInParent<GlobalNamespace::MaterialPropertyBlockController*>();
        AddRenderersToPropertyBlockController();
    }

    void DebrisColorHandler::SetColors(Sombrero::FastColor thisColor, Sombrero::FastColor thatColor)
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

    void DebrisColorHandler::FetchCCMaterials()
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

    void DebrisColorHandler::AddRenderersToPropertyBlockController()
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