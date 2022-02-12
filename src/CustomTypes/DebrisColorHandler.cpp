#include "CustomTypes/DebrisColorHandler.hpp"
#include "MaterialUtils.hpp"
#include "PropertyID.hpp"
#include "logging.hpp"

#include "UnityEngine/Renderer.hpp"
#include "UnityEngine/Shader.hpp"

#include <fmt/format.h>

DEFINE_TYPE(Qosmetics::Notes, DebrisColorHandler);

namespace Qosmetics::Notes
{
    void DebrisColorHandler::Awake()
    {
        FetchCCMaterials();
    }

    void DebrisColorHandler::SetColors(Sombrero::FastColor thisColor, Sombrero::FastColor thatColor)
    {
        if (lastThisColor.operator==(thisColor) && lastThatColor.operator==(thatColor))
            return;

        DEBUG("{}: setting colors!", fmt::ptr(this));
        DEBUG("thisColor: {:.2f}, {:.2f}, {:.2f}, {:.2f}", thisColor.r, thisColor.g, thisColor.b, thisColor.a);
        DEBUG("thatColor: {:.2f}, {:.2f}, {:.2f}, {:.2f}", thatColor.r, thatColor.g, thatColor.b, thatColor.a);

        lastThisColor = thisColor;
        lastThatColor = thatColor;

        for (auto* mat : customColorMaterials)
        {
            if (mat->HasProperty(PropertyID::_Color()))
                mat->SetColor(PropertyID::_Color(), thisColor);
            if (mat->HasProperty(PropertyID::_OtherColor()))
                mat->SetColor(PropertyID::_OtherColor(), thatColor);
        }
    }

    void DebrisColorHandler::FetchCCMaterials()
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