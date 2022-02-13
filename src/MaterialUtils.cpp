#include "MaterialUtils.hpp"
#include "PropertyID.hpp"

#include "UnityEngine/Resources.hpp"
#include "UnityEngine/Shader.hpp"

#define CHECK_FLOAT(theFloat) material->HasProperty(PropertyID::theFloat()) && (material->GetFloat(PropertyID::theFloat()) > 0)

using namespace UnityEngine;
namespace Qosmetics::Notes::MaterialUtils
{
    bool ShouldCC(Material* material)
    {
        if (!material)
            return false;
        else if (CHECK_FLOAT(_CustomColors))
            return true;
        else if (CHECK_FLOAT(_Glow))
            return true;
        else if (CHECK_FLOAT(_Bloom))
            return true;
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
            if (materialName.find(u"_replace") != std::u16string::npos)
                continue;

            ReplaceMaterialForGameObjectChildren(object, mat, materialName);
        }
    }

    void ReplaceMaterialForGameObjectChildren(GameObject* gameObject, Material* material, std::u16string_view materialToReplaceName)
    {
        ArrayW<Renderer*> renderers = gameObject->GetComponentsInChildren<Renderer*>(true);

        for (auto renderer : renderers)
            ReplaceMaterialForRenderer(renderer, material, materialToReplaceName);
    }

    void ReplaceMaterialForRenderer(Renderer* renderer, Material* replacingMaterial, std::u16string_view materialToReplaceName)
    {
        ArrayW<Material*> materials = renderer->GetMaterialArray();
        int length = materials.Length();
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

                Material* newMat = Object::Instantiate(replacingMaterial);
                newMat->set_color(oldColor);
                newMat->set_name(materialName);
                materials[i] = newMat;

                materialsDidChange = true;
            }
        }

        if (materialsDidChange)
        {
            renderer->SetMaterialArray(materials);
        }
    }

}