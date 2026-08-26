#include "common_structs.h"

#define DEBUG_INFO 1

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
typedef struct HeapStruct {
    void *prev_obj;
    u32 size;
    struct HeapStruct *prev;
    struct HeapStruct *next;
} HeapStruct;

typedef struct HeapArenaMeta {
    void *index;
    HeapStruct *start;
    HeapStruct *tail;
    s32 bin_size;
    s16 chunk_size;
    u8 pad12[2];
} HeapArenaMeta;

f32 dl_load = 0.0f;
Gfx *alignHUD(Gfx * dl, u8 alignment);
Gfx *popHUD(Gfx *dl, u8 alignment);
Gfx* printStyledText(Gfx* dl, s16 style, s16 x, s16 y, u8* string, u32 extraBitfield);
extern Gfx** D_1000118;
extern Mtx D_2000180;
extern Mtx D_20000C0;
extern void *D_global_asm_80744470[];
extern HeapArenaMeta D_global_asm_807F0988[5];

f32 getHeapFill(void) {
    s32 total_capacity;
    s32 total_size;
    s32 i;
    HeapStruct *addr;

    total_size = 0;
    total_capacity = (0x805FAE00 - 0x25800) - ((s32)D_global_asm_80744470[1] + 0x25800);
    for (i = 0; i < 5; i++) {
        addr = D_global_asm_807F0988[i].start;
        while (addr) {
            total_size += addr->size;
            addr = addr->next;
        }
    }
    return (f32)(100.0f * total_size) / (f32)total_capacity;
}

Gfx *displayPercentage(Gfx *dl, f32 perc, char *str, s32 y) {
    u8 sp3C[13];
    f32 redness, greenness;

    redness = 255.0f * (perc / 100.0f);
    greenness = 255.0f * (1.0f - (perc / 100.0f));
    gDPSetPrimColor(dl++, 0, 0, MIN(redness, 0xFF), MIN(greenness, 0xFF), 0x00, 0xFF);
    _sprintf(sp3C, str, perc);
    return printStyledText(dl, 6, 12 * 4, y * 4, sp3C, 1);
}

Gfx *displayGFXLoad(Gfx *dl, Actor *ac) {

    gSPDisplayList(dl++, &D_1000118);
    gDPSetCombineMode(dl++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
    gSPMatrix(dl++, &D_2000180, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPMatrix(dl++, &D_20000C0, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_PROJECTION);
    gDPPipeSync(dl++);
    dl = alignHUD(dl, ALIGN_LEFT);
    dl = displayPercentage(dl, dl_load, "DL: %.2f%%", 220);
    dl = displayPercentage(dl, getHeapFill(), "HF: %.2f%%", 200);
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