#include "KirbDebug.h"

// FIXME: Add to fe8u.s
static const void(*DisplayHandCursor)(int x, int y) = (void(*)(int, int))(0x804E848+1);
static const void(*DrawMenuItemHighlight)() = (void(*)(int bgIndex, int, int x, int y, int width))(0x804E98C+1);
static const void(*ClearMenuItemHighlight)() = (void(*)(int bgIndex, int, int x, int y, int width))(0x804EA08+1);

// FIXME: Add to libgbafe
void MakeUIWindowTileMap_BG0BG1(int x, int y, int width, int height, int style) __attribute__((long_call));
void LoadNewUIPal(int palIndex) __attribute__((long_call));

void StartFadeInBlack(int speed, struct Proc* parent) __attribute__((long_call)); //! FE8U = 0x8013D09
void NewFadeOutBlack(int speed, struct Proc* parent) __attribute__((long_call)); //! FE8U = 0x8013D21

void ReloadGameCoreGraphics(void) __attribute__((long_call));

void EndBG3Slider(void) __attribute__((long_call));

extern u8 gGfx_ChapterIntroBackground[];
extern u16 gPal_ChapterIntroBackground[];

enum { KD_PAGE_COUNT = 2 };

/*
struct KDPageDefinition {
	void (*onInit) (UnitEditorProc*);
	void (*onEnd) (UnitEditorProc*);
	void (*onSubjectChange) (UnitEditorProc*);
	void (*onIdle) (UnitEditorProc*);
};

static struct KDPageDefinition sKD_PageDefinitions[] = {
	{
		// page 1

		.onInit          = PrintConstantsPage1,
		.onSubjectChange = SetupDebugUnitEditorPage1,
		.onIdle          = UpdateDebugUnitEditorPage1,
	}, {
		// page 2

		.onInit          = PrintConstantsPage2,
		.onSubjectChange = SetupDebugUnitEditorPage2,
		.onIdle          = UpdateDebugUnitEditorPage2,
	}
}; //*/

static const struct KDPageDefinition* sKDPageDefinitions[] = {
	&gKDStatPageDefinition,

	NULL
};

u8 KribDebugMenuEffect(void) {
	StartProc(Debug6C, ROOT_PROC_3);
	return ME_END | ME_PLAY_BEEP | ME_CLEAR_GFX;
}

void DebugScreenSetup(UnitEditorProc* proc) {
	enum {
		PALBG_BACKGROUND = 14,

		PALOBJ_FONT = 14,
	};

	LoadBgConfig(NULL);

	FillBgMap(gBg0MapBuffer, 0);
	FillBgMap(gBg1MapBuffer, 0);

	//Load the background graphics and palette and tsa(generate it)
	Decompress(gGfx_ChapterIntroBackground, (void*)(VRAM + 0x8000));
	CopyToPaletteBuffer(gPal_ChapterIntroBackground, PALBG_BACKGROUND * 0x20, 0x20);

	GenerateBGTsa(gBg3MapBuffer, 0x280, PALBG_BACKGROUND);

	CopyToPaletteBuffer(gPal_MiscUIGraphics, 16 * 0x20, 0x20);

	SetupDebugFontForBG(2, 0);
	SetupDebugFontForOBJ(-1, PALOBJ_FONT);
	Font_InitDefault();

	//Setup the unit editor™
	proc->pUnit     = NULL;
	proc->pPageProc = NULL;

	KDSwitchUnit(proc, GetUnit(1));
	KDSwitchPage(proc, 0);
}

