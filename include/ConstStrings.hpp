#pragma once
#include "beatsaber-hook/shared/utils/typedefs.h"

#ifndef CONST_STRING
#define CONST_STRING(identifier)                    \
    static StringW identifier()                     \
    {                                               \
        static ConstString identifier(#identifier); \
        return identifier;                          \
    }
#endif

namespace ConstStrings
{
    CONST_STRING(NoteArrow);
    CONST_STRING(NoteCircleGlow);
    CONST_STRING(NoteArrowGlow);
    CONST_STRING(LeftArrow);
    CONST_STRING(RightArrow);
    CONST_STRING(LeftDot);
    CONST_STRING(RightDot);
    CONST_STRING(Debris);
    CONST_STRING(LeftDebris);
    CONST_STRING(RightDebris);
    CONST_STRING(Notes);
    CONST_STRING(MirrorNotes);
    CONST_STRING(Bomb);
    CONST_STRING(MirrorBomb);
}