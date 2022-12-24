#include "Installers/AppInstaller.hpp"

#include "CustomTypes/NoteModelContainer.hpp"
#include "Zenject/DiContainer.hpp"
#include "Zenject/FromBinderNonGeneric.hpp"
#include "lapiz/shared/utilities/ZenjectExtensions.hpp"

DEFINE_TYPE(Qosmetics::Notes, AppInstaller);

void Qosmetics::Notes::AppInstaller::InstallBindings()
{
    auto container = get_Container();

    ::Lapiz::Zenject::ZenjectExtensions::FromNewComponentOnNewGameObject(container->BindInterfacesAndSelfTo<Qosmetics::Notes::NoteModelContainer*>())->AsSingle()->NonLazy();
}