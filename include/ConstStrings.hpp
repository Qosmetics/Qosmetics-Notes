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
    CONST_STRING(NoteCube);
    CONST_STRING(NoteArrow);
    CONST_STRING(NoteCircleGlow);
    CONST_STRING(NoteArrowGlow);
    CONST_STRING(Circle);
    CONST_STRING(LeftArrow);
    CONST_STRING(RightArrow);
    CONST_STRING(LeftDot);
    CONST_STRING(RightDot);
    CONST_STRING(Debris);
    CONST_STRING(ChainHeadDebris);
    CONST_STRING(ChainLinkDebris);
    CONST_STRING(LeftDebris);
    CONST_STRING(RightDebris);
    CONST_STRING(Notes);
    CONST_STRING(MirrorNotes);
    CONST_STRING(Bomb);
    CONST_STRING(MirrorBomb);
    CONST_STRING(BigCuttable);
    CONST_STRING(SmallCuttable);
    CONST_STRING(Chains);
    CONST_STRING(MirrorChains);
    CONST_STRING(LeftHead);
    CONST_STRING(RightHead);
    CONST_STRING(LeftLink);
    CONST_STRING(LeftLinkCopy1);
    CONST_STRING(LeftLinkCopy2);
    CONST_STRING(RightLink);
    CONST_STRING(RightLinkCopy1);
    CONST_STRING(RightLinkCopy2);
    CONST_STRING(Mesh);

}