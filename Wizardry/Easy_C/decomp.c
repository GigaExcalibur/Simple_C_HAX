#include "gbafe.h"
#include "variables.h"



void DrawItemMenuLine(struct TextHandle* text, int item, s8 isUsable, u16* mapOut) {
    Text_SetParameters(text, 0, (isUsable ? TEXT_COLOR_NORMAL : TEXT_COLOR_GRAY));
    Text_AppendString(text, GetItemName(item));

    Text_Draw(text, mapOut + 2);
	
	if (!(GetItemAttributes(item) & IA_UNBREAKABLE)) {
		if (ProcFind(0x859BB1C)) {
			DrawDecNumber(mapOut + 11, isUsable ? TEXT_COLOR_BLUE : TEXT_COLOR_GRAY, GetItemUses(item));
		}
		else {
			DrawDecNumber(mapOut + 11, isUsable ? TEXT_COLOR_BLUE : TEXT_COLOR_GRAY, GetItemUses(item));			
		}
	}

    DrawIcon(mapOut, GetItemIconId(item), 0x4000);
}

void DrawItemMenuLineLong(struct TextHandle* text, int item, s8 isUsable, u16* mapOut) {
    Text_SetParameters(text, 0, (isUsable ? TEXT_COLOR_NORMAL : TEXT_COLOR_GRAY));
    Text_AppendString(text, GetItemName(item));

    Text_Draw(text, mapOut + 2);
	
	if (!(GetItemAttributes(item) & IA_UNBREAKABLE)) {
		DrawDecNumber(mapOut + 10, isUsable ? TEXT_COLOR_BLUE : TEXT_COLOR_GRAY, GetItemUses(item));
	}

    //DrawDecNumber(mapOut + 13, isUsable ? TEXT_COLOR_BLUE : TEXT_COLOR_GRAY, GetItemMaxUses(item));
    //sub_8004B0C(mapOut + 11, isUsable ? TEXT_COLOR_NORMAL : TEXT_COLOR_GRAY, 0x16); // draw special character?
    DrawIcon(mapOut, GetItemIconId(item), 0x4000);
	// DrawTextInline(0, BGLoc(BG0Buffer, 2, 3), 3, 13, 2, "  ");
}

void DrawItemMenuLineNoColor(struct TextHandle* text, int item, u16* mapOut) {
    Text_SetXCursor(text, 0);
    Text_AppendString(text, GetItemName(item));

    Text_Draw(text, mapOut + 2);
	
	if (!(GetItemAttributes(item) & IA_UNBREAKABLE)) {
		DrawDecNumber(mapOut + 11, Text_GetColorId(text), GetItemUses(item));
	}
    DrawIcon(mapOut, GetItemIconId(item), 0x4000);
}

void DrawItemStatScreenLine(struct TextHandle* text, int item, int nameColor, u16* mapOut) {
    int color;

    Text_Clear(text);

    color = nameColor;
    Text_SetColorId(text, color);

    Text_AppendString(text, GetItemName(item));

    color = (nameColor != TEXT_COLOR_GRAY) ? TEXT_COLOR_BLUE : TEXT_COLOR_GRAY;
	
	if (!(GetItemAttributes(item) & IA_UNBREAKABLE)) { // Supposed to check whether the item is unbreakable
		//color = (nameColor == TEXT_COLOR_GRAY) ? TEXT_COLOR_GRAY : TEXT_COLOR_NORMAL;
		//sub_8004B0C(mapOut + 12, color, 0x16);
		color = (nameColor != TEXT_COLOR_GRAY) ? TEXT_COLOR_BLUE : TEXT_COLOR_GRAY;
		DrawDecNumber(mapOut + 14, color, GetItemUses(item)); // 11 if not displaying max
		//DrawDecNumber(mapOut + 14, color, GetItemMaxUses(item));
	}
	
	Text_Draw(text, mapOut + 2);
    DrawIcon(mapOut, GetItemIconId(item), 0x4000);
}

extern struct TextHandle* gPrepUnitTexts[];

void DrawPrepScreenItems(u16* tm, struct TextHandle* th, struct Unit* unit, u8 checkPrepUsability) {
    s8 isUsable;
    int i;
    int itemCount;

    TileMap_FillRect(tm, 11, 9, 0);

    itemCount = GetUnitItemCount(unit);

    for (i = 0; i < itemCount; i++) {
        int item = unit->items[i];

        if (checkPrepUsability != 0) {
            isUsable = CanUnitUseItemPrepScreen(unit, item);
        } else {
            isUsable = IsItemDisplayUsable(unit, item);
        }

        Text_Clear(th);
        DrawTextInline(
            th,
            tm + i * 0x40 + 2,
            !isUsable ? TEXT_COLOR_GRAY : TEXT_COLOR_NORMAL,
            0,
            0,
            GetItemName(item)
        );
		if (!(GetItemAttributes(item) & IA_UNBREAKABLE)) {
			DrawUiNumberOrDoubleDashes(tm + i * 0x40 + 0xB, isUsable ? TEXT_COLOR_BLUE : TEXT_COLOR_GRAY, GetItemUses(item));
		}
        DrawIcon(tm + i * 0x40, GetItemIconId(item), 0x4000);

        th++;
    }

    return;
}