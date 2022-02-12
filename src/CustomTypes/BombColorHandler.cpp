#include "CustomTypes/BombColorHandler.hpp"
#include "MaterialUtils.hpp"
#include "PropertyID.hpp"
#include "logging.hpp"

#include "UnityEngine/Renderer.hpp"
#include "UnityEngine/Shader.hpp"

DEFINE_TYPE(Qosmetics::Notes, BombColorHandler);

namespace Qosmetics::Notes
{
    void BombColorHandler::Awake()
    {
        FetchCCMaterials();
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
    }

    void BombColorHandler::FetchCCMaterials()
    {
        if (customColorMaterials)
            return;
        auto renderers = GetComponentsInChildren<UnityEngine::Renderer*>(true);

        std::vector<UnityEngine::Material*> customColorMaterialsVec = {};
        for (auto renderer : renderers)
        {
            auto materials = renderer->get_materials();

            for (auto material : materials)
            {
                if (MaterialUtils::ShouldCC(material))
                    customColorMaterialsVec.push_back(material);
            }
        }
        DEBUG("Found {} custom colors materials", customColorMaterialsVec.size());
        customColorMaterials = il2cpp_utils::vectorToArray(customColorMaterialsVec);
    }

}