void DebugScreenLoop(UnitEditorProc* proc) {
	if ((gKeyStatus.pressedKeys & B_BUTTON) && proc->KonamiCodeCounter != 10) {
		// let page save unit
		if (proc->pPageProc && sKDPageDefinitions[proc->PageIndex]->onSaveUnit)
			sKDPageDefinitions[proc->PageIndex]->onSaveUnit(proc->pPageProc, proc, proc->pUnit);

		BreakProcLoop((struct Proc*)(proc));
		PlaySfx(0x6B); 
	}

	// Check for R (next unit)
	if (gKeyStatus.repeatedKeys & R_BUTTON) {
		if (gKeyStatus.heldKeys & L_BUTTON)
			KDSwitchUnit(proc, GetUnit(1));
		else {
			u8 index = proc->pUnit->index; // u8 to guarantee 0-255 range with proper under/overflow behavior
			struct Unit* unit = NULL;

			// Find previous valid unit index
			do {
				unit = GetUnit(++index);
			} while (!unit || !(unit->pCharacterData));

			KDSwitchUnit(proc, unit);
		}
	}

	// Check for L (prev unit)
	if (gKeyStatus.repeatedKeys & L_BUTTON) {
		if (gKeyStatus.heldKeys & R_BUTTON)
			KDSwitchUnit(proc, GetUnit(1));
		else {
			u8 index = proc->pUnit->index; // u8 to guarantee 0-255 range with proper under/overflow behavior
			struct Unit* unit = NULL;

			// Find previous valid unit index
			do {
				unit = GetUnit(--index);
			} while (!unit || !(unit->pCharacterData));

			KDSwitchUnit(proc, unit);
		}
	}

	// Check for Start (reset unit)
	if ((gKeyStatus.pressedKeys & START_BUTTON)) {
		// let page load unit
		if (proc->pPageProc && sKDPageDefinitions[proc->PageIndex]->onLoadUnit)
			sKDPageDefinitions[proc->PageIndex]->onLoadUnit(proc->pPageProc, proc, proc->pUnit);
	}

	// Check for Select (page switch)
	if ((gKeyStatus.pressedKeys & SELECT_BUTTON))
		KDSwitchPage(proc, proc->PageIndex + 1);

	PrintDebugNumberHex(
		8 * (KD_PAGE_FRAME_X + 0),
		8 * (KD_PAGE_FRAME_Y - 1),
		proc->pUnit->index, 2
	);

	// TODO: do something with this
	CheckKonamiCode(proc);
}

void KDSwitchPage(UnitEditorProc* proc, unsigned id) {
	unsigned count;

	// Compute page count
	for (count = 0; sKDPageDefinitions[count]; ++count);

	if (id >= count)
		id = 0; // Prevent invalid page (any page > count rolls back to page 0)

	if (proc->pPageProc) {
		// let current page save unit
		if (sKDPageDefinitions[proc->PageIndex]->onSaveUnit)
			sKDPageDefinitions[proc->PageIndex]->onSaveUnit(proc->pPageProc, proc, proc->pUnit);

		// end page
		EndProc(proc->pPageProc);
	}

	// clear page/frame gfx
	KDClearPage(proc);

	// setup new page
	proc->PageIndex = id;
	proc->pPageProc = sKDPageDefinitions[id]->start(proc);

	// let new page load unit
	if (sKDPageDefinitions[proc->PageIndex]->onLoadUnit)
		sKDPageDefinitions[proc->PageIndex]->onLoadUnit(proc->pPageProc, proc, proc->pUnit);
}

void KDSwitchUnit(UnitEditorProc* proc, struct Unit* unit) {
	// let page save unit
	if (proc->pPageProc && sKDPageDefinitions[proc->PageIndex]->onSaveUnit)
		sKDPageDefinitions[proc->PageIndex]->onSaveUnit(proc->pPageProc, proc, proc->pUnit);

	proc->pUnit = unit;

	// let page load unit
	if (proc->pPageProc && sKDPageDefinitions[proc->PageIndex]->onLoadUnit)
		sKDPageDefinitions[proc->PageIndex]->onLoadUnit(proc->pPageProc, proc, proc->pUnit);

	KDClearHeader(proc);
}

void KDClearHeader(UnitEditorProc* proc) {
	Font_InitDefault();

	// clear unit name line
	for (unsigned i = 0; i < 30; ++i)
		*BG_LOCATED_TILE(gBg0MapBuffer, i, KD_PAGE_FRAME_Y - 1) = 0;

	DBG_BG_Print(
		BG_LOCATED_TILE(gBg0MapBuffer, (KD_PAGE_FRAME_X + 3), (KD_PAGE_FRAME_Y - 1)),
		GetStringFromIndex(proc->pUnit->pCharacterData->nameTextId)
	);

	DrawTextInline(NULL, BG_LOCATED_TILE(gBg0MapBuffer, 0, 0), 0, 0, 8, " By Kirb");
	DrawTextInline(NULL, BG_LOCATED_TILE(gBg0MapBuffer, 8, 0), 3, 0, 21, "L/R: units  Select: pages  Start: reload");

	EnableBgSyncByMask(0b0001);
}

void KDClearPage(UnitEditorProc* proc) {
	LoadNewUIPal(-1);

	MakeUIWindowTileMap_BG0BG1(
		KD_PAGE_FRAME_X - 1,
		KD_PAGE_FRAME_Y - 1,
		KD_PAGE_FRAME_WIDTH + 2,
		KD_PAGE_FRAME_HEIGHT + 2,
		3
	);

	EnableBgSyncByMask(0b0011);
}

