#include <gbafe.h>

int GetBattleUnitExpGain(BattleUnit_t* actor, BattleUnit_t* target) {
    // int result;

    if (!CanBattleUnitGainLevels(actor) || (actor->unit.curHP == 0) || UNIT_ATTRIBUTES(&target->unit) & UNIT_ATTR_FINAL_BOSS)
        return 0;

    return 100;

    // if (!actor->took_damage)
    //     return 1;

    // result = GetUnitRoundExp(&actor->unit, &target->unit);
    // result += GetUnitKillExpBonus(&actor->unit, &target->unit);

    // if (result > 100)
    //     result = 100;

    // if (result < 1)
    //     result = 1;

    // ModifyUnitSpecialExp(&actor->unit, &target->unit, &result);

    // return result;
}
