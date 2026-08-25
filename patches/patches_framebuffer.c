#include "common_structs.h"
#include "ui.h"

extern u32 D_global_asm_80744478;
extern s32 D_global_asm_80747B24;
extern u16 *D_global_asm_807F5D80;
extern s8 D_global_asm_807F5D84;
extern s8 D_global_asm_807F5D85;
extern s16 D_global_asm_807F5D86;
extern s16 D_global_asm_807F5D88;
extern f32 D_global_asm_807F5D8C;
extern f32 D_global_asm_807F5D90;
extern f32 D_global_asm_807F5D94;
extern Mtx D_global_asm_807F5D98;
extern u32 global_properties_bitfield;
extern u8 D_global_asm_80747B20; 
extern void func_global_asm_8061134C(void*);
extern Gfx *func_global_asm_805FD030(Gfx *);
extern void func_global_asm_8062A3F0(void);
extern void func_global_asm_807023E8(Gfx **, u16 *, s32, s32, s32, s32, s32, f32, f32, f32, f32, f32, f32);
extern void func_global_asm_807024E0(Gfx **, void*, s32, s32, s32, s32, s32, f32, f32, f32, f32, f32, f32, s32, s32, s32, void *);
void func_global_asm_8062A130(s32 arg0, s32 arg1, void *arg2);
void func_global_asm_8062A228(s32 arg0, s32 arg1, void *arg2);
extern void func_global_asm_8062A24C(s32 arg0, s32 arg1, void *arg2);
extern s16 D_global_asm_80744490;
extern s16 D_global_asm_80744494;
extern s16 D_global_asm_80744498;
extern s16 D_global_asm_8074449C;
extern s16 D_global_asm_807444A0;
extern s16 D_global_asm_807444A4;
extern s16 D_global_asm_807444A8;
extern s16 D_global_asm_807444AC;
extern s16 D_global_asm_807444B0;
extern s16 D_global_asm_807444B4;
extern Gfx **D_1000118;
extern Mtx D_2000140;
extern u8 is_cutscene_active;

typedef struct {
    u32 unk0;
    u32 unk4;
    u32 unk8;
    u32 unkC;
    u32 unk10;
    u32 unk14;
    u32 unk18;
    u32 unk1C;
} Struct8070A848;

u16 stored_framebuffer[320 * 240];

extern Actor *gCurrentActorPointer;
extern void addActorToTextOverlayRenderArray(void *arg0, Actor *arg1, u8 arg2);

extern void *D_global_asm_80744470[];
extern u8  D_global_asm_807444FC;
Gfx *rdpStoreFB(Gfx *dl, Actor *a) {
    s32 x;
    s32 y;

    gDPPipeSync(dl++);
    gDPSetCycleType(dl++, G_CYC_1CYCLE);
    gDPSetTextureFilter(dl++, G_TF_POINT);
    gDPSetAlphaCompare(dl++, G_AC_NONE);
    gDPSetCombineMode(dl++, G_CC_DECALRGBA, G_CC_DECALRGBA);
    gDPSetRenderMode(dl++, G_RM_NOOP, G_RM_NOOP2);
    gDPSetColorImage(dl++, G_IM_FMT_RGBA, G_IM_SIZ_16b, 320, OS_K0_TO_PHYSICAL(stored_framebuffer));
    for (y = 0; y < 240; y += 6) {
        gDPLoadTextureTile(
            dl++, D_global_asm_80744470[D_global_asm_807444FC],
            G_IM_FMT_RGBA, G_IM_SIZ_16b, 320, 240,
            0, y, 320 - 1, MIN(y + 6, 240) - 1,
            NULL,
            G_TX_CLAMP, G_TX_CLAMP,
            G_TX_NOMASK, G_TX_NOMASK,
            0, 0
        );
        gSPScisTextureRectangle(
            dl++,
            0, y * 4,
            320 * 4, MIN(y + 6, 240) * 4,
            G_TX_RENDERTILE,
            0, y << 5,
            1024, 1024
        );
    }

    gDPPipeSync(dl++);
    gDPFullSync(dl++); // Required to ensure that this process is done before anything else gets rendered to the referenced FB
    return dl;
}

extern void *D_global_asm_8076A060;
extern Gfx *func_global_asm_8068C20C(Gfx *, u8);
RECOMP_PATCH Gfx *func_global_asm_805FE398(Gfx *dl) {
    dl = func_global_asm_8068C20C(dl, 0); // @recomp: Run a special pre-world load framebuffer thing
    gDPPipeSync(dl++);
    gDPSetRenderMode(dl++, G_RM_NOOP, G_RM_NOOP2);
    gDPSetCycleType(dl++, G_CYC_FILL);
    gSPClearGeometryMode(dl++, G_ZBUFFER);
    gDPSetDepthImage(dl++, osVirtualToPhysical(D_global_asm_8076A060));
    gDPSetColorImage(dl++, G_IM_FMT_RGBA, G_IM_SIZ_16b, D_global_asm_80744490, osVirtualToPhysical(D_global_asm_8076A060));
    gDPSetFillColor(dl++, 0xFFFCFFFC);
    gDPFillRectangle(dl++, 0, 0, D_global_asm_80744490 - 1, D_global_asm_80744494 - 1);
    gDPPipeSync(dl++);
    return dl;
}

RECOMP_PATCH void func_global_asm_8070A848(Struct8070A848 *arg0, Struct8070A848 *arg1) {
    s32 i;
    Struct8070A848 *src = arg1;
    Struct8070A848 *dst = arg0;

    for (i = 0; i < ((D_global_asm_80744490 * D_global_asm_80744494) / 16); i++) {
        dst->unk0 = src->unk0 | 0x10001;
        dst->unk4 = src->unk4 | 0x10001;
        dst->unk8 = src->unk8 | 0x10001;
        dst->unkC = src->unkC | 0x10001;
        dst->unk10 = src->unk10 | 0x10001;
        dst->unk14 = src->unk14 | 0x10001;
        dst->unk18 = src->unk18 | 0x10001;
        dst->unk1C = src->unk1C | 0x10001;
        dst++;
        src++;
    }
    if ((is_cutscene_active == 6) || (D_global_asm_807F5D84 == 0)) {
        addActorToTextOverlayRenderArray(rdpStoreFB, gCurrentActorPointer, 0);
    }
}

