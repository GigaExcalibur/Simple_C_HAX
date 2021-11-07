#include "gbafe.h"

int GetBattleUnitExpGain(struct BattleUnit* actor, struct BattleUnit* target) {
    int result;

    if (!CanBattleUnitGainLevels(actor) || (actor->unit.curHP == 0) || UNIT_CATTRIBUTES(&target->unit) & CA_NEGATE_LETHALITY)
        return 0;

    // if (!actor->nonZeroDamage)
    //     return 1;

    // result = GetUnitRoundExp(&actor->unit, &target->unit);
    // result += GetUnitKillExpBonus(&actor->unit, &target->unit);

    // if (result > 100)
    //     result = 100;

    // if (result < 1)
    //     result = 1;

    // ModifyUnitSpecialExp(&actor->unit, &target->unit, &result);
    result = 100;

    return result;
}

static u16 gRNSeeds[3];
static int gLCGRNValue;

int NextRN(void) {
    // This generates a pseudorandom string of 16 bits
    // In other words, a pseudorandom integer that can range from 0 to 65535

    // u16 rn = (gRNSeeds[1] << 11) + (gRNSeeds[0] >> 5);

    // // Shift state[2] one bit
    // gRNSeeds[2] *= 2;

    // // "carry" the top bit of state[1] to state[2]
    // if (gRNSeeds[1] & 0x8000)
    //     gRNSeeds[2]++;

    // rn ^= gRNSeeds[2];

    // // Shifting the whole state 16 bits
    // gRNSeeds[2] = gRNSeeds[1];
    // gRNSeeds[1] = gRNSeeds[0];
    // gRNSeeds[0] = rn;

    return 0;
}

void MakeTargetListForAdjacentHeal(struct Unit* unit);
void MakeTargetListForRangedHeal(struct Unit* unit);
void MakeTargetListForRestore(struct Unit* unit);
void MakeTargetListForRescueStaff(struct Unit* unit);
void MakeTargetListForBarrier(struct Unit* unit);
void MakeTargetListForSilence(struct Unit* unit);
void MakeTargetListForSleep(struct Unit* unit);
void MakeTargetListForBerserk(struct Unit* unit);
void MakeTargetListForWarp(struct Unit* unit);
void MakeTargetListForHammerne(struct Unit* unit);
void MakeTargetListForUnlock(struct Unit* unit);
void MakeTargetListForLatona(struct Unit* unit);
void MakeTargetListForMine(struct Unit* unit);
void MakeTargetListForLightRune(struct Unit* unit);
void MakeTargetListForDanceRing(struct Unit* unit);
void MakeTargetListForDoorAndBridges(struct Unit* unit, int terrain);

s8 HasSelectTarget(struct Unit* unit, void(*func)(struct Unit*))
{
    func(unit);

    return sub_804FD28() != 0;
}

s8 CanUnitUseItem(struct Unit* unit, int item)
{
    if ((GetItemAttributes(item) & IA_STAFF) && !CanUnitUseStaff(unit, item))
        return FALSE;

    switch (GetItemIndex(item))
    {

    case ITEM_STAFF_HEAL:
    case ITEM_STAFF_MEND:
    case ITEM_STAFF_RECOVER:
        return HasSelectTarget(unit, MakeTargetListForRangedHeal);
    // return FALSE;

    case ITEM_STAFF_PHYSIC:
        return HasSelectTarget(unit, MakeTargetListForRangedHeal);

    case ITEM_STAFF_FORTIFY:
        return HasSelectTarget(unit, MakeTargetListForRangedHeal);

    case ITEM_STAFF_RESTORE:
        return HasSelectTarget(unit, MakeTargetListForRestore);

    case ITEM_STAFF_RESCUE:
        return HasSelectTarget(unit, MakeTargetListForRescueStaff);

    case ITEM_STAFF_BARRIER:
        return HasSelectTarget(unit, MakeTargetListForBarrier);

    case ITEM_STAFF_SILENCE:
        return HasSelectTarget(unit, MakeTargetListForSilence);

    case ITEM_STAFF_SLEEP:
        return HasSelectTarget(unit, MakeTargetListForSleep);

    case ITEM_STAFF_BERSERK:
        return HasSelectTarget(unit, MakeTargetListForBerserk);

    case ITEM_STAFF_WARP:
        return HasSelectTarget(unit, MakeTargetListForWarp);

    case ITEM_STAFF_REPAIR:
        return HasSelectTarget(unit, MakeTargetListForHammerne);

    case ITEM_STAFF_UNLOCK:
        return HasSelectTarget(unit, MakeTargetListForUnlock);

    case ITEM_BOOSTER_HP:
    case ITEM_BOOSTER_POW:
    case ITEM_BOOSTER_SKL:
    case ITEM_BOOSTER_SPD:
    case ITEM_BOOSTER_LCK:
    case ITEM_BOOSTER_DEF:
    case ITEM_BOOSTER_RES:
    case ITEM_BOOSTER_MOV:
    case ITEM_BOOSTER_CON:
        return CanUnitUseStatGainItem(unit, item);

    case ITEM_HEROCREST:
    case ITEM_KNIGHTCREST:
    case ITEM_ORIONSBOLT:
    case ITEM_ELYSIANWHIP:
    case ITEM_GUIDINGRING:
    case ITEM_MASTERSEAL:
    case ITEM_HEAVENSEAL:
    case ITEM_OCEANSEAL:
    case ITEM_LUNARBRACE:
    case ITEM_SOLARBRACE:
    case ITEM_UNK_C1:
        return CanUnitUsePromotionItem(unit, item);

    case ITEM_VULNERARY:
    case ITEM_ELIXIR:
    case ITEM_VULNERARY_2:
        return CanUnitUseHealItem(unit);

    case ITEM_PUREWATER:
        return CanUnitUsePureWaterItem(unit);

    case ITEM_TORCH:
        return CanUnitUseTorchItem(unit);

    case ITEM_ANTITOXIN:
        return CanUnitUseAntitoxinItem(unit);

    case ITEM_CHESTKEY:
    case ITEM_CHESTKEY_BUNDLE:
        return CanUnitUseChestKeyItem(unit);

    case ITEM_DOORKEY:
        return CanUnitUseDoorKeyItem(unit);

    case ITEM_LOCKPICK:
        return CanUnitUseLockpickItem(unit);

    case ITEM_STAFF_LATONA:
        return HasSelectTarget(unit, MakeTargetListForLatona);

    case ITEM_MINE:
        return HasSelectTarget(unit, MakeTargetListForMine);

    case ITEM_LIGHTRUNE:
        return HasSelectTarget(unit, MakeTargetListForLightRune);

    case ITEM_STAFF_TORCH:
        return gRAMChapterData.chapterVisionRange != 0;

    case ITEM_FILLAS_MIGHT:
    case ITEM_NINISS_GRACE:
    case ITEM_THORS_IRE:
    case ITEM_SETS_LITANY:
        return HasSelectTarget(unit, MakeTargetListForDanceRing);

    case ITEM_METISSTOME:
        if (unit->state & US_GROWTH_BOOST)
            return FALSE;

        return TRUE;

    case ITEM_JUNAFRUIT:
        return CanUnitUseFruitItem(unit);

    default:
        return FALSE;

    }
}
