#include "gbafe.h"

extern CONST_DATA struct NewSupportBonuses gNewAffinityBonuses[];

struct NewSupportBonuses
{
    /* 00 */ u8 affinity;

    /* 01 */ s8 bonusAttack;
    /* 02 */ s8 bonusDefense;
    /* 03 */ s8 bonusHit;
    /* 04 */ s8 bonusAvoid;
    /* 05 */ s8 bonusCrit;
    /* 06 */ s8 bonusDodge;
	/* 07 */ s8 bonusAS;
};

static const struct NewSupportBonuses* GetAffinityBonuses(int affinity)
{
    const struct NewSupportBonuses* it;

    for (it = gNewAffinityBonuses; it->affinity; ++it)
    {
        if (it->affinity == affinity)
            return it;
    }

    // return NULL; // BUG?
}

static void ApplyAffinitySupportBonuses(struct NewSupportBonuses* bonuses, int affinity, int level)
{
    const struct NewSupportBonuses* added = GetAffinityBonuses(affinity);

    bonuses->bonusAttack  += level * added->bonusAttack;
    bonuses->bonusDefense += level * added->bonusDefense;
    bonuses->bonusHit     += level * added->bonusHit;
    bonuses->bonusAvoid   += level * added->bonusAvoid;
    bonuses->bonusCrit    += level * added->bonusCrit;
    bonuses->bonusDodge   += level * added->bonusDodge;
	bonuses->bonusAS	  += level * added->bonusAS;
}

static void NewInitSupportBonuses(struct NewSupportBonuses* bonuses)
{
    bonuses->bonusAttack  = 0;
    bonuses->bonusDefense = 0;
    bonuses->bonusHit     = 0;
    bonuses->bonusAvoid   = 0;
    bonuses->bonusCrit    = 0;
    bonuses->bonusDodge   = 0;
	bonuses->bonusAS	  = 0;
}

int NewGetUnitSupportBonuses(struct Unit* unit, struct NewSupportBonuses* bonuses)
{
    int i, count;
    int result = 0;

    NewInitSupportBonuses(bonuses);

    count = GetUnitSupporterCount(unit);

    for (i = 0; i < count; ++i)
    {
        struct Unit* other;
        int level1, level2;

        result = result >> 1;
        other = GetUnitSupporterUnit(unit, i);

        if (!other)
            continue;

        // TODO: gameStateBits constants
        if (!(gBmSt.gameStateBits & 0x40))
        {
            if (RECT_DISTANCE(unit->xPos, unit->yPos, other->xPos, other->yPos) > SUPPORT_BONUSES_MAX_DISTANCE)
                continue;
        }

        if (other->state & (US_UNAVAILABLE | US_RESCUED))
            continue;

        level1 = GetUnitSupportLevel(other, GetUnitSupporterNum(other, unit->pCharacterData->number));
        ApplyAffinitySupportBonuses(bonuses, other->pCharacterData->affinity, level1);

        level2 = GetUnitSupportLevel(unit, i);
        ApplyAffinitySupportBonuses(bonuses, unit->pCharacterData->affinity, level2);

        if (level1 != 0 && level2 != 0)
            result += 1 << (count - 1);
    }

    bonuses->bonusAttack  /= 2;
    bonuses->bonusDefense /= 2;
    bonuses->bonusHit     /= 2;
    bonuses->bonusAvoid   /= 2;
    bonuses->bonusCrit    /= 2;
    bonuses->bonusDodge   /= 2;
	bonuses->bonusAS	  /= 2;

    return result;
}

void ComputeBattleUnitSupportBonuses(struct BattleUnit* attacker, struct BattleUnit* defender) {
    if (!(gBattleStats.config & BATTLE_CONFIG_ARENA) || gPlaySt.chapterWeatherId) {
        struct NewSupportBonuses tmpBonuses;

        NewGetUnitSupportBonuses(&attacker->unit, &tmpBonuses);

        attacker->battleAttack    += tmpBonuses.bonusAttack;
        attacker->battleDefense   += tmpBonuses.bonusDefense;
        attacker->battleHitRate   += tmpBonuses.bonusHit;
        attacker->battleAvoidRate += tmpBonuses.bonusAvoid;
        attacker->battleCritRate  += tmpBonuses.bonusCrit;
        attacker->battleDodgeRate += tmpBonuses.bonusDodge;
		attacker->battleSpeed	  += tmpBonuses.bonusAS;
    }
}