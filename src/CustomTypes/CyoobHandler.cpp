#include "CustomTypes/CyoobHandler.hpp"
#include "UnityEngine/Transform.hpp"

DEFINE_TYPE(Qosmetics::Notes, CyoobHandler);

namespace Qosmetics::Notes
{
    void CyoobHandler::ShowNote(bool right, bool dot)
    {
        ShowNote(right + dot * 2);
    }

    void CyoobHandler::ShowNote(int obj)
    {
        if (!objects.convert())
            FindNotes();

        for (int i = 0; i < 4; i++)
        {
            objects[i]->SetActive(obj == i);
        }
    }

    void CyoobHandler::FindNotes()
    {
        if (!objects.convert())
            objects = ArrayW<UnityEngine::GameObject*>(4);
        auto t = get_transform();

        objects[0] = t->Find("LeftArrow")->get_gameObject();
        objects[1] = t->Find("RightArrow")->get_gameObject();
        objects[2] = t->Find("LeftDot")->get_gameObject();
        objects[3] = t->Find("RightDot")->get_gameObject();
    }

}