void SetupDebugUnitEditorPage1(UnitEditorProc *proc) {
	proc->StatsPage1[0]  = proc->pUnit->pow;
	proc->StatsPage1[1]  = proc->pUnit->skl;
	proc->StatsPage1[2]  = proc->pUnit->spd;
	proc->StatsPage1[3]  = proc->pUnit->lck;
	proc->StatsPage1[4]  = proc->pUnit->def;
	proc->StatsPage1[5]  = proc->pUnit->res;
	proc->StatsPage1[6]  = proc->pUnit->maxHP;
	proc->StatsPage1[7]  = proc->pUnit->curHP;
	proc->StatsPage1[8]  = proc->pUnit->conBonus;
	proc->StatsPage1[9]  = proc->pUnit->movBonus;
	proc->StatsPage1[10] = proc->pUnit->statusIndex;
	proc->StatsPage1[11] = proc->pUnit->statusDuration;
	proc->StatsPage1[12] = proc->pUnit->pClassData->number;
}

void UpdateDebugUnitEditorPage1(UnitEditorProc* proc) {
	//Check for down
	if ((gKeyStatus.repeatedKeys & DPAD_DOWN)) {
		if (proc->CursorIndex < UnitEditor_PAGE1ENTRIES) {
			proc->CursorIndex++;

			ClearMenuItemHighlight(1,0,2,(CursorLocationTable[proc->CursorIndex - 1].y / 8), 23);
			DrawMenuItemHighlight(1,0,2,(CursorLocationTable[proc->CursorIndex].y / 8), 23);
		} else {
			proc->CursorIndex = 0;

			ClearMenuItemHighlight(1,0,2,(CursorLocationTable[UnitEditor_PAGE1ENTRIES].y / 8), 23);
			DrawMenuItemHighlight(1,0,2,(CursorLocationTable[proc->CursorIndex].y / 8), 23);
		}
	}	
	
	//Check for up
	if ((gKeyStatus.repeatedKeys & DPAD_UP)) {
		if (proc->CursorIndex != 0) {
			proc->CursorIndex--;

			ClearMenuItemHighlight(1,0,2,(CursorLocationTable[proc->CursorIndex + 1].y / 8), 23);
			DrawMenuItemHighlight(1,0,2,(CursorLocationTable[proc->CursorIndex].y / 8), 23);
		} else {
			proc->CursorIndex = UnitEditor_PAGE1ENTRIES;

			ClearMenuItemHighlight(1,0,2,(CursorLocationTable[0].y / 8), 23);
			DrawMenuItemHighlight(1,0,2,(CursorLocationTable[proc->CursorIndex].y / 8), 23);
		}
	}

	//Check for right
	if ((gKeyStatus.repeatedKeys & DPAD_RIGHT)) {
		proc->StatsPage1[proc->CursorIndex]++;
	}

	//Check for left
	if ((gKeyStatus.repeatedKeys & DPAD_LEFT) != 0) {
		proc->StatsPage1[proc->CursorIndex]--;
	}

	//Check for a
	if ((gKeyStatus.pressedKeys & A_BUTTON) != 0) {
		proc->pUnit->pow = proc->StatsPage1[0];
		proc->pUnit->skl = proc->StatsPage1[1];
		proc->pUnit->spd = proc->StatsPage1[2];
		proc->pUnit->lck = proc->StatsPage1[3];
		proc->pUnit->def = proc->StatsPage1[4];
		proc->pUnit->res = proc->StatsPage1[5];
		proc->pUnit->maxHP = proc->StatsPage1[6];
		proc->pUnit->curHP = proc->StatsPage1[7];
		proc->pUnit->conBonus = proc->StatsPage1[8];
		proc->pUnit->movBonus = proc->StatsPage1[9];
		proc->pUnit->statusIndex = proc->StatsPage1[10];
		proc->pUnit->statusDuration = proc->StatsPage1[11];
		proc->pUnit->pClassData = GetClassData(proc->StatsPage1[12]);
	}

	//Get the unit index/name and print it
	char UnitName[0x20];
	GetStringFromIndexInBuffer(proc->pUnit->pCharacterData->nameTextId, UnitName);

	PrintDebugNumberHex(10, 25, proc->pUnit->index, 2);
	PrintDebugStringAsOBJ(48, 25, UnitName);
	
	//Get the class name and prints it
	char ClassName[0x20];

	GetStringFromIndexInBuffer(proc->pUnit->pClassData->nameTextId, ClassName);
	PrintDebugStringAsOBJ(48, 128, ClassName);

	//Prints the stats
	PrintDebugNumberDec(10, 40, proc->StatsPage1[0], 3);
	PrintDebugNumberDec(10, 48, proc->StatsPage1[1], 3);
	PrintDebugNumberDec(10, 56, proc->StatsPage1[2], 3);
	PrintDebugNumberDec(10, 64, proc->StatsPage1[3], 3);
	PrintDebugNumberDec(10, 72, proc->StatsPage1[4], 3);
	PrintDebugNumberDec(10, 80, proc->StatsPage1[5], 3);
	PrintDebugNumberDec(10, 88, proc->StatsPage1[6], 3);
	PrintDebugNumberDec(10, 96, proc->StatsPage1[7], 3);
	PrintDebugNumberDec(10, 104, proc->StatsPage1[8], 3);
	PrintDebugNumberDec(10, 112, proc->StatsPage1[9], 3);
	PrintDebugNumberDec(10, 120, proc->StatsPage1[10], 3);
	PrintDebugNumberDec(112, 120, proc->StatsPage1[11], 2);
	PrintDebugNumberHex(10, 128, proc->StatsPage1[12], 3);

	DisplayHandCursor(
		CursorLocationTable[proc->CursorIndex].x,
		CursorLocationTable[proc->CursorIndex].y
	);
}

