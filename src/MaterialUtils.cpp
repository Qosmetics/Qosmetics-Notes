#include "MaterialUtils.hpp"
#include "PropertyID.hpp"

#include "UnityEngine/Shader.hpp"

#define CHECK_FLOAT(theFloat) material->HasProperty(PropertyID::theFloat()) && (material->GetFloat(PropertyID::theFloat()) > 0)

namespace Qosmetics::Notes::MaterialUtils
{
    bool ShouldCC(UnityEngine::Material* material)
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
}