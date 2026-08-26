#include "common_structs.h"

#define DEBUG_INFO 0

extern u8 D_global_asm_807FF01C;
extern s32 D_global_asm_807FF020;
extern s32 D_global_asm_807FF024;
extern s32 D_global_asm_807FF028;
void func_global_asm_8061D4E4(Actor *arg0);

void *D_global_asm_80756360[] = {
    0,
    "CLIP ARRAY OVERFLOW",
    "MAIN STACK OVERFLOW",
    "DFS OVERFLOW",
    "OUT OF MEMORY",
    "DATABASE ERROR",
    "DMA ERROR",
    "LOOKUP ERROR",
    "TOO MANY OBJECTS",
    "KILL SOUND ERROR",
    "STORED STATE ERROR",
    "MATRIX COPY ERROR",
    "DELAYED KILLS OVERFLOW",
    "LOCK STACK OVERFLOW",
    "POSTFUNCTIONS OVERFLOW",
    "SIGNALS OVERFLOW",
    "SORT LIST EARLY ERROR",
    "SORT LIST LATE ERROR",
    "DISPLAY LIST ERROR",
    "OBJECT EXIST OVERFLOW",
};

RECOMP_PATCH void raiseException(u8 arg0, s32 arg1, s32 arg2, s32 arg3) {
    D_global_asm_807FF01C = arg0;
    D_global_asm_807FF020 = arg1;
    D_global_asm_807FF024 = arg2;
    D_global_asm_807FF028 = arg3;
    recomp_printf("Exception raised: %s (Code %d) %d %d %d", D_global_asm_80756360[arg0], arg0, arg1, arg2, arg3);
    func_global_asm_8061D4E4(NULL); // Causes an instant crash
}

#if DEBUG_INFO
f32 dl_load = 0.0f;
Gfx *alignHUD(Gfx * dl, enumSpriteAlignment alignment);
Gfx *popHUD(Gfx *dl, enumSpriteAlignment alignment);
Gfx* printStyledText(Gfx* dl, s16 style, s16 x, s16 y, u8* string, u32 extraBitfield);
extern Gfx** D_1000118;
extern Mtx D_2000180;
extern Mtx D_20000C0;

Gfx *displayGFXLoad(Gfx *dl, Actor *ac) {
    u8 sp3C[13];
    f32 redness, greenness;

    gSPDisplayList(dl++, &D_1000118);
    gDPSetCombineMode(dl++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
    gSPMatrix(dl++, &D_2000180, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPMatrix(dl++, &D_20000C0, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_PROJECTION);
    gDPPipeSync(dl++);
    redness = 255.0f * (dl_load / 100.0f);
    greenness = 255.0f * (1.0f - (dl_load / 100.0f));
    gDPSetPrimColor(dl++, 0, 0, MIN(redness, 0xFF), MIN(greenness, 0xFF), 0x00, 0xFF);
    _sprintf(sp3C, "DL: %.2f%%", dl_load);
    dl = alignHUD(dl, ALIGN_LEFT);
    dl = printStyledText(dl, 6, 12 * 4, (240 - 20) * 4, sp3C, 1);
    dl = popHUD(dl, ALIGN_LEFT);
    return dl;
}

extern s32 D_global_asm_8076A058;
void addActorToTextOverlayRenderArray(void *arg0, Actor *arg1, u8 arg2);
extern Gfx *D_global_asm_8076A050[];

RECOMP_PATCH void func_global_asm_805FE71C(Gfx *dl, u8 arg1, s32 *arg2, u8 arg3) {
    Gfx *dl2 = dl;
    if (arg3) {
        gDPFullSync(dl2++);
    }
    gSPEndDisplayList(dl2++);
    *arg2 = (dl2 - D_global_asm_8076A050[arg1]);
    dl_load = (f32)(*arg2 * 100)/(f32)(D_global_asm_8076A058);
    if (*arg2 >= D_global_asm_8076A058) {
        raiseException(0x12, 0, 0, 0);
    }
    addActorToTextOverlayRenderArray(displayGFXLoad, NULL, 5);
}
#endif