void GenerateBGTsa(u16* MapOffset, u32 NumberOfTiles, u8 PaletteId) {
	for (int i = 0; i < NumberOfTiles; i++)
		MapOffset[i] = (i | (PaletteId << 12));
}

void CheckKonamiCode(UnitEditorProc* proc) {
	if ((gKeyStatus.pressedKeys & ButtonCombo[proc->KonamiCodeCounter]))
		proc->KonamiCodeCounter++;

	if (proc->KonamiCodeCounter == 11)
		DBG_BG_Print(BG_LOCATED_TILE(gBg0MapBuffer, 10, 18), "Konami Code");
}

void PrintConstantsPage1() {
	Font_InitDefault();

	FillBgMap(gBg0MapBuffer, 0);
	FillBgMap(gBg1MapBuffer, 0);

	MakeUIWindowTileMap_BG0BG1(0, 2, 26, 16, 0);
	LoadNewUIPal(-1);

	DrawTextInline(NULL, BG_LOCATED_TILE(gBg0MapBuffer, 0, 0), 0, 0, 12, "Kirb's debug-o-matic.");
	DrawTextInline(NULL, BG_LOCATED_TILE(gBg0MapBuffer, 13, 0), 3, 0, 17, "Press Select to switch pages.");

	DrawTextInline(NULL, BG_LOCATED_TILE(gBg0MapBuffer, 0, 18), 4, 0, 20, " Donate to those that need it.");

	DBG_BG_Print(BG_LOCATED_TILE(gBg0MapBuffer, 6, 5), "Power");
	DBG_BG_Print(BG_LOCATED_TILE(gBg0MapBuffer, 6, 6), "Skill");
	DBG_BG_Print(BG_LOCATED_TILE(gBg0MapBuffer, 6, 7), "Speed");
	DBG_BG_Print(BG_LOCATED_TILE(gBg0MapBuffer, 6, 8), "Luck");
	DBG_BG_Print(BG_LOCATED_TILE(gBg0MapBuffer, 6, 9), "Defense");
	DBG_BG_Print(BG_LOCATED_TILE(gBg0MapBuffer, 6, 10), "Resistance");
	DBG_BG_Print(BG_LOCATED_TILE(gBg0MapBuffer, 6, 11), "Max HP");
	DBG_BG_Print(BG_LOCATED_TILE(gBg0MapBuffer, 6, 12), "Current HP");
	DBG_BG_Print(BG_LOCATED_TILE(gBg0MapBuffer, 6, 13), "Constitution");
	DBG_BG_Print(BG_LOCATED_TILE(gBg0MapBuffer, 6, 14), "Move");
	DBG_BG_Print(BG_LOCATED_TILE(gBg0MapBuffer, 6, 15), "Status");
	DBG_BG_Print(BG_LOCATED_TILE(gBg0MapBuffer, 17, 15), "Duration");
}

