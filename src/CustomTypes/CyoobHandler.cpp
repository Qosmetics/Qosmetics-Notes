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
    }

    void CyoobHandler::FindNotes()
    {
        // create array
        if (!objects)
            objects = ArrayW<UnityEngine::GameObject*>(4);
        // cache local transform ptr
        auto t = get_transform();

        // find the different objects
        objects[0] = t->Find("LeftArrow")->get_gameObject();
        objects[1] = t->Find("RightArrow")->get_gameObject();
        objects[2] = t->Find("LeftDot")->get_gameObject();
        objects[3] = t->Find("RightDot")->get_gameObject();
    }

}