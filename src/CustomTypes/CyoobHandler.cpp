#include "CustomTypes/CyoobHandler.hpp"
#include "UnityEngine/Transform.hpp"
#include "logging.hpp"

DEFINE_TYPE(Qosmetics::Notes, CyoobHandler);

namespace Qosmetics::Notes
{
    void CyoobHandler::ctor()
    {
        INVOKE_CTOR();
        previouslyActive = -1;
    }

    void CyoobHandler::ShowNote(bool right, bool dot)
    {
        // bools map to ints and the way they do is crucial here to do the calculation
        ShowNote(right + dot * 2);
    }

    void CyoobHandler::ShowNote(int obj)
    {
        // if we are not changing which one is active, just return
        if (previouslyActive == obj)
        {
            DEBUG("Skipping showing specific note due to it being the same one as already active");
            return;
        }
        // if we have not cached the note objects yet, we should do that
        if (!objects)
            FindNotes();

        // for each of the 4 possibilities, check if they are the correct one
        for (int i = 0; i < 4; i++)
        {
            objects[i]->SetActive(obj == i);
        }

        previouslyActive = obj;
    }

    void CyoobHandler::SetColors(UnityEngine::Color thisColor, UnityEngine::Color thatColor)
    {
        if (previouslyActive != -1)
            colorHandlers[previouslyActive]->SetColors(thisColor, thatColor);
    }

    void CyoobHandler::FindNotes()
    {
        // create array
        if (!objects)
            objects = ArrayW<UnityEngine::GameObject*>(4);
        if (!colorHandlers)
            colorHandlers = ArrayW<CyoobColorHandler*>(4);
        // cache local transform ptr
        auto t = get_transform();

        // find the different objects
        objects[0] = t->Find("LeftArrow")->get_gameObject();
        colorHandlers[0] = objects[0]->GetComponent<CyoobColorHandler*>();
        objects[1] = t->Find("RightArrow")->get_gameObject();
        colorHandlers[1] = objects[1]->GetComponent<CyoobColorHandler*>();
        objects[2] = t->Find("LeftDot")->get_gameObject();
        colorHandlers[2] = objects[2]->GetComponent<CyoobColorHandler*>();
        objects[3] = t->Find("RightDot")->get_gameObject();
        colorHandlers[3] = objects[3]->GetComponent<CyoobColorHandler*>();
    }

}