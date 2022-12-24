#pragma once

#include "CustomTypes/NoteModelContainer.hpp"
#include "GlobalNamespace/BombNoteController.hpp"
#include "GlobalNamespace/BurstSliderGameNoteController.hpp"
#include "GlobalNamespace/GameNoteController.hpp"
#include "GlobalNamespace/GameplayCoreSceneSetupData.hpp"
#include "GlobalNamespace/MirroredBombNoteController.hpp"
#include "GlobalNamespace/MirroredGameNoteController.hpp"
#include "GlobalNamespace/MirroredSliderController.hpp"
#include "GlobalNamespace/NoteDebris.hpp"

#include "Zenject/Installer.hpp"
#include "custom-types/shared/macros.hpp"
#include "lapiz/shared/macros.hpp"

#include "private_field.hpp"

DECLARE_CLASS_CODEGEN(Qosmetics::Notes, GameInstaller, ::Zenject::Installer,
                      DECLARE_INSTANCE_PRIVATE_FIELD(NoteModelContainer*, _noteModelContainer);
                      DECLARE_INSTANCE_PRIVATE_FIELD(GlobalNamespace::GameplayCoreSceneSetupData*, _gameplayCoreSceneSetupData);
                      DECLARE_INSTANCE_PRIVATE_FIELD(bool, _ghostNotes);
                      DECLARE_INSTANCE_PRIVATE_FIELD(bool, _disappearingArrows);
                      DECLARE_INSTANCE_PRIVATE_FIELD(bool, _smallCubes);
                      DECLARE_INSTANCE_PRIVATE_FIELD(bool, _proMode);
                      DECLARE_INSTANCE_PRIVATE_FIELD(bool, _reduceDebris);
                      DECLARE_PRIVATE_METHOD(GlobalNamespace::GameNoteController*, DecorateNote, GlobalNamespace::GameNoteController* original);
                      DECLARE_PRIVATE_METHOD(GlobalNamespace::BombNoteController*, DecorateBombs, GlobalNamespace::BombNoteController* original);
                      DECLARE_PRIVATE_METHOD(GlobalNamespace::BurstSliderGameNoteController*, DecorateBurstSliderElement, GlobalNamespace::BurstSliderGameNoteController* original);
                      DECLARE_PRIVATE_METHOD(GlobalNamespace::GameNoteController*, DecorateBurstSliderHead, GlobalNamespace::GameNoteController* original);
                      DECLARE_PRIVATE_METHOD(GlobalNamespace::MirroredGameNoteController*, DecorateMirrorNote, GlobalNamespace::MirroredGameNoteController* original);
                      DECLARE_PRIVATE_METHOD(GlobalNamespace::MirroredGameNoteController*, DecorateMirrorBurstSliderHead, GlobalNamespace::MirroredGameNoteController* original);
                      DECLARE_PRIVATE_METHOD(GlobalNamespace::MirroredGameNoteController*, DecorateMirrorBurstSliderElement, GlobalNamespace::MirroredGameNoteController* original);
                      DECLARE_PRIVATE_METHOD(GlobalNamespace::MirroredBombNoteController*, DecorateMirrorBomb, GlobalNamespace::MirroredBombNoteController* original);

                      DECLARE_PRIVATE_METHOD(GlobalNamespace::NoteDebris*, DecorateNoteDebris, GlobalNamespace::NoteDebris* original);
                      DECLARE_PRIVATE_METHOD(GlobalNamespace::NoteDebris*, DecorateHeadNoteDebris, GlobalNamespace::NoteDebris* original);
                      DECLARE_PRIVATE_METHOD(GlobalNamespace::NoteDebris*, DecorateLinkNoteDebris, GlobalNamespace::NoteDebris* original);
                      DECLARE_PRIVATE_METHOD(GlobalNamespace::NoteDebris*, GeneralDecorateDebris, GlobalNamespace::NoteDebris* original, StringW targetName, bool useDebris, bool forceDefaultDebris);

                      DECLARE_OVERRIDE_METHOD(void, InstallBindings, il2cpp_utils::il2cpp_type_check::MetadataGetter<&::Zenject::Installer::InstallBindings>::get());
                      DECLARE_CTOR(ctor, NoteModelContainer* noteModelContainer, GlobalNamespace::GameplayCoreSceneSetupData* gameplayCoreSceneSetupData);

                      static constexpr const int DECORATION_PRIORITY = 300;

)