extern s32 D_global_asm_80747B30;
extern s32 D_global_asm_80747B34;
// @recomp: Framebuffer effects renderer
RECOMP_PATCH Gfx *func_global_asm_80629300(Gfx *dl) {
    f32 sp54;
    s32 width, height;
    s32 progress;

    sp54 = D_global_asm_80744478 * 0.5f;
    if (D_global_asm_80747B24 == 0) {
        guScale(&D_global_asm_807F5D98, 2.0f, 2.0f, 1.0f);
        D_global_asm_80747B24 = 1;
    }
    if ((D_global_asm_807F5D84 == 0) && (D_global_asm_80747B20 != 0)) {
        D_global_asm_80747B20--;
    }
    if (D_global_asm_807F5D84 < 0) {
        D_global_asm_807F5D84++;
        if (D_global_asm_807F5D84 == 0) {
            func_global_asm_8061134C(D_global_asm_807F5D80);
        }
    } else {
        if (D_global_asm_807F5D84 > 0) {
            gSPDisplayList(dl++, &D_1000118);
            dl = func_global_asm_805FD030(dl);
            gEXMatrixGroup(dl++, MTXTAG_FRAMEBUFFERTRANSITION, G_EX_INTERPOLATE_SIMPLE, G_EX_NOPUSH, G_MTX_PROJECTION, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_SKIP, G_EX_COMPONENT_INTERPOLATE, G_EX_ORDER_LINEAR, G_EX_EDIT_NONE, G_EX_ASPECT_STRETCH, G_EX_COMPONENT_SKIP, G_EX_COMPONENT_AUTO);
            gSPMatrix(dl++, &D_2000140, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_PROJECTION);
            gSPMatrix(dl++, &D_global_asm_807F5D98, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gDPPipeSync(dl++);
            gDPSetTextureFilter(dl++, G_TF_POINT);
            gDPSetColorDither(dl++, G_CD_DISABLE);
            gDPSetScissor(dl++, G_SC_NON_INTERLACE, D_global_asm_80744498, D_global_asm_8074449C, D_global_asm_807444A0, D_global_asm_807444A4);
            gDPSetRenderMode(dl++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
            recomp_get_ui_bounds(&width, &height);
            gEXPushScissor(dl++);
            gEXPushViewport(dl++);
            gEXSetScissor(dl++, G_SC_NON_INTERLACE, G_EX_ORIGIN_LEFT, G_EX_ORIGIN_RIGHT, 0, 0, 0, D_global_asm_80744494);
            gEXSetViewportAlign(dl++, G_EX_ORIGIN_LEFT, 0, 0);
            switch (D_global_asm_807F5D85) {
                case 7: // Pausing (Blurred Background)
                    // @recomp: We can't morph the framebuffer image as this breaks RT64 widescreen support
                    // func_global_asm_8062A3F0();
                    gDPSetCombineMode(dl++, G_CC_MODULATEI_PRIM, G_CC_MODULATEI_PRIM); // @recomp: Remove alpha texture stuff
                    // Blurs it a little
                    gDPSetPrimColor(dl++, 0, 0, 0x80, 0x80, 0x80, 0xFF);
                    func_global_asm_807023E8(&dl, stored_framebuffer, 0, 0x140, 0xF0, 0x20, 0x20, 0.0f, 0.0f, 319.0f, 239.0f, 0.0f, 0.0f);
                    // Add a feathering call to add a little distortion
                    // func_global_asm_807024E0(&dl, stored_framebuffer, 0, 0x140, 0xF0, 0x10, 0x50, 0, 0.0f, 320, 239.0f, 0, 0.0f, 1, 0x10, 1, NULL);
                    if (global_properties_bitfield & 0x40) {
                        D_global_asm_807F5D84 = -2;
                    }
                    break;
                case 1: // Fade Transition
                    gDPSetCombineMode(dl++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
                    gDPSetPrimColor(dl++, 0, 0, 0xFF, 0xFF, 0xFF, D_global_asm_807F5D86);
                    func_global_asm_807023E8(&dl, stored_framebuffer, 0, 0x140, 0xF0, 0x20, 0x20, 0.0f, 0.0f, 319.0f, 239.0f, 0.0f, 0.0f);
                    D_global_asm_807F5D86 -= sp54 * 5;
                    if (D_global_asm_807F5D86 < 0) {
                        D_global_asm_807F5D84 = -2;
                    }
                    break;
                case 2: // L -> R Swipe
                    gDPSetCombineMode(dl++, G_CC_MODULATEI_PRIM, G_CC_MODULATEI_PRIM); // @recomp: Remove alpha texture stuff
                    gDPSetPrimColor(dl++, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF);
                    func_global_asm_807023E8(&dl, stored_framebuffer, 0, 0x140, 0xF0, 0x20, 0x20, D_global_asm_807F5D86, 0.0f, 319.0f, 239.0f, D_global_asm_807F5D86, 0.0f);
                    gDPPipeSync(dl++);
                    gDPSetCombineMode(dl++, G_CC_MODULATEIA, G_CC_MODULATEIA);
                    if (D_global_asm_807F5D86 >= 0x11) {
                        func_global_asm_807024E0(&dl, stored_framebuffer, 0, 0x140, 0xF0, 0x10, 0x50, (D_global_asm_807F5D86 - 0x10), 0.0f, D_global_asm_807F5D86, 239.0f, (D_global_asm_807F5D86 - 0x10), 0.0f, 1, 0x10, 1, NULL);
                    }
                    D_global_asm_807F5D86 += (sp54 * 0xA);
                    if (D_global_asm_807F5D86 >= 0x137) {
                        D_global_asm_807F5D84 = -2;
                    }
                    break;
                case 0: // R -> L Swipe (Lanky/Tiny crypt)
                    gDPSetCombineMode(dl++, G_CC_MODULATEI_PRIM, G_CC_MODULATEI_PRIM); // @recomp: Remove alpha texture stuff
                    gDPSetPrimColor(dl++, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF);
                    func_global_asm_807023E8(&dl, stored_framebuffer, 0, 0x140, 0xF0, 0xA, 0x50, 0.0f, 0.0f, D_global_asm_807F5D86, 239.0f, 0.0f, 0.0f);
                    gDPSetCombineMode(dl++, G_CC_MODULATEIA, G_CC_MODULATEIA);
                    if (D_global_asm_807F5D86 < 0x131) {
                        func_global_asm_807024E0(&dl, stored_framebuffer, 0, 0x140, 0xF0, 0x10, 0x20, D_global_asm_807F5D86, 0.0f, D_global_asm_807F5D86 + 0x10, 239.0f, D_global_asm_807F5D86, 0.0f, 1, 0x10, 2, NULL);
                    }
                    D_global_asm_807F5D86 -= (sp54 * 0xA);
                    if (D_global_asm_807F5D86 < 0xB) {
                        D_global_asm_807F5D84 = -2;
                    }
                    break;
                case 3: // Dual Swipe
                    gDPSetCombineMode(dl++, G_CC_MODULATEI_PRIM, G_CC_MODULATEI_PRIM); // @recomp: Remove alpha texture stuff
                    gDPSetPrimColor(dl++, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF);
                    func_global_asm_807023E8(&dl, stored_framebuffer, 0, 0x140, 0xF0, 0x20, 0x20, D_global_asm_807F5D86, 0.0f, 319.0f, 239.0f, D_global_asm_807F5D86, 0.0f);
                    func_global_asm_807023E8(&dl, stored_framebuffer, 0, 0x140, 0xF0, 0xA, 0x50, 0.0f, 0.0f, D_global_asm_807F5D88, 239.0f, 0.0f, 0.0f);
                    gDPSetCombineMode(dl++, G_CC_MODULATEIA, G_CC_MODULATEIA);
                    if (D_global_asm_807F5D86 >= 0x11) {
                        func_global_asm_807024E0(&dl, stored_framebuffer, 0, 0x140, 0xF0, 0x10, 0x50, (D_global_asm_807F5D86 - 0x10), 0.0f, D_global_asm_807F5D86, 239.0f, (D_global_asm_807F5D86 - 0x10), 0.0f, 1, 0x10, 1, NULL);
                    }
                    if (D_global_asm_807F5D88 < 0x131) {
                        func_global_asm_807024E0(&dl, stored_framebuffer, 0, 0x140, 0xF0, 0x10, 0x20, D_global_asm_807F5D88, 0.0f, D_global_asm_807F5D88 + 0x10, 239.0f, D_global_asm_807F5D88, 0.0f, 1, 0x10, 2, NULL);
                    }
                    D_global_asm_807F5D86 += (sp54 * 0xA);
                    D_global_asm_807F5D88 -= (sp54 * 0xA);
                    if (D_global_asm_807F5D86 >= 0x137) {
                        D_global_asm_807F5D84 = -2;
                    }
                    break;
                case 4: // Iris Wipe
                    gDPSetCombineMode(dl++, G_CC_MODULATEIA, G_CC_MODULATEIA);
                    D_global_asm_807F5D8C = D_global_asm_807F5D8C + (5.0 * sp54);
                    D_global_asm_807F5D90 = D_global_asm_807F5D8C + 40.0f;
                    func_global_asm_807024E0(&dl, stored_framebuffer, 0, 0x140, 0xF0, 0x20, 0x20, 0.0f, 0.0f, width, height, 0.0f, 0.0f, 1, 0x10, 1, func_global_asm_8062A24C);
                    if (D_global_asm_807F5D8C > 170.0f) {
                        D_global_asm_807F5D84 = -2;
                    }
                    break;
                case 5: // TL->BR Wipe
                    gDPSetCombineMode(dl++, G_CC_MODULATEIA, G_CC_MODULATEIA);
                    D_global_asm_807F5D8C = D_global_asm_807F5D8C + (12.0 * sp54);
                    D_global_asm_807F5D90 = D_global_asm_807F5D8C + 40.0f;
                    func_global_asm_807024E0(&dl, stored_framebuffer, 0, 0x140, 0xF0, 0x20, 0x20, 0.0f, 0.0f, width, height, 0.0f, 0.0f, 1, 0x10, 1, func_global_asm_8062A228);
                    if (D_global_asm_807F5D8C > 350.0f) {
                        D_global_asm_807F5D84 = -2;
                    }
                    break;
                case 6: // Clock Wipe
                    gDPSetCombineMode(dl++, G_CC_MODULATEIA, G_CC_MODULATEIA);
                    D_global_asm_807F5D94 = D_global_asm_807F5D94 + (15.0 * sp54);
                    func_global_asm_807024E0(&dl, stored_framebuffer, 0, 0x140, 0xF0, 0x20, 0x20, 0.0f, 0.0f, width, height, 0.0f, 0.0f, 1, 0x10, 1, func_global_asm_8062A130);
                    if (D_global_asm_807F5D94 > 350.0f) {
                        D_global_asm_807F5D84 = -2;
                    }
                    break;
            }
            gEXMatrixGroup(dl++, MTXTAG_FRAMEBUFFERTRANSITION, G_EX_INTERPOLATE_SIMPLE, G_EX_NOPUSH, G_MTX_PROJECTION, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_SKIP, G_EX_COMPONENT_INTERPOLATE, G_EX_ORDER_LINEAR, G_EX_EDIT_NONE, G_EX_ASPECT_AUTO, G_EX_COMPONENT_SKIP, G_EX_COMPONENT_AUTO);
            gEXPopScissor(dl++);
            gEXPopViewport(dl++);
            // Clear gEX
            gEXSetRectAlign(dl++, G_EX_ORIGIN_NONE, G_EX_ORIGIN_NONE, 0, 0, 0, 0);
            gEXSetViewportAlign(dl++, G_EX_ORIGIN_NONE, 0, 0);
            gDPPipeSync(dl++);
            gDPSetColorDither(dl++, G_CD_MAGICSQ);
            gDPSetTextureFilter(dl++, G_TF_BILERP);
        }
    }
    return dl;
}

extern Actor *gLastSpawnedActor;
extern PlayerAdditionalActorData *extra_player_info_pointer;
extern s32 spawnActor(Actors actorIndex, s32 modelIndex);
extern void moveAndScaleActorToAnother(Actor *destination, Actor *source, f32 scale); 
extern void func_global_asm_806291B4(u8 arg0);
extern void func_global_asm_8065EACC(void);
extern void func_global_asm_80672C30(Actor *arg0);
extern void func_global_asm_806C8220(s32, void *, s32);
extern s32 func_global_asm_8061EB04(Actor *playerPointer, u8 playerIndex);
extern void func_global_asm_80602498(void);
extern s32 handleInputsForControlState(s32 arg0);
extern void playAnimation(Actor *arg0, s32 arg2);
extern void setYAccelerationFrom80753578(void);
extern void applyActorYAcceleration(void);
extern void func_global_asm_80617770(Actor *arg0, u32 arg1, u8 arg2);
extern void func_global_asm_806C8D20(Actor *arg0);
extern void func_global_asm_8065EAF4(void);
extern void func_global_asm_806CC970(void);
extern void renderActor(Actor *arg0, u8 arg1);

typedef struct Struct80767CE8 {
    Mtx unk0;
    u8 pad40[0xDB0 - 0x40];
    Gfx unkDB0;
    u8 padDB8[0x11B0 - 0xDB8];
} Struct80767CE8;

typedef struct {
    s32 unk0;
    s32 unk4;
    s32 unk8;
    s32 unkC;
    s32 unk10;
    s32 unk14;
    s32 unk18;
} Struct807FD9A8_unkC;

typedef struct {
    s32 unk0;
    s32 unk4;
    void *unk8;
    Struct807FD9A8_unkC *unkC;
    void *unk10;
    void *unk14;
    s32 *unk18;
    s32 *unk1C;
    s32 *unk20;
    s32 unk24;
    s32 unk28;
    s32 unk2C;
} Struct807FD9A8;

typedef struct {
    s32 offset;
    s32 width;
    s32 height;
} TexEntry;

extern u8 D_global_asm_807444F4;
extern u8 D_global_asm_807444F8;
extern Struct80767CE8 D_global_asm_80767CE8[2];
extern Struct80767CE8 *D_global_asm_8076A048;
extern s32 D_global_asm_8076A088;
extern s32 D_global_asm_8076A08C;
extern u8 D_global_asm_8076A0A4;
extern u8 D_global_asm_8076A0B1;
extern u8 D_global_asm_8076A0B2;
extern f32 D_global_asm_807FD888;
extern Struct807FD9A8 * D_global_asm_807FD9A8;
extern void* D_global_asm_807FD9B0;
extern TexEntry* D_global_asm_807FD9B4;
extern u8 D_global_asm_807FD9BC;
extern u8 D_global_asm_807FD9BD;
extern Gfx *D_global_asm_8076A050[];
extern u32 object_timer;
extern s32 D_global_asm_807FBB64;
extern u8 D_global_asm_807501E0;
extern Vtx *D_global_asm_807FD9B8;
extern s32 D_global_asm_80755068;
extern s32 D_global_asm_8075506C;
extern void *D_global_asm_807FD9A4;
extern s32 func_global_asm_8070B7EC(Gfx**, void*, void*);
extern void func_global_asm_80610044(void *arg0, s32 arg1, u8 arg2, u8 arg3, s32 arg4, u8 arg5);
extern void func_global_asm_8070AF24(void);
extern void func_global_asm_8070AC74(Mtx *arg0, Gfx **dlp);
extern void func_global_asm_805FF378(Maps nextMap, s32 nextExit);
extern void func_global_asm_8066B434(void *arg0, s32 arg1, s32 arg2);
extern void func_global_asm_8061CBCC(void);
extern void func_global_asm_805FE71C(Gfx *dl, u8 arg1, s32 *arg2, u8 arg3);
extern void func_global_asm_805FE7B4(Gfx *dl, Gfx *arg1, s32 *arg2, u8 arg3);
extern void *getPointerTableFile(enum pointertable_e pointerTableIndex, s32 fileIndex, u8 arg2, u8 arg3);
extern void func_global_asm_80709890(Vtx *, Struct807FD9A8 **, void **, s32);
extern s32 func_global_asm_80709ACC(Struct807FD9A8 *);
extern void func_global_asm_807095E4(s32, s32);

extern void *D_global_asm_807FD9AC;
extern void *_malloc(s32);

// @recomp: Zipper Display
RECOMP_PATCH void func_global_asm_8070A934(enum map_e arg0, s32 arg1) {
    Gfx* sp34;
    Gfx* sp30;
    u8 temp_t1;
    u8 temp_t5;
    s32 i;

    func_global_asm_80610044(D_global_asm_8076A050[D_global_asm_807444FC], D_global_asm_8076A088, 3U, 1U, 0x4D2, 1U);
    D_global_asm_807444FC ^= 1;
    object_timer += 1;
    D_global_asm_8076A048 = &D_global_asm_80767CE8[D_global_asm_807444FC];
    sp30 = &D_global_asm_8076A048->unkDB0;
    switch (D_global_asm_807FD9BC) {
    case 1:
        break;
    case 0:
        D_global_asm_807FD9BC = 1;
        func_global_asm_8070AF24();
        break;
    }
    if ((D_global_asm_8076A0B1 & 1) && (D_global_asm_807FD888 == 31.0f)) {
        sp34 = D_global_asm_8076A050[D_global_asm_807444FC];
        if (D_global_asm_8076A0B2 == 1) {
            is_cutscene_active = D_global_asm_807444F4;
        }
    } else {
        func_global_asm_8070AC74(&D_global_asm_8076A048->unk0, &sp34);
        D_global_asm_807501E0 = 0; // @recomp: Release all overlays to prevent repeated snapshots
        gEXMatrixGroup(sp34++, MTXTAG_FRAMEBUFFERTRANSITION, G_EX_INTERPOLATE_SIMPLE, G_EX_NOPUSH, G_MTX_PROJECTION, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_SKIP, G_EX_COMPONENT_INTERPOLATE, G_EX_ORDER_LINEAR, G_EX_EDIT_NONE, G_EX_ASPECT_STRETCH, G_EX_COMPONENT_SKIP, G_EX_COMPONENT_AUTO);
        if (func_global_asm_8070B7EC(&sp34, D_global_asm_807FD9B8, stored_framebuffer) != 0) {
            switch (D_global_asm_807444F8) {
            case 1:
                osViBlack(1U);
                func_global_asm_805FF378(arg0, arg1);
                D_global_asm_807FD888 = 31.0f;
                D_global_asm_807444F8 = 2;
                break;
            case 2:
                func_global_asm_8061134C(D_global_asm_807FD9B0);
                func_global_asm_8061134C(D_global_asm_807FD9A8->unk8);
                func_global_asm_8061134C(D_global_asm_807FD9B4);
                func_global_asm_8061134C(D_global_asm_807FD9A8->unk10);
                func_global_asm_8061134C(D_global_asm_807FD9A8->unk14);
                func_global_asm_8066B434(D_global_asm_807FD9B8, 0x24B, 0x4A);
                is_cutscene_active = D_global_asm_807444F4;
                if ((is_cutscene_active == 1) && (D_global_asm_807FBB64 & 1)) {
                    func_global_asm_8061CBCC();
                }
                D_global_asm_807444F8 = 3;
                D_global_asm_807FD888 = 0.0f;
                break;
            }
        }
        gEXMatrixGroup(sp34++, MTXTAG_FRAMEBUFFERTRANSITION, G_EX_INTERPOLATE_SIMPLE, G_EX_NOPUSH, G_MTX_PROJECTION, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_SKIP, G_EX_COMPONENT_INTERPOLATE, G_EX_ORDER_LINEAR, G_EX_EDIT_NONE, G_EX_ASPECT_AUTO, G_EX_COMPONENT_SKIP, G_EX_COMPONENT_AUTO);
    }
    if (D_global_asm_807FD9BD != 0) {
        D_global_asm_807FD9BD--;
        if (!D_global_asm_807FD9BD) {
            D_global_asm_8076A0B1 |= 2;
        }
    }
    if ((D_global_asm_8076A0A4 != 0) && (is_cutscene_active != 6)) {
        func_global_asm_805FE71C(sp34, D_global_asm_807444FC, &D_global_asm_8076A088, 0U);
        func_global_asm_805FE7B4(sp30, (Gfx* ) D_global_asm_8076A048, &D_global_asm_8076A08C, 1U);
        return;
    }
    func_global_asm_805FE71C(sp34, D_global_asm_807444FC, &D_global_asm_8076A088, 1U);
}

void func_global_asm_80682E38(void *arg0);
extern s32 D_global_asm_8074E848[];

typedef struct TagAAD {
    Actor *unk0;
    u16 unk4;
    s8 unk6;
    u8 unk7;
    Actor *unk8[5];
    Actor *unk1C;
    s8 unk20;
    u8 pad21;
    s16 unk22;
    f32 unk24;
    u8 pad28[4];
    f32 unk2C;
    f32 unk30;
    f32 unk34;
    s16 unk38;
    u8 pad3A[2];
    s32 unk3C;
} TagAAD;
typedef struct {
    s16 unk0;
    s16 unk2;
    u8 unk4;
    u8 unk5;
} GlobalASMStruct45;

extern Maps current_map;
extern GlobalASMStruct45 D_global_asm_8074E814[];
extern GlobalASMStruct35 D_global_asm_807FBB70;
extern u8 func_global_asm_8061CB50(void);
extern void func_global_asm_806C9304(Actor *arg0, PlayerAdditionalActorData *arg1);
extern void func_global_asm_806C93E4(Actor *arg0, PlayerAdditionalActorData *arg1);
extern void func_global_asm_80659620(f32 *arg0, f32 *arg1, f32 *arg2, s16 arg3);
extern void func_global_asm_80659670(f32 arg0, f32 arg1, f32 arg2, s16 arg3);
extern void playSong(MUSIC_E arg0, f32 arg1);
extern void func_global_asm_80602CE0(s32 arg0, s32 arg1, u8 arg2);
extern s32 func_global_asm_805FF018(s32 actorBehaviour, s32 kongIndex);
extern void setFlag(s16 flagIndex, u8 newValue, u8 flagType);
extern void func_global_asm_80627878(Actor *arg0);
extern void func_global_asm_8060DEC8(void);
extern s32 playCutscene(Actor *arg0, s16 arg1, u8 arg2);
extern void func_global_asm_8061B650(Actor *arg0);
extern void func_global_asm_806FB218(void);

typedef struct Struct807FD610 {
    s32 unk0; // Timer that ticks up once per frame
    f32 unk4; // Probably float
    f32 unk8; // Probably float
    f32 unkC; // Probably float
    f32 unk10[4];
    s16 unk20[4];
    s16 unk28; // Used
    u16 unk2A; // Used, controller button bitfield
    u16 unk2C; // Used, controller button bitfield
    s8 unk2E; // Used
    s8 unk2F; // Used
    u8 unk30; // Used
    u8 unk31;
    s16 unk32;
} Struct807FD610;

extern Struct807FD610 D_global_asm_807FD610[]; 

extern int gameIsInDKTVMode(void);
extern int gameIsInMysteryMenuMinigameMode(void);
extern int gameIsInAdventureMode(void);
extern u8 func_global_asm_8061CB50(void);
extern u8 func_global_asm_8062919C(void);
extern u8 func_global_asm_806291A8(void);
extern s8 D_global_asm_807FC8B9;
extern u8 cc_player_index;

extern void func_global_asm_806F5378(void);
extern void func_global_asm_807313BC(void);
extern void func_global_asm_805FF5A0(Maps map);
extern Maps next_map;
extern s32 next_exit;
extern s16 D_global_asm_8076AEF2;

extern Gfx *func_global_asm_805FCFD8(Gfx *);
extern Gfx *func_global_asm_805FE398(Gfx *);
extern Gfx *func_global_asm_805FE4D4(Gfx *);
extern void *D_global_asm_8076A080;
extern u16 D_global_asm_8076A09C;
extern Gfx **D_1000090;

// @recomp: Zipper DL Setup
RECOMP_PATCH void func_global_asm_8070AC74(Mtx *arg0, Gfx **dlp) {
    Gfx *dl;
    dl = D_global_asm_8076A050[D_global_asm_807444FC];
    gSPSegment(dl++, 0x00, 0x00000000);
    gSPSegment(dl++, 0x02, osVirtualToPhysical(arg0));
    gSPSegment(dl++, 0x01, osVirtualToPhysical(D_global_asm_8076A080));
    gSPDisplayList(dl++, &D_1000090);
    dl = func_global_asm_805FCFD8(dl);
    dl = func_global_asm_805FE398(dl);
    gDPPipeSync(dl++);
    gDPSetCycleType(dl++, G_CYC_1CYCLE);
    guTranslate(&arg0[6], 0.0f, 0.0f, 0.0f);
    guLookAt(&arg0[8], 0.0f, 0.0f, 200.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
    guPerspective(arg0, &D_global_asm_8076A09C, 61.9f, 1.3333334f, 10.0f, 1000.0f, 1.0f);
    gDPPipeSync(dl++);
    dl = func_global_asm_805FE4D4(dl);
    gDPSetColorDither(dl++, G_CD_MAGICSQ);
    gDPSetAlphaDither(dl++, G_AD_PATTERN);
    gDPSetScissor(dl++, G_SC_NON_INTERLACE, 0, 0, D_global_asm_80744490, D_global_asm_80744494);
    gDPSetFillColor(dl++, 0xFFC1FFC1);
    gDPSetRenderMode(dl++, G_RM_NOOP, G_RM_NOOP2);
    gSPClearGeometryMode(dl++, G_ZBUFFER);
    gDPFillRectangle(dl++, 0, 0, D_global_asm_80744490, D_global_asm_80744494);
    gSPPerspNormalize(dl++, D_global_asm_8076A09C);
    gSPClipRatio(dl++, FRUSTRATIO_2);
    *dlp = dl;
}

extern s16 D_global_asm_80754CE0;
extern s16 D_global_asm_80754CEC[];
s16 menuXShift = 0;
s16 menuYShift = 0;
extern void *D_global_asm_807FD978[8];

// @recomp: Render main menu scrolling background
RECOMP_PATCH Gfx* func_global_asm_80706F90(Gfx* dl) {
    s32 i;
    s32 spF8;
    s32 spF4;
    s32 x;
    s32 y;
    s32 var_t5;
    u16 spE6;
    s16 var_s4;
    void *a2;
    s32 j;
    s32 X_REPEAT_COUNT, Y_REPEAT_COUNT;
    s32 width, height;

    recomp_get_ui_bounds(&width, &height);
    X_REPEAT_COUNT = (width >> 7) + 4;
    Y_REPEAT_COUNT = (height >> 7) + 4;

    gSPLoadGeometryMode(dl++, 0);
    gSPSetGeometryMode(dl++, G_SHADE | G_SHADING_SMOOTH);
    gDPSetCycleType(dl++, G_CYC_1CYCLE);
    gDPSetRenderMode(dl++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
    gDPSetCombineMode(dl++, G_CC_MODULATEIDECALA_PRIM, G_CC_MODULATEIDECALA_PRIM);
    gDPSetTexturePersp(dl++, G_TP_NONE);
    gDPSetTextureFilter(dl++, G_TF_POINT);
    //
    gEXPushScissor(dl++);
    gEXSetScissor(dl++, G_SC_NON_INTERLACE, G_EX_ORIGIN_NONE, G_EX_ORIGIN_NONE, 0, 0, width, height);
    gEXSetRectAlign(dl++, G_EX_ORIGIN_NONE, G_EX_ORIGIN_NONE, 0, 0, 0, 0);
    //
    gSPTexture(dl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
    gDPSetPrimColor(dl++, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF);
    gDPPipeSync(dl++);
    var_s4 = menuYShift + D_global_asm_80754CE0;
    if (var_s4 < 0) {
        var_s4 += 0x200;
    }
    if (var_s4 >= 0x200) {
        var_s4 -= 0x200;
    }
    if (D_global_asm_807FD978[0] == NULL) {
        for (i = 0; i < 8; i++) {
            D_global_asm_807FD978[i] = getPointerTableFile(TABLE_25_TEXTURES_GEOMETRY, D_global_asm_80754CEC[i], 0U, 0U);
        }
    }
    spE6 = 0;
    for (spF4 = 0x40; spF4 >= 0; spF4 -= 0x40) {
        spF8 = 0;
        while (spF8 < 0x80) {
            gDPPipeSync(dl++);
            a2 = D_global_asm_807FD978[spE6 % 8];
            gDPLoadTextureBlock(dl++,
                OS_PHYSICAL_TO_K0(a2),
                G_IM_FMT_RGBA, G_IM_SIZ_16b, 32, 64, 0,
                G_TX_NOMIRROR | G_TX_CLAMP,
                G_TX_NOMIRROR | G_TX_CLAMP,
                G_TX_NOMASK, G_TX_NOMASK,
                G_TX_NOLOD, G_TX_NOLOD);
            x = (spF8 << 2) - menuXShift;
            for (var_t5 = 0; var_t5 < X_REPEAT_COUNT; var_t5++) {
                y = (spF4 << 2) - var_s4;
                for (j = 0; j < Y_REPEAT_COUNT; j++) {
                    gSPScisTextureRectangle(
                        dl++,
                        x,
                        y,
                        x + 0x80,
                        y + 0x100,
                        0,
                        0,
                        0x7FF,
                        1024,
                        -1024
                    );
                    y += 0x200;
                }
                x += 0x200;
            }
            spF8 += 0x20, spE6++;
        }
    }
    menuXShift += 4;
    menuYShift += 4;
    if (menuXShift >= 0x200) {
        menuXShift = 0;
    }
    if (menuYShift >= 0x200) {
        menuYShift = 0;
    }
    gEXPopScissor(dl++);
    gEXSetRectAlign(dl++, G_EX_ORIGIN_NONE, G_EX_ORIGIN_NONE, 0, 0, 0, 0);
    gDPPipeSync(dl++);
    gDPSetTexturePersp(dl++, G_TP_PERSP);
    gDPSetTextureFilter(dl++, G_TF_BILERP);
    return dl;
}

// @recomp: Tag Barrel Kickout
void func_global_asm_80682AB4(void);
void func_global_asm_80682DF4(void*, PlayerAdditionalActorData *, u8);
void func_global_asm_806C8E58(s16);
u8 func_global_asm_805FCA64(void);
void func_global_asm_80602B60(s32 arg0, u8 arg1);
s32 deleteActor(Actor*);
void func_global_asm_806F09F0(Actor *arg0, u16 arg1);
void func_global_asm_80627888(Actor *arg0);
void func_global_asm_80709464(u8 playerIndex);
s16 playSound(s16 arg0, s32 arg1, f32 arg2, f32 arg3, u8 arg4, u8 arg5);
extern s32 D_global_asm_8074E848[];

s16 func_global_asm_8070C200(f32);

extern TexEntry D_global_asm_80754F80[];
extern u16 D_global_asm_80754FC8[];

typedef struct Struct80709BC4 {
    s32 unk0;
    s32 unk4;
    tuple_f *unk8;
    Struct807FD9A8_unkC *unkC;
    Vtx *unk10[2];
    s32 *unk18;
    s32 *unk1C;
    s32 *unk20;
} Struct80709BC4;

// @recomp: Zipper renderer
RECOMP_PATCH void func_global_asm_80709BC4(Gfx ** dl_ptr, Struct80709BC4* arg1, u16 *arg2) {
    s32 temp_s2;
    s32 temp_v1;
    s32 var_t4;
    s32 j;
    s32 var_fp;
    Gfx *dl;
    s32 i;
    s32 vtx_index; // 9c

    dl = *dl_ptr;
    for (j = 0; j < arg1->unk0; j++) {
        arg1->unk10[D_global_asm_807444FC][j].v.ob[0] = func_global_asm_8070C200(arg1->unk8[j].x);
        arg1->unk10[D_global_asm_807444FC][j].v.ob[1] = func_global_asm_8070C200(arg1->unk8[j].y);
        arg1->unk10[D_global_asm_807444FC][j].v.ob[2] = func_global_asm_8070C200(arg1->unk8[j].z);
    }
    var_t4 = 0;
    gDPSetCombineMode(dl++, G_CC_MODULATEI_PRIM, G_CC_MODULATEI_PRIM); // @recomp: Add this to disable an artifact with RT64's snapshot
    gDPSetPrimColor(dl++, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF); // @recomp: If I don't do this, then everything renders green-ish if chunky is not unlocked
    for (i = 0; arg1->unk18[i]; i++) {
        temp_s2 = arg1->unk20[i];
        var_fp = -0x63;
        vtx_index = (arg1->unk1C[i] - temp_s2) + 1;
        gSPVertex(dl++, osVirtualToPhysical(&arg1->unk10[D_global_asm_807444FC][temp_s2]), vtx_index, 0);
        for (j = 0; j < arg1->unk18[i]; j++) {
            temp_v1 = arg1->unkC[var_t4].unk18;
            if (temp_v1 != var_fp) {
                var_fp = temp_v1;
                if (temp_v1 < 0) {
                    temp_v1 = -1 - temp_v1;
                    gDPLoadTextureBlock(dl++,
                        &D_global_asm_80754FC8[D_global_asm_80754F80[temp_v1].offset],
                        G_IM_FMT_RGBA, G_IM_SIZ_16b,
                        D_global_asm_80754F80[temp_v1].width, D_global_asm_80754F80[temp_v1].height - 1,
                        0,
                        G_TX_NOMIRROR, G_TX_NOMIRROR,
                        G_TX_NOMASK,   G_TX_NOMASK,
                        G_TX_NOLOD,    G_TX_NOLOD);
                } else {
                    gDPLoadTextureTile(dl++,
                        &arg2[D_global_asm_807FD9B4[temp_v1].offset],
                        G_IM_FMT_RGBA, G_IM_SIZ_16b,
                        320, D_global_asm_807FD9B4[temp_v1].height,
                        0, 0,
                        D_global_asm_807FD9B4[temp_v1].width, D_global_asm_807FD9B4[temp_v1].height - 1,
                        0,
                        G_TX_NOMIRROR, G_TX_NOMIRROR,
                        G_TX_NOMASK,   G_TX_NOMASK,
                        G_TX_NOLOD,    G_TX_NOLOD);
                }
            }
            gSPModifyVertex(dl++, arg1->unkC[var_t4].unk0 - temp_s2, G_MWO_POINT_ST, arg1->unkC[var_t4].unkC);
            gSPModifyVertex(dl++, arg1->unkC[var_t4].unk4 - temp_s2, G_MWO_POINT_ST, arg1->unkC[var_t4].unk10);
            gSPModifyVertex(dl++, arg1->unkC[var_t4].unk8 - temp_s2, G_MWO_POINT_ST, arg1->unkC[var_t4].unk14);
            gSP1Triangle(dl++, arg1->unkC[var_t4].unk0 - temp_s2, arg1->unkC[var_t4].unk4 - temp_s2, arg1->unkC[var_t4].unk8 - temp_s2, 0);
            var_t4++;
        }
    }
    *dl_ptr = dl;
}

typedef struct Struct80754ED8 {
    void *unk0;
    u8 pad4[0x22 - 0x4];
    s16 unk22;
} Struct80754ED8;

extern Struct80754ED8 *D_global_asm_80754ED8[];
extern rgb D_global_asm_80754EF8[8][4];
extern f32 D_global_asm_807FD968;
extern f32 D_global_asm_807FD96C;
extern f32 D_global_asm_807FD970;
extern Actor *D_global_asm_807F5D10;
extern Actor *gPlayerPointer;
extern Mtx D_2000080;
extern Mtx D_2000180;
extern void func_global_asm_80612CA0(f32 (*arg0)[4], f32 arg1);
extern f32 func_global_asm_80612D10(f32 arg0);

#define GPACK_RGBA32(r, g, b, a) (((u8)(r) << 0x18) | ((u8)(g) << 0x10) | ((u8)(b) << 0x8) | (u8)(a))
extern u8 skip_interpolation;
extern u8 skip_persp_interp;

// @recomp: Render skybox blend
RECOMP_PATCH Gfx* func_global_asm_80704B20(Gfx* dl, f32 arg1, f32 arg2, Mtx* arg3, u8 arg4, u8 arg5, u8 arg6, s8 arg7, f32 arg8) {
    f32 temp_f2;
    s32 sp140;
    s32 sp13C;
    s32 sp138;
    s32 sp134;
    f32 sp130;
    f32 sp12C;
    f32 sp128;
    void** temp_t3;
    f32 spE4[4][4];
    f32 spA4[4][4];
    s32 temp_t2;
    CameraPaad* var_v1;

    temp_f2 = -(256.0f - (func_global_asm_80612D10(arg2) * 300.0f)) - 80.0f;
    var_v1 = D_global_asm_807F5D10->CaaD;
    if (arg4 == 5) {
        temp_f2 += ((0.08 * character_change_array->unk2CA) - 75.0);
    }
    switch (arg6) {
        case 0:
            sp128 = 1.0f;
            sp12C = 1.0f;
            sp130 = 1.0f;
            break;
        case 1:
            if (D_global_asm_807FD968 >= 0.0f) {
                sp130 = D_global_asm_807FD968;
                sp12C = D_global_asm_807FD96C;
                sp128 = D_global_asm_807FD970;
            } else {
                if (arg7 == -1) {
                    arg7 = gPlayerPointer->unk12C;
                }
                func_global_asm_80659620(&sp130, &sp12C, &sp128, arg7);
            }
            break;
    }
    if ((var_v1->unkFA != 0) && (character_change_array->unk220 < (var_v1->unk90 + 3.0f))) {
        sp130 *= 0.1;
        sp12C *= 0.1;
        sp128 *= 0.3;
    }
    sp140 = GPACK_RGBA32(
        sp130 * (D_global_asm_80754EF8[arg4][0].red + (arg8 * (D_global_asm_80754EF8[arg5][0].red - D_global_asm_80754EF8[arg4][0].red))),
        sp12C * (D_global_asm_80754EF8[arg4][0].green + (arg8 * (D_global_asm_80754EF8[arg5][0].green - D_global_asm_80754EF8[arg4][0].green))),
        sp128 * (D_global_asm_80754EF8[arg4][0].blue + (arg8 * (D_global_asm_80754EF8[arg5][0].blue - D_global_asm_80754EF8[arg4][0].blue))),
        0xFF);
    sp13C = GPACK_RGBA32(
        sp130 * (D_global_asm_80754EF8[arg4][1].red + (arg8 * (D_global_asm_80754EF8[arg5][1].red - D_global_asm_80754EF8[arg4][1].red))),
        sp12C * (D_global_asm_80754EF8[arg4][1].green + (arg8 * (D_global_asm_80754EF8[arg5][1].green - D_global_asm_80754EF8[arg4][1].green))),
        sp128 * (D_global_asm_80754EF8[arg4][1].blue + (arg8 * (D_global_asm_80754EF8[arg5][1].blue - D_global_asm_80754EF8[arg4][1].blue))),
        0xFF);
    sp138 = GPACK_RGBA32(
        sp130 * (D_global_asm_80754EF8[arg4][2].red + (arg8 * (D_global_asm_80754EF8[arg5][2].red - D_global_asm_80754EF8[arg4][2].red))),
        sp12C * (D_global_asm_80754EF8[arg4][2].green + (arg8 * (D_global_asm_80754EF8[arg5][2].green - D_global_asm_80754EF8[arg4][2].green))),
        sp128 * (D_global_asm_80754EF8[arg4][2].blue + (arg8 * (D_global_asm_80754EF8[arg5][2].blue - D_global_asm_80754EF8[arg4][2].blue))),
        0xFF);
    sp134 = GPACK_RGBA32(
        sp130 * (D_global_asm_80754EF8[arg4][3].red + (arg8 * (D_global_asm_80754EF8[arg5][3].red - D_global_asm_80754EF8[arg4][3].red))),
        sp12C * (D_global_asm_80754EF8[arg4][3].green + (arg8 * (D_global_asm_80754EF8[arg5][3].green - D_global_asm_80754EF8[arg4][3].green))),
        sp128 * (D_global_asm_80754EF8[arg4][3].blue + (arg8 * (D_global_asm_80754EF8[arg5][3].blue - D_global_asm_80754EF8[arg4][3].blue))),
        0xFF);
    gDPPipeSync(dl++);
    gDPSetRenderMode(dl++, G_RM_AA_OPA_SURF, G_RM_AA_OPA_SURF2);
    gDPSetCombineMode(dl++, G_CC_SHADE, G_CC_SHADE);
    gDPSetCycleType(dl++, G_CYC_1CYCLE);
    gSPLoadGeometryMode(dl++, 0);
    gSPSetGeometryMode(dl++, G_SHADE | G_SHADING_SMOOTH);
    gEXMatrixGroup(dl++, MTXTAG_SKYBOXBLEND, G_EX_INTERPOLATE_SIMPLE, G_EX_NOPUSH, G_MTX_PROJECTION, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_SKIP, G_EX_COMPONENT_INTERPOLATE, G_EX_ORDER_LINEAR, G_EX_EDIT_NONE, G_EX_ASPECT_STRETCH, G_EX_COMPONENT_SKIP, G_EX_COMPONENT_AUTO);
    gSPMatrix(dl++, &D_2000080, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_PROJECTION);
    gSPMatrix(dl++, &D_2000180, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPViewport(dl++, osVirtualToPhysical(&character_change_array->unk250[D_global_asm_807444FC]));
    gDPSetScissor(dl++, G_SC_NON_INTERLACE,
        character_change_array->unk270[0],
        character_change_array->unk270[1],
        character_change_array->unk270[2],
        character_change_array->unk270[3]
    );
    temp_t2 = D_global_asm_80754ED8[arg4]->unk22;
    guTranslateF(spE4, 0.0f, -temp_t2, 0.0f);
    func_global_asm_80612CA0(spA4, ((s32) (((0x1000 - character_change_array->unk2CC) & 0xFFF) * 0x168) / 4096));
    guMtxCatF(spE4, spA4, spE4);
    guTranslateF(spA4, 160.0f, temp_t2 + temp_f2, 0.0f);
    guMtxCatF(spE4, spA4, spE4);
    guMtxF2L(spE4, arg3);
    gSPMatrix(dl++, arg3, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPVertex(dl++, osVirtualToPhysical(D_global_asm_80754ED8[arg4]), 8, 0);
    gSPModifyVertex(dl++, 0, G_MWO_POINT_RGBA, sp140);
    gSPModifyVertex(dl++, 1, G_MWO_POINT_RGBA, sp140);
    gSPModifyVertex(dl++, 2, G_MWO_POINT_RGBA, sp13C);
    gSPModifyVertex(dl++, 3, G_MWO_POINT_RGBA, sp13C);
    gSPModifyVertex(dl++, 4, G_MWO_POINT_RGBA, sp138);
    gSPModifyVertex(dl++, 5, G_MWO_POINT_RGBA, sp138);
    gSPModifyVertex(dl++, 6, G_MWO_POINT_RGBA, sp134);
    gSPModifyVertex(dl++, 7, G_MWO_POINT_RGBA, sp134);
    gSP2Triangles(dl++, 0, 1, 3, 0, 0, 3, 2, 0);
    gSP2Triangles(dl++, 2, 3, 4, 0, 2, 4, 5, 0);
    gSP2Triangles(dl++, 5, 4, 6, 0, 5, 6, 7, 0);
    if (skip_interpolation || skip_persp_interp) {
        gEXMatrixGroupSkipAllAspect(dl++, MTXTAG_SKYBOXBLEND, G_EX_NOPUSH, G_MTX_PROJECTION, G_EX_EDIT_NONE, G_EX_ASPECT_AUTO);
    } else {
        gEXMatrixGroup(dl++, MTXTAG_SKYBOXBLEND, G_EX_INTERPOLATE_SIMPLE, G_EX_NOPUSH, G_MTX_PROJECTION, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_SKIP, G_EX_COMPONENT_INTERPOLATE, G_EX_ORDER_LINEAR, G_EX_EDIT_NONE, G_EX_ASPECT_AUTO, G_EX_COMPONENT_SKIP, G_EX_COMPONENT_AUTO);
    }
    gDPPipeSync(dl++);
    return dl;
}

typedef struct Struct807069A4_12C {
    f32 unk0;
    f32 unk4;
    f32 unk8;
    f32 unkC;
    f32 unk10;
    f32 unk14;
} Struct807069A4_12C;

void func_global_asm_807063B8(Struct807069A4_12C*, u8*, s32, f32);
void func_global_asm_80702464(Gfx **dl, void *texture, s32 arg2, s32 arg3, s32 arg4, s32 arg5, s32 arg6, f32 arg7, f32 arg8, f32 arg9, f32 argA, f32 argB, f32 argC, u8 argD, u8 argE);
void func_global_asm_807065F8(s16, u8*, u8*, u8*);
extern f32 D_global_asm_80754CE4;
extern u16* D_global_asm_807FD8A0;
extern Mtx D_global_asm_807FD8A8[];
extern Mtx D_global_asm_807FD928;

// @recomp: Render a textured skybox (Az Beetle Race, bonuses)
RECOMP_PATCH Gfx* func_global_asm_807069A4(Gfx* dl, f32 arg1, f32 arg2, s32 arg3, f32 arg4, f32 arg5) {
    PlayerAdditionalActorData* PaaD;
    s32 i;
    f32 var_f2;
    Struct807069A4_12C sp12C[5];
    u8 sp12B;
    f32 var_f12;
    Actor* temp_s3;
    f32 spDC[4][4];
    f32 sp9C[4][4];
    u8 sp9B;
    u8 sp9A;
    u8 sp99;
    f32 temp_f0; // 94
    s32 width, height;
    f32 upscale_ratio;

    recomp_get_ui_bounds(&width, &height);
    upscale_ratio = (f32)(width * 3) / (f32)(height * 4);  // x0.75 == / (4/3)

    temp_f0 = 5.6f / D_global_asm_80754CE4;
    temp_s3 = character_change_array->playerPointer;
    PaaD = temp_s3->PaaD;
    guScaleF(spDC, temp_f0, temp_f0, temp_f0);
    guTranslateF(sp9C, -2560.0f * upscale_ratio, -1920.0f, 0.0f);
    guMtxCatF(spDC, sp9C, spDC);
    guRotateF(sp9C, (f32) ((0x1000 - PaaD->unk104->x_rotation) & 0xFFF) * 0.087890625f, 0.0f, 0.0f, 1.0f);
    guMtxCatF(spDC, sp9C, spDC);
    guTranslateF(sp9C, 2560.0f * upscale_ratio, 1920.0f, 0.0f);
    guMtxCatF(spDC, sp9C, spDC);
    guMtxF2L(spDC, &D_global_asm_807FD8A8[D_global_asm_807444FC]);
    arg1 = 6.2831855f - arg1;
    arg2 = (arg4 / arg5) * (6.2831855f - arg2);
    if (D_global_asm_807FD8A0 == NULL) {
        D_global_asm_807FD8A0 = getPointerTableFile(TABLE_14_TEXTURES_HUD, arg3, 0U, 0U);
    }
    gEXMatrixGroup(dl++, MTXTAG_SKYBOXBLEND, G_EX_INTERPOLATE_SIMPLE, G_EX_NOPUSH, G_MTX_PROJECTION, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_SKIP, G_EX_COMPONENT_INTERPOLATE, G_EX_ORDER_LINEAR, G_EX_EDIT_NONE, G_EX_ASPECT_STRETCH, G_EX_COMPONENT_SKIP, G_EX_COMPONENT_AUTO);
    gSPMatrix(dl++, &D_global_asm_807FD8A8[D_global_asm_807444FC], G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPMatrix(dl++, &D_global_asm_807FD928, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_PROJECTION);
    gDPPipeSync(dl++);
    gDPSetRenderMode(dl++, G_RM_XLU_SURF, G_RM_NOOP2);
    gDPSetCombineMode(dl++, G_CC_MODULATEI_PRIM, G_CC_MODULATEI_PRIM);
    gDPSetCycleType(dl++, G_CYC_1CYCLE);
    func_global_asm_807065F8(temp_s3->y_position, &sp9B, &sp9A, &sp99);
    gDPSetPrimColor(dl++, 0, 0, sp9B, sp9A, sp99, 0xFF);
    gSPLoadGeometryMode(dl++, 0);
    gSPSetGeometryMode(dl++, G_SHADE | G_SHADING_SMOOTH);
    var_f2 = arg1 * 0.15915494f * D_global_asm_80754CE4;
    var_f12 = arg2 * 0.15915494f * D_global_asm_80754CE4;
    while (var_f2 > 1.0f) {
        var_f2 -= 1.0f;
    }
    while (var_f12 > 1.0f) {
        var_f12 -= 1.0f;
    }
    var_f2 *= arg4;
    var_f12 *= arg5;
    sp12B = 1;
    sp12C[0].unk0 = var_f2 - (460.0f * (1.0f / temp_f0) * 0.5f);
    sp12C[0].unk8 = (460.0f * (1.0f / temp_f0) * 0.5f) + var_f2;
    sp12C[0].unk4 = var_f12 - (380.0f * (1.0f / temp_f0) * 0.5f);
    sp12C[0].unkC = (380.0f * (1.0f / temp_f0) * 0.5f) + var_f12;
    sp12C[0].unk10 = -70.0f * (1.0f / temp_f0);
    sp12C[0].unk14 = -70.0f * (1.0f / temp_f0);
    func_global_asm_807063B8(sp12C, &sp12B, 0, arg4);
    func_global_asm_807063B8(sp12C, &sp12B, 1, arg5);
    for (i = 0; i < sp12B; i++) {
        func_global_asm_80702464(&dl, D_global_asm_807FD8A0, 0,
            (u32) arg4,
            (u32) arg5,
            0x20, 0x20,
            sp12C[i].unk0, sp12C[i].unk4, sp12C[i].unk8,
            sp12C[i].unkC, sp12C[i].unk10, sp12C[i].unk14,
            0x10U, 0x10U);
    }
    if (skip_interpolation || skip_persp_interp) {
        gEXMatrixGroupSkipAllAspect(dl++, MTXTAG_SKYBOXBLEND, G_EX_NOPUSH, G_MTX_PROJECTION, G_EX_EDIT_NONE, G_EX_ASPECT_AUTO);
    } else {
        gEXMatrixGroup(dl++, MTXTAG_SKYBOXBLEND, G_EX_INTERPOLATE_SIMPLE, G_EX_NOPUSH, G_MTX_PROJECTION, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_SKIP, G_EX_COMPONENT_INTERPOLATE, G_EX_ORDER_LINEAR, G_EX_EDIT_NONE, G_EX_ASPECT_AUTO, G_EX_COMPONENT_SKIP, G_EX_COMPONENT_AUTO);
    }
    return dl;
}

Gfx* func_global_asm_80705F5C(Gfx*, s16, s16, s16);
Gfx *func_global_asm_8070770C(Gfx *);
void func_global_asm_80705C00(s16 arg0, s16 arg1, u8 arg2);
void func_global_asm_8068B830(s16 arg0, s16 arg1, s16 arg2);
void func_global_asm_8068B8A4(f32 arg0);
void func_global_asm_8068B8FC(void);
extern f32 D_global_asm_80754CE8;
extern s16 D_global_asm_807FD800;
extern f32 loading_zone_transition_speed;
extern u8 loading_zone_transition_type;

// @recomp: Skybox manager
RECOMP_PATCH Gfx* func_global_asm_80707980(Gfx* dl, f32 arg1, f32 arg2, Mtx *arg3, s16 arg4) {
    f32 temp_f0;
    f32 var_f2;
    f32 temp_f2;
    f32 var_f14;
    f32 var_f16;
    f64 temp_f12;
    s16 temp_v0;
    s32 var_v0;

    gDPPipeSync(dl++);
    if ((loading_zone_transition_speed != 0.0f) && (loading_zone_transition_type == 3)) {
        dl = func_global_asm_8070770C(dl);
    }
    switch (current_map) {
        case MAP_FUNGI_DOGADON:
        case MAP_FACTORY_MAD_JACK:
        case MAP_AZTEC_DOGADON:
        case MAP_TRAINING_GROUNDS_END_SEQUENCE:
            return dl;
        case MAP_AZTEC_BEETLE_RACE:
            dl = func_global_asm_807069A4(dl, arg1, arg2, 0x2D, 320.0f, 240.0f);
            break;
        case MAP_KROOL_BARREL_LANKY_MAZE:
        case MAP_STEALTHY_SNOOP_NORMAL_NO_LOGO:
        case MAP_STEALTHY_SNOOP_NORMAL:
        case MAP_MAD_MAZE_MAUL_HARD:
        case MAP_STASH_SNATCH_NORMAL:
        case MAP_MAD_MAZE_MAUL_EASY:
        case MAP_MAD_MAZE_MAUL_NORMAL:
        case MAP_STASH_SNATCH_EASY:
        case MAP_STASH_SNATCH_HARD:
        case MAP_MAD_MAZE_MAUL_INSANE:
        case MAP_STASH_SNATCH_INSANE:
        case MAP_STEALTHY_SNOOP_VERY_EASY:
        case MAP_STEALTHY_SNOOP_EASY:
        case MAP_STEALTHY_SNOOP_HARD:
            dl = func_global_asm_807069A4(dl, arg1, arg2, 0x2E, 320.0f, 240.0f);
            break;
        case MAP_AZTEC:
            switch (character_change_array->chunk) {
                case 0:
                case 1:
                case 3:
                case 6:
                case 8:
                case 10:
                    dl = func_global_asm_8070770C(dl);
                    break;
                default:
                    func_global_asm_80705C00(0x3E8, 0x4E20, 0U);
                    dl = func_global_asm_80704B20(dl, arg1, arg2, arg3, 0, 0, 0, -1, 0.0f);
                    break;
            }
            break;
        case MAP_GALLEON:
            temp_v0 = character_change_array->chunk;
            // @recomp: Change chunk checks
            // if (((temp_v0 != 7) && (temp_v0 != 6) && (temp_v0 != 8) && (temp_v0 != 0)) || (D_global_asm_807FD800 != 0)) {
                if ((gPlayerPointer->unk12C == 9) || (gPlayerPointer->unk12C == 0xB) || (gPlayerPointer->unk12C == 3)) {
                    var_v0 = 1;
                } else {
                    var_v0 = 0;
                }
                dl = func_global_asm_80704B20(dl, arg1, arg2, arg3, 0, 0, var_v0, 9, 0.0f);
            // }
            break;
        case MAP_GALLEON_SEAL_RACE:
            func_global_asm_80705C00(0xBB8, 0x2328, 0U);
            dl = func_global_asm_80704B20(dl, arg1, arg2, arg3, 0, 0, 0, -1, 0.0f);
            break;
        case MAP_JAPES_MOUNTAIN:
            dl = func_global_asm_8070770C(dl);
            break;
        case MAP_TRAINING_GROUNDS:
            dl = func_global_asm_8070770C(dl);
            break;
        case MAP_JAPES:
            switch (character_change_array->chunk) {
            case 8:
            case 9:
            case 12:
            case 16:
                dl = func_global_asm_8070770C(dl);
                break;
            case 11:
            case 14:
                func_global_asm_80705C00(0, 0x7530, 1U);
                dl = func_global_asm_80704B20(dl, arg1, arg2, arg3, 4, 4, 1, -1, 0.0f);
                break;
            default:
                func_global_asm_80705C00(0, 0x7530, 0U);
                dl = func_global_asm_80704B20(dl, arg1, arg2, arg3, 4, 4, 0, -1, 0.0f);
                break;
            }
            break;
        case MAP_JAPES_ARMY_DILLO:
            dl = func_global_asm_80704B20(dl, arg1, arg2, arg3, 3, 3, 1, -1, 0.0f);
            dl = func_global_asm_80705F5C(dl, 0, 0x7D00, 0);
            break;
        case MAP_FUNGI:
            if ((character_change_array->chunk >= 0xC) && (character_change_array->chunk < 0x12)) {
                dl = func_global_asm_8070770C(dl);
            } else {
                func_global_asm_80705C00(0, 0x5DC0, 1U);
                dl = func_global_asm_80704B20(dl, arg1, arg2, arg3, 2, 2, 1, -1, 0.0f);
                dl = func_global_asm_80705F5C(dl, 0, 0x5DC0, 1);
            }
            break;
        case MAP_FUNGI_MINECART:
            dl = func_global_asm_80704B20(dl, arg1, arg2, arg3, 1, 1, 1, -1, 0.0f);
            break;
        case MAP_DK_ISLES_OVERWORLD:
            if (func_global_asm_8061CB50() != 0) {
                var_f14 = character_change_array->look_at_eye[0];
                var_f16 = character_change_array->look_at_eye[2];
            } else {
                var_f14 = gPlayerPointer->position.f[0];
                var_f16 = gPlayerPointer->position.f[2];
            }
            temp_f0 = var_f14 - 3000.0f;
            temp_f2 = var_f16 - 5000.0f;
            temp_f0 = _sqrtf(SQ(temp_f0) + SQ(temp_f2));
            if (temp_f0 < 2500.0f) {
                temp_f12 = (f64) ((2500.0f - temp_f0) / 1000.0f);
                var_f2 = MIN(1.0, temp_f12);
                func_global_asm_8068B830((s16) (s32) (2.0f + var_f2), (s16) (s32) (var_f2 * 400.0f), (s16) (s32) (var_f2 * 30.0f));
                func_global_asm_8068B8A4((f32) (((f64) var_f2 * -0.6) + 1.0));
            } else {
                func_global_asm_8068B8FC();
                func_global_asm_8068B8A4(1.0f);
            }
            func_global_asm_80705C00(0xBB8, 0x4E20, 0U);
            dl = func_global_asm_80704B20(dl, arg1, arg2, arg3, 5, 6, 1, -1, 0.0f);
            break;
        case MAP_DK_ISLES_DK_THEATRE:
        case MAP_ROCK_INTRO_STORY:
            func_global_asm_80705C00(0xBB8, 0x4E20, 0U);
            dl = func_global_asm_80704B20(dl, arg1, arg2, arg3, 5, 5, 1, -1, 0.0f);
            break;
        case MAP_GALLEON_PUFFTOSS:
            dl = func_global_asm_80704B20(dl, arg1, arg2, arg3, 6, 6, 1, -1, 0.0f);
            dl = func_global_asm_80705F5C(dl, 0xC8, 0x4268, 0);
            break;
        case MAP_CASTLE:
            dl = func_global_asm_80704B20(dl, arg1, arg2, arg3, 6, 6, 1, -1, 0.0f);
            dl = func_global_asm_80705F5C(dl, 0x3E8, 0x2EE0, 0);
            break;
        case MAP_KLUMSY_ENDING:
            func_global_asm_80705C00(0x8FC, 0x1388, 0U);
            dl = func_global_asm_80704B20(dl, arg1, arg2, arg3, 7, 7, 0, -1, 0.0f);
            break;
        case MAP_BLOOPERS_ENDING:
            gDPSetFillColor(dl++, 0xFFFFFFFF);
            goto block_55;
        case MAP_GALLEON_BARREL_BLAST:
            gDPSetFillColor(dl++, 0xFFC1FFC1);
            goto block_55;
        case MAP_MAIN_MENU:
            func_global_asm_80705C00(0x8FC, 0x1388, 1U);
            dl = func_global_asm_80704B20(dl, arg1, arg2, arg3, 7, 6, 0, -1, D_global_asm_80754CE8);
            dl = func_global_asm_80705F5C(dl, 0xC8, 0x2EE0, 2);
            break;
            
        default:
            gDPSetFillColor(dl++, 0x00010001);
    block_55:
            gDPSetRenderMode(dl++, G_RM_NOOP, G_RM_NOOP2);
            gDPSetCycleType(dl++, G_CYC_FILL);
            // @recomp: remove the -1
            gDPFillRectangle(dl++,
                character_change_array[arg4].unk270[0],
                character_change_array[arg4].unk270[1],
                character_change_array[arg4].unk270[2],
                character_change_array[arg4].unk270[3]
            );
            gDPPipeSync(dl++);
            gDPSetCycleType(dl++, G_CYC_1CYCLE);
            break;
    }
    D_global_asm_80754CE8 = 0.0f;
    gDPPipeSync(dl++);
    return dl;
}