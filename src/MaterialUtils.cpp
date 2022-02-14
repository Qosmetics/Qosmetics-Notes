#include "MaterialUtils.hpp"
#include "PropertyID.hpp"
#include "logging.hpp"

#include "UnityEngine/Resources.hpp"
#include "UnityEngine/Shader.hpp"

#include <fmt/format.h>

#define HAS_FLOAT(theFloat) material->HasProperty(PropertyID::theFloat())
#define CHECK_FLOAT(theFloat) (material->GetFloat(PropertyID::theFloat()) > 0)

using namespace UnityEngine;
namespace Qosmetics::Notes::MaterialUtils
{
    bool ShouldReplaceExtraCC(Material* material)
    {
        if (!material)
            return false;
        else if (static_cast<std::u16string_view>(material->get_name()).find(u"_replace") == std::u16string::npos)
            return false;
        else if (static_cast<std::u16string_view>(material->get_name()).find(u"_noCC") == std::u16string::npos)
            return true;
        return false;
    }

    bool ShouldCC(Material* material)
    {
        if (!material)
            return false;
        else if (HAS_FLOAT(_CustomColors))
            return CHECK_FLOAT(_CustomColors);
        else if (HAS_FLOAT(_Glow))
            return CHECK_FLOAT(_Glow);
        else if (HAS_FLOAT(_Bloom))
            return CHECK_FLOAT(_Bloom);
        return false;
    }

    std::u16string toLower(std::u16string_view in)
    {
        std::u16string result(in);

        for (auto& c : result)
            c = tolower(c);
        return result;
    }

    void ReplaceMaterialsForGameObject(GameObject* object)
    {
        ArrayW<Material*> allMaterials = Resources::FindObjectsOfTypeAll<Material*>();

        for (auto& mat : allMaterials)
        {
            std::u16string materialName = toLower(mat->get_name());
            if (materialName.empty())
                continue;
            if (materialName.find(u"_replace") != std::u16string::npos)
                continue;

            ReplaceMaterialForGameObjectChildren(object, mat, materialName);
        }
    }

    void ReplaceMaterialForGameObjectChildren(GameObject* gameObject, Material* replacingMaterial, std::u16string_view materialToReplaceName)
    {
        ArrayW<Renderer*> renderers = gameObject->GetComponentsInChildren<Renderer*>(true);

        for (auto renderer : renderers)
            ReplaceMaterialForRenderer(renderer, replacingMaterial, materialToReplaceName);
    }

    void ReplaceMaterialForRenderer(Renderer* renderer, Material* replacingMaterial, std::u16string_view materialToReplaceName)
    {
        ArrayW<Material*> materials = renderer->get_materials();

        int length = materials.Length();
        ArrayW<Material*> materialsCopy(length);
        bool materialsDidChange = false;
        for (int i = 0; i < length; i++)
        {
            auto mat = materials[i];
            std::u16string materialName = toLower(mat->get_name());
            // if we find _replace, don't find _done, and find the current name, we should replace a thing
            if (materialName.find(u"_replace") != std::string::npos && materialName.find(u"_done") == std::string::npos && materialName.find(materialToReplaceName) != std::string::npos)
            {
                // now we should be good to update this stuff
                materialName += u"_done";
                if (!ShouldCC(mat) && materialName.find(u"_noCC") == std::string::npos)
                    materialName += u"_noCC";
                // now we have the new material name, as well as being sure that this material is the one that needs to be replaced;
                auto oldColor = mat->get_color();

                DEBUG("Instantiating new material from old material {}", static_cast<std::string>(replacingMaterial->get_name()));
                Material* newMat = UnityEngine::Object::Instantiate(replacingMaterial);
                DEBUG("Old Material: {}, New Material {}", fmt::ptr(replacingMaterial), fmt::ptr(newMat));
                newMat->set_color(oldColor);
                newMat->set_name(materialName);
                materialsCopy[i] = newMat;

                materialsDidChange = true;
            }
            else
            {
                materialsCopy[i] = mat;
            }
        }

        if (materialsDidChange)
        {
            renderer->set_materials(materialsCopy);
        }
    }

}