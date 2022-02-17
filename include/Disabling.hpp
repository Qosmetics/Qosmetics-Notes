#pragma once
#include "modloader/shared/modloader.hpp"

namespace Qosmetics::Notes::Disabling
{
    void RegisterDisablingModInfo(ModInfo info);
    void UnregisterDisablingModInfo(ModInfo info);

    bool GetAnyDisabling();
}