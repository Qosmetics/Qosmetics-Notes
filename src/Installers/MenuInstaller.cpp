#include "Installers/MenuInstaller.hpp"

#include "UI/CyoobFlowCoordinator.hpp"
#include "UI/PreviewViewController.hpp"
#include "UI/SelectionViewController.hpp"
#include "UI/SettingsViewController.hpp"

#include "lapiz/shared/arrayutils.hpp"
#include "lapiz/shared/utilities/ZenjectExtensions.hpp"

#include "Zenject/ConcreteIdBinderGeneric_1.hpp"
#include "Zenject/DiContainer.hpp"
#include "Zenject/FromBinderNonGeneric.hpp"

DEFINE_TYPE(Qosmetics::Notes, MenuInstaller);

using namespace ::Lapiz::Zenject::ZenjectExtensions;

void Qosmetics::Notes::MenuInstaller::InstallBindings()
{
    auto container = get_Container();

    FromNewComponentOnNewGameObject(container->Bind<Qosmetics::Core::QosmeticsBaseFlowCoordinator*>()->To(Lapiz::ArrayUtils::TypeArray<Qosmetics::Notes::CyoobFlowCoordinator*>()))->AsSingle();
    FromNewComponentAsViewController(container->BindInterfacesAndSelfTo<Qosmetics::Notes::PreviewViewController*>())->AsSingle();
    FromNewComponentAsViewController(container->BindInterfacesAndSelfTo<Qosmetics::Notes::SettingsViewController*>())->AsSingle();
    FromNewComponentAsViewController(container->BindInterfacesAndSelfTo<Qosmetics::Notes::SelectionViewController*>())->AsSingle();
}