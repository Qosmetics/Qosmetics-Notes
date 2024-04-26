#include "CustomTypes/DebrisHandler.hpp"
#include "PropertyID.hpp"
#include "sombrero/shared/MiscUtils.hpp"

#include "UnityEngine/Shader.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/Vector4.hpp"


DEFINE_TYPE(Qosmetics::Notes, DebrisHandler);

ArrayW<Qosmetics::Notes::DebrisHandler::RendererMaterialPair> GetSlicableMaterials(UnityEngine::GameObject* go)
{
    std::vector<Qosmetics::Notes::DebrisHandler::RendererMaterialPair> result;
    auto renderers = go->GetComponentsInChildren<UnityEngine::Renderer*>(true);

    for (auto renderer : renderers)
    {
        auto materials = renderer->get_materials();
        for (auto material : materials)
        {
            if (material->HasProperty(PropertyID::_SlicePos()) &&
                material->HasProperty(PropertyID::_CutPlane()) &&
                material->HasProperty(PropertyID::_TransformOffset()))
                result.emplace_back(renderer, material);
        }
    }

    return il2cpp_utils::vectorToArray(result);
}

namespace Qosmetics::Notes
{
    void DebrisHandler::Awake()
    {
        previouslyActive = -1;

        objects = ArrayW<UnityEngine::GameObject*>(2);
        colorHandlers = ArrayW<DebrisColorHandler*>(2);
        sliceMaterials = ArrayW<ArrayW<RendererMaterialPair>>(2);
        anySliceMaterials = ArrayW<bool>(2);

        FindObjects();
    }

    void DebrisHandler::SetColors(Sombrero::FastColor thisColor, Sombrero::FastColor thatColor)
    {
        if (previouslyActive != -1)
            colorHandlers[previouslyActive]->SetColors(thisColor, thatColor);
    }

    void DebrisHandler::ShowDebris(bool right)
    {
        if (previouslyActive == right)
            return;
        previouslyActive = right;
        objects[0]->SetActive(!right);
        objects[1]->SetActive(right);
    }

    void DebrisHandler::SetSliceProperties(Sombrero::FastVector3 cutPoint, Sombrero::FastVector3 cutNormal)
    {
        // if same values or there are no slice materials, return
        if (!anySliceMaterials[previouslyActive] || (lastCutPoint.operator==(cutPoint) && lastCutNormal.operator==(cutNormal)))
            return;

        float magnitude = cutPoint.Magnitude();

        if (magnitude > 0.04f)
        {
            cutPoint = (cutPoint * 0.2f) / Sombrero::sqroot(magnitude);
        }

        UnityEngine::Vector4 slicePos(cutPoint.x, cutPoint.y, cutPoint.z, 0.0f);
        UnityEngine::Vector4 cutPlane(cutNormal.x, cutNormal.y, cutNormal.z, 0.0f);

        UnityEngine::Renderer* renderer = nullptr;
        UnityEngine::Vector3 localPosition;
        UnityEngine::Vector4 transformOffset;
        for (auto rendererMaterialPair : sliceMaterials[previouslyActive])
        {
            auto pairRenderer = rendererMaterialPair.get_Key();
            if (pairRenderer != renderer)
            {
                renderer = pairRenderer;
                renderer->set_sortingLayerID(9);
                localPosition = renderer->get_transform()->get_localPosition();
                transformOffset = UnityEngine::Vector4(localPosition.x, localPosition.y, localPosition.z, 0.0f);
            }
            auto material = rendererMaterialPair.get_Value();

            material->SetVector(PropertyID::_SlicePos(), slicePos);
            material->SetVector(PropertyID::_CutPlane(), cutPlane);
            material->SetVector(PropertyID::_TransformOffset(), transformOffset);
        }
    }

    void DebrisHandler::FindObjects()
    {
        objects[0] = get_transform()->Find("LeftDebris")->get_gameObject();
        colorHandlers[0] = objects[0]->GetComponent<DebrisColorHandler*>();
        sliceMaterials[0] = GetSlicableMaterials(objects[0]);
        anySliceMaterials[0] = sliceMaterials[0].size() > 0;

        objects[1] = get_transform()->Find("RightDebris")->get_gameObject();
        colorHandlers[1] = objects[1]->GetComponent<DebrisColorHandler*>();
        sliceMaterials[1] = GetSlicableMaterials(objects[1]);
        anySliceMaterials[1] = sliceMaterials[1].size() > 0;
    }

}
