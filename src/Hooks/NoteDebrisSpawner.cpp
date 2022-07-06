#include "GlobalNamespace/NoteDebrisSpawner.hpp"
#include "GlobalNamespace/BeatmapObjectData.hpp"
#include "GlobalNamespace/ILazyCopyHashSet_1.hpp"
#include "GlobalNamespace/INoteDebrisDidFinishEvent.hpp"
#include "GlobalNamespace/NoteDebris.hpp"
#include "GlobalNamespace/NoteDebris_Pool.hpp"

#include "UnityEngine/Quaternion.hpp"
#include "UnityEngine/Random.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/Vector3.hpp"

#include "sombrero/shared/FastQuaternion.hpp"
#include "sombrero/shared/FastVector3.hpp"
#include "sombrero/shared/MiscUtils.hpp"
#include "sombrero/shared/RandomUtils.hpp"

#include "CustomTypes/DebrisParent.hpp"
#include "hooking.hpp"

static constexpr Sombrero::FastVector3 Cross(const Sombrero::FastVector3& lhs, const Sombrero::FastVector3& rhs)
{
    return Sombrero::FastVector3(lhs.y * rhs.z - lhs.z * rhs.y, lhs.z * rhs.x - lhs.x * rhs.z, lhs.x * rhs.y - lhs.y * rhs.x);
}

// this method is orig because it does not call orig
// it does not call orig because we need access to the spawned notedebris which is not possible in any other way
MAKE_AUTO_HOOK_ORIG_MATCH(NoteDebrisSpawner_SpawnDebris, &GlobalNamespace::NoteDebrisSpawner::SpawnDebris, void, GlobalNamespace::NoteDebrisSpawner* self, ::GlobalNamespace::NoteData::GameplayType noteGameplayType, ::UnityEngine::Vector3 cutPoint, ::UnityEngine::Vector3 cutNormal, float saberSpeed, ::UnityEngine::Vector3 saberDir, ::UnityEngine::Vector3 notePos, ::UnityEngine::Quaternion noteRotation, ::UnityEngine::Vector3 noteScale, ::GlobalNamespace::ColorType colorType, float timeToNextColorNote, ::UnityEngine::Vector3 moveVec)
{
    GlobalNamespace::NoteDebris* noteDebris;
    GlobalNamespace::NoteDebris* noteDebris2;

    self->SpawnNoteDebris(noteGameplayType, byref(noteDebris), byref(noteDebris2));

    if (!noteDebris || !noteDebris2)
    {
        return;
    }

    noteDebris->get_didFinishEvent()->Add(reinterpret_cast<GlobalNamespace::INoteDebrisDidFinishEvent*>(self));
    noteDebris->get_transform()->SetPositionAndRotation(Sombrero::FastVector3::zero(), Sombrero::FastQuaternion::identity());
    noteDebris2->get_didFinishEvent()->Add(reinterpret_cast<GlobalNamespace::INoteDebrisDidFinishEvent*>(self));
    noteDebris2->get_transform()->SetPositionAndRotation(Sombrero::FastVector3::zero(), Sombrero::FastQuaternion::identity());

    float magnitude = Sombrero::sqroot(double(moveVec.x * moveVec.x) + double(moveVec.y * moveVec.y) + double(moveVec.z * moveVec.z));
    float lifeTime = std::clamp(timeToNextColorNote + 0.05f, 0.2f, 2.0f);
    Sombrero::FastVector3 vector = UnityEngine::Vector3::ProjectOnPlane(saberDir, moveVec / magnitude);
    Sombrero::FastVector3 vector2 = vector * (saberSpeed * self->cutDirMultiplier) + moveVec * self->moveSpeedMultiplier;
    if (cutPoint.y < 1.3f)
        vector2.y = std::min(vector2.y, 0.0f);
    else if (cutPoint.y > 1.3f)
        vector2.y = std::min(vector2.y, 0.0f);
    Sombrero::FastQuaternion rotation = self->get_transform()->get_rotation();
    Sombrero::FastVector3 randomOnSphere(Sombrero::RandomFast::randomNumber(0.0f), Sombrero::RandomFast::randomNumber(0.0f), Sombrero::RandomFast::randomNumber(0.0f));
    Sombrero::FastVector3 force = rotation * (-(cutNormal + randomOnSphere * 0.1f) * self->fromCenterSpeed + vector2);
    randomOnSphere.Normalize();
    Sombrero::FastVector3 randomOnSphere2(Sombrero::RandomFast::randomNumber(0.0f), Sombrero::RandomFast::randomNumber(0.0f), Sombrero::RandomFast::randomNumber(0.0f));
    randomOnSphere2.Normalize();
    Sombrero::FastVector3 force2 = rotation * ((cutNormal + randomOnSphere2 * 0.1f) * self->fromCenterSpeed + vector2);
    Sombrero::FastVector3 vector3 = rotation * Cross(cutNormal, vector) * self->rotation / std::max(1.0f, timeToNextColorNote * 2.0f);
    Sombrero::FastVector3 position = self->get_transform()->get_position();
    noteDebris->Init(colorType, notePos, noteRotation, moveVec, noteScale, position, rotation, cutPoint, -cutNormal, force, -vector3, lifeTime);
    noteDebris2->Init(colorType, notePos, noteRotation, moveVec, noteScale, position, rotation, cutPoint, cutNormal, force2, vector3, lifeTime);

    /// if these notedebris are actually our own custom type, do some setting on those
    auto noteDebrisParent = Qosmetics::Notes::DebrisParent::GetDebrisParent(noteDebris);
    auto noteDebrisParent2 = Qosmetics::Notes::DebrisParent::GetDebrisParent(noteDebris2);
    if (noteDebrisParent && noteDebrisParent2)
    {
        noteDebrisParent->SetSliceProperties(colorType, cutPoint, -cutNormal);
        noteDebrisParent2->SetSliceProperties(colorType, cutPoint, cutNormal);
    }
}
