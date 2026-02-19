#include "openal.hpp"

#define OPENAL_HPP_EXTERN
OPENAL_EFX_FUNCTIONS

#define LOADFUNC(type, name, fname)\
    name = (type)alGetProcAddress(fname);\
    if (!name) return false;

bool initEFX()
{
    // effects:

    LOADFUNC(LPALGENEFFECTS, alGenEffects, "alGenEffects");
    LOADFUNC(LPALDELETEEFFECTS, alDeleteEffects, "alDeleteEffects");
    LOADFUNC(LPALISEFFECT, alIsEffect, "alIsEffect");

    LOADFUNC(LPALEFFECTI, alEffecti, "alEffecti");
    LOADFUNC(LPALEFFECTIV, alEffectiv, "alEffectiv");
    LOADFUNC(LPALEFFECTF, alEffectf, "alEffectf");
    LOADFUNC(LPALEFFECTFV, alEffectfv, "alEffectfv");

    LOADFUNC(LPALGETEFFECTI, alGetEffecti, "alGetEffecti");
    LOADFUNC(LPALGETEFFECTIV, alGetEffectiv, "alGetEffectiv");
    LOADFUNC(LPALGETEFFECTF, alGetEffectf, "alGetEffectf");
    LOADFUNC(LPALGETEFFECTFV, alGetEffectfv, "alGetEffectfv");

    // effects slots:

    LOADFUNC(LPALGENAUXILIARYEFFECTSLOTS, alGenAuxiliaryEffectSlots, "alGenAuxiliaryEffectSlots");
    LOADFUNC(LPALDELETEAUXILIARYEFFECTSLOTS, alDeleteAuxiliaryEffectSlots, "alDeleteAuxiliaryEffectSlots");
    LOADFUNC(LPALISAUXILIARYEFFECTSLOT, alIsAuxiliaryEffectSlot, "alIsAuxiliaryEffectSlot");

    LOADFUNC(LPALAUXILIARYEFFECTSLOTI, alAuxiliaryEffectSloti, "alAuxiliaryEffectSloti");
    LOADFUNC(LPALAUXILIARYEFFECTSLOTIV, alAuxiliaryEffectSlotiv, "alAuxiliaryEffectSlotiv");
    LOADFUNC(LPALAUXILIARYEFFECTSLOTF, alAuxiliaryEffectSlotf, "alAuxiliaryEffectSlotf");
    LOADFUNC(LPALAUXILIARYEFFECTSLOTFV, alAuxiliaryEffectSlotfv, "alAuxiliaryEffectSlotfv");

    LOADFUNC(LPALGETAUXILIARYEFFECTSLOTI, alGetAuxiliaryEffectSloti, "alGetAuxiliaryEffectSloti");
    LOADFUNC(LPALGETAUXILIARYEFFECTSLOTIV, alGetAuxiliaryEffectSlotiv, "alGetAuxiliaryEffectSlotiv");
    LOADFUNC(LPALGETAUXILIARYEFFECTSLOTF, alGetAuxiliaryEffectSlotf, "alGetAuxiliaryEffectSlotf");
    LOADFUNC(LPALGETAUXILIARYEFFECTSLOTFV, alGetAuxiliaryEffectSlotfv, "alGetAuxiliaryEffectSlotfv");

    return true;
}