void PrintConstantsPage2() {
	Font_InitDefault();

	FillBgMap(gBg0MapBuffer, 0);
	FillBgMap(gBg1MapBuffer, 0);

	MakeUIWindowTileMap_BG0BG1(0, 2, 29, 9, 3);
	LoadNewUIPal(-1);

	DrawTextInline(NULL, BG_LOCATED_TILE(gBg0MapBuffer, 0, 0), 0, 0, 12, "Kirb's debug-o-matic.");
	DrawTextInline(NULL, BG_LOCATED_TILE(gBg0MapBuffer, 13, 0), 3, 0, 16, "Press Select to switch pages.");

	DrawTextInline(NULL, BG_LOCATED_TILE(gBg0MapBuffer, 0, 18), 4, 0, 20, " Donate to those that need it.");

	DBG_BG_Print(BG_LOCATED_TILE(gBg0MapBuffer, 6, 5), "Item 1:");
	DBG_BG_Print(BG_LOCATED_TILE(gBg0MapBuffer, 6, 6), "Item 2:");
	DBG_BG_Print(BG_LOCATED_TILE(gBg0MapBuffer, 6, 7), "Item 3:");
	DBG_BG_Print(BG_LOCATED_TILE(gBg0MapBuffer, 6, 8), "Item 4:");
	DBG_BG_Print(BG_LOCATED_TILE(gBg0MapBuffer, 6, 9), "Item 5:");
}

void SetupDebugUnitEditorPage2(UnitEditorProc* proc) {
	proc->StatsPage2[0] = proc->pUnit->items[0] & 0xFF;
	proc->StatsPage2[1] = proc->pUnit->items[1] & 0xFF;
	proc->StatsPage2[2] = proc->pUnit->items[2] & 0xFF;
	proc->StatsPage2[3] = proc->pUnit->items[3] & 0xFF;
	proc->StatsPage2[4] = proc->pUnit->items[4] & 0xFF;
}

void UpdateDebugUnitEditorPage2(UnitEditorProc* proc) {
	//Check for down
	if ((gKeyStatus.repeatedKeys & DPAD_DOWN)) {
		if (proc->CursorIndex < UnitEditor_PAGE2ENTRIES) {
			proc->CursorIndex++;

			ClearMenuItemHighlight(1, 0,
				((CursorLocationTable[proc->CursorIndex - 1].x + 6) / 8),
				(CursorLocationTable[proc->CursorIndex - 1].y / 8),
				26
			);

			DrawMenuItemHighlight(1, 0,
				((CursorLocationTable[proc->CursorIndex].x + 6) / 8),
				(CursorLocationTable[proc->CursorIndex].y / 8),
				26
			);
		} else {
			proc->CursorIndex = 0;

			ClearMenuItemHighlight(1, 0,
				((CursorLocationTable[UnitEditor_PAGE2ENTRIES].x + 6) / 8),
				(CursorLocationTable[UnitEditor_PAGE2ENTRIES].y / 8),
				26
			);

			DrawMenuItemHighlight(1, 0,
				((CursorLocationTable[proc->CursorIndex].x + 6) / 8),
				(CursorLocationTable[proc->CursorIndex].y / 8),
				26
			);
		}
	}

	//Check for up
	if ((gKeyStatus.repeatedKeys & DPAD_UP)) {
		if (proc->CursorIndex != 0) {
			proc->CursorIndex--;

			ClearMenuItemHighlight(1,0,((CursorLocationTable[proc->CursorIndex + 1].x + 6) / 8),(CursorLocationTable[proc->CursorIndex + 1].y / 8), 26);
			DrawMenuItemHighlight(1,0,((CursorLocationTable[proc->CursorIndex].x + 6) / 8),(CursorLocationTable[proc->CursorIndex].y / 8), 26);
		}
		else {
			proc->CursorIndex = UnitEditor_PAGE2ENTRIES;

			ClearMenuItemHighlight(1,0,((CursorLocationTable[0].x + 6) / 8),(CursorLocationTable[0].y / 8), 26);
			DrawMenuItemHighlight(1,0,((CursorLocationTable[proc->CursorIndex].x + 6) / 8),(CursorLocationTable[proc->CursorIndex].y / 8), 26);
		}
	}	
	
	//Check for right
	if ((gKeyStatus.repeatedKeys & DPAD_RIGHT) != 0) {
		proc->StatsPage2[proc->CursorIndex]++;
	}	
	
	//Check for left
	if ((gKeyStatus.repeatedKeys & DPAD_LEFT) != 0) {
		proc->StatsPage2[proc->CursorIndex]--;
	}	

	//Check for a
	if ((gKeyStatus.pressedKeys & A_BUTTON) != 0) {
		proc->pUnit->items[0] = proc->StatsPage2[0];
		proc->pUnit->items[1] = proc->StatsPage2[1];
		proc->pUnit->items[2] = proc->StatsPage2[2];
		proc->pUnit->items[3] = proc->StatsPage2[3];
		proc->pUnit->items[4] = proc->StatsPage2[4];
	}	
	
	//Get the unit index/name and print it
	char UnitName[0x20];

	GetStringFromIndexInBuffer(proc->pUnit->pCharacterData->nameTextId, UnitName);
	PrintDebugNumberHex(10, 25, proc->pUnit->index, 2);
	PrintDebugStringAsOBJ(48, 25, UnitName);
	
	//Get Items names
	char ItemName[0x20];

	GetStringFromIndexInBuffer(GetItemData(GetItemIndex(proc->pUnit->items[0]))->nameTextId, ItemName);
	PrintDebugStringAsOBJ(104, 40, ItemName);
	
	GetStringFromIndexInBuffer(GetItemData(GetItemIndex(proc->pUnit->items[1]))->nameTextId, ItemName);
	PrintDebugStringAsOBJ(104, 48, ItemName);
	
	GetStringFromIndexInBuffer(GetItemData(GetItemIndex(proc->pUnit->items[2]))->nameTextId, ItemName);
	PrintDebugStringAsOBJ(104, 56, ItemName);
	
	GetStringFromIndexInBuffer(GetItemData(GetItemIndex(proc->pUnit->items[3]))->nameTextId, ItemName);
	PrintDebugStringAsOBJ(104, 64, ItemName);
	
	GetStringFromIndexInBuffer(GetItemData(GetItemIndex(proc->pUnit->items[4]))->nameTextId, ItemName);
	PrintDebugStringAsOBJ(104, 72, ItemName);

	//Prints the stats
	PrintDebugNumberHex(10, 40, proc->StatsPage2[0], 3);
	PrintDebugNumberHex(10, 48, proc->StatsPage2[1], 3);
	PrintDebugNumberHex(10, 56, proc->StatsPage2[2], 3);
	PrintDebugNumberHex(10, 64, proc->StatsPage2[3], 3);
	PrintDebugNumberHex(10, 72, proc->StatsPage2[4], 3);
	
	//Draw cursor according to location table down there
	DisplayHandCursor(CursorLocationTable[proc->CursorIndex].x,CursorLocationTable[proc->CursorIndex].y);
}

