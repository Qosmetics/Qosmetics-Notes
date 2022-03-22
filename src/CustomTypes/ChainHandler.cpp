#include "CustomTypes/ChainHandler.hpp"
#include "ConstStrings.hpp"
#include "UnityEngine/Transform.hpp"
#include "logging.hpp"

DEFINE_TYPE(Qosmetics::Notes, ChainHandler);

namespace Qosmetics::Notes
{
    void ChainHandler::ctor()
    {
        INVOKE_CTOR();
        previouslyActive = -1;
    }

    void ChainHandler::ShowNote(bool right, bool head)
    {
        ShowNote(right + (!head) * 2);
    }

    void ChainHandler::ShowNote(int obj)
    {
        if (previouslyActive == obj)
        {
            DEBUG("Skipping showing specific note due to it being the same one as already active");
            return;
        }

        if (!objects)
            FindNotes();

        for (int i = 0; i < 4; i++)
        {
            objects[i]->SetActive(obj == i);
        }

        previouslyActive = obj;
    }

    void ChainHandler::SetColors(UnityEngine::Color thisColor, UnityEngine::Color thatColor)
    {
        if (!colorHandlers)
            FindNotes();

        if (previouslyActive != -1)
            colorHandlers[previouslyActive]->SetColors(thisColor, thatColor);
    }

    void ChainHandler::FindNotes()
    {
        // create array
        if (!objects)
        {
            // cache local transform ptr
            auto t = get_transform();
            // find the different objects
            objects = ArrayW<UnityEngine::GameObject*>(4);
            objects[0] = t->Find(ConstStrings::LeftHead())->get_gameObject();
            objects[1] = t->Find(ConstStrings::RightHead())->get_gameObject();
            objects[2] = t->Find(ConstStrings::LeftLink())->get_gameObject();
            objects[3] = t->Find(ConstStrings::RightLink())->get_gameObject();
        }

        if (!colorHandlers)
        {
            colorHandlers = ArrayW<CyoobColorHandler*>(4);
            colorHandlers[0] = objects[0]->GetComponent<CyoobColorHandler*>();
            colorHandlers[1] = objects[1]->GetComponent<CyoobColorHandler*>();
            colorHandlers[2] = objects[2]->GetComponent<CyoobColorHandler*>();
            colorHandlers[3] = objects[3]->GetComponent<CyoobColorHandler*>();
        }
    }

}