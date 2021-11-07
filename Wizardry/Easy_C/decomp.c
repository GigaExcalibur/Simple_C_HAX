#include "gbafe.h"

int GetBattleUnitExpGain(struct BattleUnit* actor, struct BattleUnit* target) {

    // Copied from src/bmbattle.c in fireemblem8u decomp
    // we are editing this to always return 100 exp (unless no exp can be gained)

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


int GetUnitAid(struct Unit* unit) {

    // copied from src/bmunit.c
    // let's make it less sexist
    
    if (!(UNIT_CATTRIBUTES(unit) & CA_MOUNTEDAID))
        return UNIT_CON(unit) - 1;

    // if (UNIT_CATTRIBUTES(unit) & CA_FEMALE)
        // return 20 - UNIT_CON(unit);
    // else
        return 25 - UNIT_CON(unit);
}

void ComputeBattleUnitHitRate(struct BattleUnit* bu) {

    // copied from src/bmbattle.c
    // changing unit.skl multiplier from 2 to 4

    bu->battleHitRate = (bu->unit.skl * 2) + GetItemHit(bu->weapon) + (bu->unit.lck / 2) + bu->wTriangleHitBonus;
}
