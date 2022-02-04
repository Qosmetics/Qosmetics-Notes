#include "MaterialUtils.hpp"

#include "UnityEngine/Shader.hpp"

#define PROPERTY_ID(identifier)                                                 \
    static int identifier()                                                     \
    {                                                                           \
        static int identifier = UnityEngine::Shader::PropertyToID(#identifier); \
        return identifier;                                                      \
    }

namespace PropertyID
{
    PROPERTY_ID(_CustomColors);
    PROPERTY_ID(_Glow);
    PROPERTY_ID(_Bloom);
}

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