const ProcInstruction Debug6C[] = {
	PROC_SET_NAME("Kirb:DebugUnitEditor"),

	PROC_CALL_ROUTINE(LockGameLogic),

	PROC_CALL_ROUTINE_ARG(StartFadeInBlack, 0x40),
	PROC_YIELD,

	PROC_CALL_ROUTINE(BlockGameGraphicsLogic),

	PROC_CALL_ROUTINE(DebugScreenSetup),
	PROC_NEW_CHILD(gProc_BG3HSlide),

	PROC_CALL_ROUTINE_ARG(NewFadeOutBlack, 0x40),
	PROC_YIELD,

	//Main Logic
	PROC_LOOP_ROUTINE(DebugScreenLoop),

	PROC_CALL_ROUTINE_ARG(StartFadeInBlack, 0x40),
	PROC_YIELD,

	PROC_CALL_ROUTINE(EndBG3Slider),

	PROC_CALL_ROUTINE(ReloadGameCoreGraphics),
	PROC_CALL_ROUTINE(UnblockGameGraphicsLogic),

	PROC_CALL_ROUTINE_ARG(NewFadeOutBlack, 0x40),
	PROC_YIELD,

	PROC_CALL_ROUTINE(UnlockGameLogic),

	PROC_END
};	

const LocationTable CursorLocationTable[] = {
	{ 10,  40-4  },
	{ 10,  48-4  },
	{ 10,  56-4  },
	{ 10,  64-4  },
	{ 10,  72-4  },
	{ 10,  80-4  },
	{ 10,  88-4  },
	{ 10,  96-4  },
	{ 10,  104-4 },
	{ 10,  112-4 },
	{ 10,  120-4 },
	{ 112, 120-4 },
	{ 10,  128-4 },
};

const u16 ButtonCombo[] = {
	DPAD_UP,
	DPAD_UP,
	DPAD_DOWN,
	DPAD_DOWN,
	DPAD_LEFT,
	DPAD_RIGHT,
	DPAD_LEFT,
	DPAD_RIGHT,
	A_BUTTON,
	B_BUTTON,
	START_BUTTON,
};