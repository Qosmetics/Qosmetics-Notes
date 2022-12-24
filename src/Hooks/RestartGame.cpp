#include "CustomTypes/NoteModelContainer.hpp"
#include "hooking.hpp"
#include "logging.hpp"

#include "GlobalNamespace/MenuTransitionsHelper.hpp"
#include "System/Action_1.hpp"
#include "Zenject/DiContainer.hpp"
/*
#include "beatsaber-hook/shared/utils/accessor-wrapper-types.hpp"
#include "beatsaber-hook/shared/utils/base-wrapper-type.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-utils-exceptions.hpp"

namespace System
{
    struct IntPtrW : public ::System::IntPtr
    {
        IntPtrW* operator->() { return this; }
        const IntPtrW* operator->() const { return this; }
    };
}
DEFINE_IL2CPP_ARG_TYPE(::System::IntPtrW, "System", "IntPtr");

namespace UnityEngine
{
    struct ObjectW : public ::bs_hook::Il2CppWrapperType
    {
        constexpr explicit ObjectW(void* ptr) noexcept : ::bs_hook::Il2CppWrapperType(ptr) {}
        bs_hook::InstanceField<::System::IntPtrW, 0x10> m_CachedPtr{instance};
        bs_hook::InstanceProperty<"name", ::StringW, true, true> name{instance};
        bs_hook::InstanceProperty<"hideFlags", ::UnityEngine::HideFlags, true, true> hideFlags{instance};
    };
    static_assert(il2cpp_utils::has_il2cpp_conversion<::UnityEngine::ObjectW>);

    struct ComponentW : public ::UnityEngine::ObjectW
    {
        constexpr explicit ComponentW(void* ptr) noexcept : ::UnityEngine::ObjectW(ptr) {}
    };
    static_assert(il2cpp_utils::has_il2cpp_conversion<::UnityEngine::ComponentW>);

    struct BehaviourW : public ComponentW
    {
        bs_hook::InstanceProperty<"enabled", bool, true, true> enabled{instance};
        constexpr explicit BehaviourW(void* ptr) noexcept : ComponentW(ptr) {}
    };
    static_assert(il2cpp_utils::has_il2cpp_conversion<::UnityEngine::BehaviourW>);

    struct MonoBehaviourW : public BehaviourW
    {
        constexpr explicit MonoBehaviourW(void* ptr) noexcept : BehaviourW(ptr) {}
    };
    static_assert(il2cpp_utils::has_il2cpp_conversion<::UnityEngine::MonoBehaviourW>);
}

NEED_NO_BOX(::UnityEngine::ObjectW);
DEFINE_IL2CPP_ARG_TYPE(::UnityEngine::ObjectW, "UnityEngine", "Object");
NEED_NO_BOX(::UnityEngine::ComponentW);
DEFINE_IL2CPP_ARG_TYPE(::UnityEngine::ComponentW, "UnityEngine", "Component");
NEED_NO_BOX(::UnityEngine::BehaviourW);
DEFINE_IL2CPP_ARG_TYPE(::UnityEngine::BehaviourW, "UnityEngine", "Behaviour");
NEED_NO_BOX(::UnityEngine::MonoBehaviourW);
DEFINE_IL2CPP_ARG_TYPE(::UnityEngine::MonoBehaviourW, "UnityEngine", "MonoBehaviour");

namespace GlobalNamespace
{
    struct MenuTransitionsHelperW : public ::UnityEngine::MonoBehaviourW
    {
        constexpr explicit MenuTransitionsHelperW(void* ptr) noexcept : MonoBehaviourW(ptr){};
    };
    static_assert(il2cpp_utils::has_il2cpp_conversion<::GlobalNamespace::MenuTransitionsHelperW>);
}

NEED_NO_BOX(::GlobalNamespace::MenuTransitionsHelperW);
DEFINE_IL2CPP_ARG_TYPE(::GlobalNamespace::MenuTransitionsHelperW, "", "MenuTransitionsHelperW");
MAKE_AUTO_HOOK_WRAPPER(MenuTransitionsHelper_RestartGame, &GlobalNamespace::MenuTransitionsHelper::RestartGame, void, GlobalNamespace::MenuTransitionsHelperW self, System::Action_1<Zenject::DiContainer*>* finishCallback)
{
    DEBUG("Hook Called: {}", fmt::ptr(self.convert()));
    auto obj = (Il2CppObject*)self.convert();
    DEBUG("self type: '{}::{}'", obj->klass->namespaze, obj->klass->name);
    DEBUG("As Wrapper:");
    auto name = *self.name;
    DEBUG("name: {}", name);
    DEBUG("enabled: {}", *self.enabled);
    DEBUG("value: {}", self.m_CachedPtr->m_value);
    DEBUG("As Codegen:");
    auto selfC = (GlobalNamespace::MenuTransitionsHelper*)self.convert();
    DEBUG("name: {}", selfC->get_name());
    DEBUG("enabled: {}", selfC->get_enabled());
    DEBUG("value: {}", selfC->m_CachedPtr.m_value);

    // Qosmetics::Notes::NoteModelContainer::get_instance()->OnGameRestart();
    MenuTransitionsHelper_RestartGame(self, finishCallback);
}
*/
