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
