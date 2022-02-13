#include "CustomTypes/CyoobColorHandler.hpp"
#include "MaterialUtils.hpp"
#include "PropertyID.hpp"
#include "logging.hpp"

#include "UnityEngine/Renderer.hpp"
#include "UnityEngine/Shader.hpp"

#include <fmt/format.h>

DEFINE_TYPE(Qosmetics::Notes, CyoobColorHandler);

// TODO: Implement colornotevisuals meshrenderer addition (how to check what to add? (replacemenet notehd / notelw materials obv))
namespace Qosmetics::Notes
{
    void CyoobColorHandler::Awake()
    {
        FetchCCMaterials();
    }

    void CyoobColorHandler::SetColors(UnityEngine::Color thisColor, UnityEngine::Color thatColor)
    {
        if (lastThisColor == thisColor && lastThatColor == thatColor)
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

    void CyoobColorHandler::FetchCCMaterials()
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