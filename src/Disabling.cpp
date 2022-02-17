#include "Disabling.hpp"
#include "logging.hpp"
#include <set>

namespace Qosmetics::Notes::Disabling
{
    std::set<ModInfo> disablingModInfos = {};

    void RegisterDisablingModInfo(ModInfo info)
    {
        auto disablingModInfoItr = disablingModInfos.find(info);
        if (disablingModInfoItr != disablingModInfos.end())
        {
            INFO("Mod {} tried to register disabling mod info more than once!", info.id);
        }
        else
        {
            disablingModInfos.insert(info);
        }
    }

    void UnregisterDisablingModInfo(ModInfo info)
    {
        auto disablingModInfoItr = disablingModInfos.find(info);
        if (disablingModInfoItr != disablingModInfos.end())
        {
            disablingModInfos.erase(disablingModInfoItr);
        }
        else
        {
            INFO("Mod {} tried to remove their disabling mod info more than once!", info.id);
        }
    }

    bool GetAnyDisabling()
    {
        return disablingModInfos.size() > 0;
    }
}