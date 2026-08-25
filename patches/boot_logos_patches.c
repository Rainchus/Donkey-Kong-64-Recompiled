#include "common_structs.h"
#include "options.h"
#include "patches_main.h"

Gfx gfxArena[0x200];
u8 redNintendoLogoTexture[320 * 240 * 2];

void func_global_asm_80611730(void);
u8* getTextString(u8 fileIndex, s32 stringIndex, s32 arg2);
u8 func_global_asm_806FD894(s16 arg0);
extern Gfx** D_1000118;
extern Mtx D_20000C0;
extern s16 D_global_asm_80744494;
extern s16 D_global_asm_80744490;
void* func_global_asm_8068C12C(u16 textureIndex);
Gfx* printStyledText(Gfx* dl, s16 style, s16 x, s16 y, u8* string, u32 extraBitfield);
s32 getCenterOfString(s16 renderStyle, u8* string);
Gfx* printText(Gfx* dl, s16 x, s16 y, f32 scale, u8* string);
extern u8 D_global_asm_8074450C;
extern u8 D_menu_800339D0_02175720;
extern s16 D_global_asm_807476F4;
RECOMP_DECLARE_EVENT(recomp_on_init());
RECOMP_DECLARE_EVENT(dk64recomp_every_frame());
#define TEXT_SCALE 0.25f
#define ORIGINAL_TEXT_SCALE 0.5f

#define gScissorUpLX D_global_asm_80744498
#define gScissorUpLY D_global_asm_8074449C
#define gScissorLowerRightX D_global_asm_807444A0
#define gScissorLowerRightY D_global_asm_807444A4
#define OVERSCAN_SIZE 0  // Normally 10

//@recomp: normally this is drawn at 480i, but rt64 doesn't support this. We swap the mode to 240p, then manually half the rare logo size when displaying
RECOMP_PATCH Gfx* func_menu_8003292C(Gfx* dl) {
    u8* spCC;
    u8* spC8;
    u8* spC4;
    s32 spC0;
    s32 sp48;
    s32 sp50;
    f32 temp_f6;
    s16 temp_s0_19;
    s32 temp_a1;
    s32 temp_a2;
    s32 temp_a2_2;
    s32 temp_t0;
    s32 temp_t9;
    s32 temp_t6;
    s32 var_a2;
    s32 x_offset, y_offset;
    s32 i, j;
    void* temp_v0;

    spCC = getTextString(0xD, 0, 1);
    spC8 = getTextString(0xD, 1, 1);
    spC4 = getTextString(0xD, 2, 1);
    spC0 = func_global_asm_806FD894(1);

    gEXEnable(dl++);

    gDPPipeSync(dl++);
    gSPDisplayList(dl++, &D_1000118);
    gDPSetRenderMode(dl++, G_RM_OPA_SURF, G_RM_OPA_SURF2);
    gDPSetCombineMode(dl++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
    gSPMatrix(dl++, &D_20000C0, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_PROJECTION);

    var_a2 = D_menu_800339D0_02175720 * 0xF;
    if (var_a2 >= 0x100) {
        var_a2 = 0xFF;
    }

    gDPSetPrimColor(dl++, 0, 0, var_a2, var_a2, var_a2, 0xFF);
    gDPSetTextureFilter(dl++, G_TF_POINT);
    gDPSetTexturePersp(dl++, G_TP_NONE);

    temp_v0 = func_global_asm_8068C12C(0xB0U);

    s32 half_width = 204 / 2;   // 102
    s32 half_height = 82 / 2;   // 41
    s32 center_x = (s32)((f32)(D_global_asm_80744490 - half_width) * 0.5f);
    s32 center_y = (s32)((f32)(D_global_asm_80744494 - half_height) * 0.5f);

    gEXSetRectAspect(dl++, G_EX_ASPECT_ADJUST);

    y_offset = center_y;
    for (i = 0; i < 0x52; i += 0x29) {
        temp_a1 = i + 0x29;
        temp_a2 = y_offset + (0x29 / 2);
        x_offset = center_x;
        for (j = 0; j < 0xCC; j += 0x66) {
            gDPLoadTextureTile_4b(dl++, temp_v0, G_IM_FMT_I, 204, 82,
                j, i, j + 0x66, temp_a1, 0,
                G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP,
                G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
            gSPTextureRectangle(dl++,
                x_offset << 2, y_offset << 2,
                (x_offset + (0x66 / 2)) << 2, temp_a2 << 2,
                G_TX_RENDERTILE,
                j << 5, i << 5,
                0x0800, 0x0800);
            x_offset += (0x66 / 2);
        }
        y_offset += (0x29 / 2);
    }


    gDPSetTextureFilter(dl++, G_TF_BILERP);
    gDPSetTexturePersp(dl++, G_TP_PERSP);

    temp_f6 = (f32)(D_global_asm_80744490 - getCenterOfString(1, spCC)) * 0.5f;

    gDPSetPrimColor(dl++, 0, 0, 0xFF, 0xFF, 0xFF, var_a2);
    gDPSetRenderMode(dl++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
    gDPSetCombineLERP(dl++, 0, 0, 0, TEXEL0, TEXEL0, 0, PRIMITIVE, 0,
        0, 0, 0, TEXEL0, TEXEL0, 0, PRIMITIVE, 0);

    gEXSetRectAspect(dl++, G_EX_ASPECT_AUTO);

    dl = printText(dl,
        (s16)((s32)temp_f6 * 4) + 330,
        (s16)((s32)(((f32)(D_global_asm_80744494 - 82.0f) * 0.5f) - (f32)spC0) * 4) + 105,
        TEXT_SCALE * 2, spCC);

    temp_a2_2 = D_global_asm_8074450C * 0xC8;
    temp_s0_19 = (s16)((s32)((f32)D_global_asm_80744490 * TEXT_SCALE) * 4);

    dl = printText(dl, temp_s0_19 + 330, temp_a2_2 * 4, 0.25f, spC8);
    dl = printText(dl, temp_s0_19 + 330, ((s32)(temp_a2_2 + (spC0 * 0.5)) * 4) - 15, 0.25f, spC4);

    return dl;
}

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240

Gfx* gfx_draw_textured_rect(Gfx* gfx, int x, int y, int width, int height, u8* texture) {
    gDPPipeSync(gfx++);
    gDPSetCycleType(gfx++, G_CYC_1CYCLE);
    gDPSetTextureFilter(gfx++, G_TF_BILERP);
    gDPSetCombineMode(gfx++, G_CC_DECALRGBA, G_CC_DECALRGBA);
    gDPSetRenderMode(gfx++, G_RM_AA_TEX_EDGE, G_RM_AA_TEX_EDGE2);

#define TILE_WIDTH 64
#define TILE_HEIGHT 32

    for (int tile_y = 0; tile_y < height; tile_y += TILE_HEIGHT) {
        int cur_h = tile_y + TILE_HEIGHT > height ? height - tile_y : TILE_HEIGHT;

        for (int tile_x = 0; tile_x < width; tile_x += TILE_WIDTH) {
            int cur_w = tile_x + TILE_WIDTH > width ? width - tile_x : TILE_WIDTH;

            u8* tile_texture = texture + ((tile_y * width) + tile_x) * 2;

            gDPLoadTextureTile(gfx++, tile_texture, G_IM_FMT_RGBA, G_IM_SIZ_16b,
                width, cur_h,
                0, 0, cur_w - 1, cur_h - 1,
                0,
                G_TX_CLAMP, G_TX_CLAMP,
                G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

            gSPTextureRectangle(gfx++,
                (x + tile_x) << 2, (y + tile_y) << 2,
                (x + tile_x + cur_w) << 2, (y + tile_y + cur_h) << 2,
                G_TX_RENDERTILE,
                0, 0, 1 << 10, 1 << 10);
        }
    }

    gDPPipeSync(gfx++);
    return gfx;

#undef TILE_WIDTH
#undef TILE_HEIGHT
}

typedef struct {
    /* 0x00 */ u8 unk0;
    /* 0x01 */ u8 unk1;
    /* 0x02 */ u8 unk2;
    /* 0x03 */ u8 unk3;
    /* 0x04 */ s16 unk4_arr[4];
} Struct80750948; //sizeof 0xC
extern Struct80750948 D_global_asm_80750948[];
extern Actor *gCurrentActorPointer;

void fixMPBoundary(s16 *addr, s32 target, s32 grace) {
    if (*addr >= (target - grace)) {
        if (*addr <= (target + grace)) {
            *addr = target;
        }
    }
}

extern void clear_actor_interpolation_lockdowns(void);
RECOMP_PATCH void func_global_asm_805FBFF4(s32 arg0) {
    s32 phi_s4;
    OSMesg* sp38;
    OSTimer timer;
    OSMesgQueue mq;
    OSMesg mq_buf;
    Gfx* gfxPos = gfxArena;

    recomp_printf("func_global_asm_805FBFF4 Started:\n");
    phi_s4 = 1;
    if (osTvType == OS_TV_PAL) {
        D_global_asm_807444BC = 1.25f;
    }

    func_global_asm_805FBC5C();
    osViSetSpecialFeatures(VI_CTRL_TYPE_16 | VI_CTRL_SERRATE_ON);
    func_global_asm_805FBE04();
    D_global_asm_8076A070 = D_global_asm_80767CC0 - 2;

    osRecvMesg(&D_global_asm_8076A110, (void*)&sp38, 1);

    //@recomp: create a queue so we can have a timer wait 1.3 seconds
    osCreateMesgQueue(&mq, &mq_buf, 1);

    //@recomp: wait 1.3 seconds, now things can actually be drawn in present early mode
    osSetTimer(&timer, OS_USEC_TO_CYCLES(1300000), 0, &mq, NULL);
    osRecvMesg(&mq, NULL, OS_MESG_BLOCK);

    //@recomp: draw red nintendo logo with proper display list instead of just writing to the framebuffer directly
    //TODO: is waiting like this OK for linux? check this
    gDPPipeSync(gfxPos++);
    gEXEnable(gfxPos++);
    gEXSetRectAspect(gfxPos++, G_EX_ASPECT_ADJUST);
    gDPSetColorImage(gfxPos++, G_IM_FMT_RGBA, G_IM_SIZ_16b, 320, 0x80034000);
    gDPSetScissor(gfxPos++, G_SC_NON_INTERLACE, 0, 0, 320, 240);
    gfxPos = gfx_draw_textured_rect(gfxPos, 0, 0, 320, 240, (void*)redNintendoLogoTexture);
    gDPFullSync(gfxPos++);
    gSPEndDisplayList(gfxPos++);
    func_global_asm_80610044(&gfxArena, ((u32)gfxPos - (u32)gfxArena) / sizeof(Gfx), 3, 1, 0x4D2, 1);
    D_global_asm_807444FC ^= 1;

    //@recomp: play DK "OK" sound
    playSound(0x23C, 0x7FFF, 63.0f, 1.0f, 0, 0);

    //@recomp: wait on graphics to finish
    osRecvMesg(&D_global_asm_807659E8.mq, NULL, OS_MESG_BLOCK);

    //@recomp: leave red nintendo logo on screen for 1.4 seconds, then continue
    osSetTimer(&timer, OS_USEC_TO_CYCLES(1400000), 0, &mq, NULL);
    osRecvMesg(&mq, NULL, OS_MESG_BLOCK);

    // @recomp Register actor extension data and call the init event.
    recomp_on_init();

    while (TRUE) {
        //@recomp patch viewports for widescreen (they get reloaded sometimes so patching each frame is easiest before it's used)
        for (int i = 0; i < 30; i++) {
            Struct80750948* x = &D_global_asm_80750948[i];
            if (x->unk4_arr[0] <= 12) {
                x->unk4_arr[0] = 0;
            }
            if (x->unk4_arr[1] <= 12) {
                x->unk4_arr[1] = 0;
            }
            if (x->unk4_arr[2] >= 307) {
                x->unk4_arr[2] = 320;
            }
            if (x->unk4_arr[3] >= 227) {
                x->unk4_arr[3] = 240;
            }
            fixMPBoundary(&x->unk4_arr[0], 160, 5);
            fixMPBoundary(&x->unk4_arr[1], 120, 5);
            fixMPBoundary(&x->unk4_arr[2], 160, 5);
            fixMPBoundary(&x->unk4_arr[3], 120, 5);
        }
        D_global_asm_8074682C = 0xC8;

        //@recomp Update the cutscene border setting every frame instead of on map load
        s32 cutscene_border_size = recomp_get_cutscene_bordering();
        if (current_map == MAP_HELM_LEVEL_INTROS_GAME_OVER) {
            if (is_cutscene_active && ((D_global_asm_807476F4 >= 15) && (D_global_asm_807476F4 <= 22))) {
                // Is level intro
                cutscene_border_size = 40;
            }
        }
        switch (is_cutscene_active) {
            case 3:
            case 4:
                gScissorUpLX = 0;
                gScissorUpLY = 0;
                gScissorLowerRightY = (D_global_asm_8074450C * 240);
                break;
            default:
                gScissorUpLX = D_global_asm_8074450C * OVERSCAN_SIZE;
                gScissorUpLY = D_global_asm_8074450C * OVERSCAN_SIZE;
                gScissorLowerRightX = (D_global_asm_8074450C * (320 - OVERSCAN_SIZE));
                gScissorLowerRightY = (D_global_asm_8074450C * (240 - OVERSCAN_SIZE));
                break;
        }
        D_global_asm_807444AC = gScissorUpLY + (D_global_asm_8074450C * cutscene_border_size);
        D_global_asm_807444B0 = gScissorLowerRightY - (D_global_asm_8074450C * cutscene_border_size);
        D_global_asm_807444A8 = gScissorUpLY;
        D_global_asm_807444B4 = gScissorLowerRightY;


        while (D_global_asm_80744460) {}

        // @recomp: Fire per-frame event.
        dk64recomp_every_frame();

        if (D_global_asm_8076A0B1 & 1 && !D_global_asm_8076A0B2) {
            func_global_asm_805FE7FC();
            clear_actor_interpolation_lockdowns();  // @recomp: flush actor interp lockdowns to prevent it getting clogged with stale references
            if (D_global_asm_807444F8 == 2) {
                global_properties_bitfield |= 0x200;
                D_global_asm_80744504 = 8;
            }
        }
        //gEXEnable(); // @recomp

        switch (is_cutscene_active) {
        case 6:
            //gEXSetRefreshRate(, 60 / 2);
            func_global_asm_8070A934(next_map, next_exit);
            break;
        case 3:
            //gEXSetRefreshRate(, 60);
            func_arcade_80024000();
            break;
        case 4:
            //gEXSetRefreshRate(, 60);
            func_jetpac_80024000();
            break;
        case 5:
            break;
        default:
            //gEXSetRefreshRate(, 60 / 2);
#ifdef DEBUG_WARP
            menuMain();
#endif
            func_global_asm_805FC2B0();
            break;
        }

        func_global_asm_80600B10();
        func_global_asm_8066AF40();
        func_global_asm_80610268(0x4D2);
        if (D_global_asm_807F059C[0]) {
            func_global_asm_80610268(0x929);
        }
        func_global_asm_80600674(); // calculateLagBoost()
        if ((is_cutscene_active == 0) || (is_cutscene_active == 1) || (is_cutscene_active == 7)) {
            func_global_asm_80658CCC();
            func_global_asm_80700BF4();
        }
        func_global_asm_80611730();
        if (gStackCanary != 0x12345678) {
            raiseException(2, 0, 0, 0);
        }
        if (phi_s4) {
            osSendMesg((void*)D_global_asm_807655E0, (void*)0x309, OS_MESG_BLOCK);
            phi_s4 = 0;
        }
        if (D_global_asm_8076A0B1 & 1 && D_global_asm_807FD888 == 31.0f) {
            D_global_asm_8076A0B2--;
        }
        D_global_asm_807444F0 = is_cutscene_active;
    }
}

RECOMP_PATCH void func_global_asm_805FB7E4(void) {
    Gfx* dl;
    s32 empty;
    s32 sp58;
    s32* temp_s0;
    u16* var_v1; // 50
    u16* sp4C;
    s32 sp48;
    s32 sp44;
    s32 sp40;
    s32 sp3C;
    u32 sp38;
    u16* end;
    s32 y, x;

    var_v1 = D_global_asm_80744470[0];
    end = &var_v1[0x12C00];
    while (var_v1 < end) {
        *var_v1++ = 1;
    }

    temp_s0 = (s32*)&D_global_asm_80744470[1][0x6400];
    func_global_asm_805FB750(0x38, 0x10, temp_s0);
    func_global_asm_805FB750(temp_s0[0] + 0x178, 0x10, temp_s0);
    func_global_asm_805FB750(temp_s0[0], temp_s0[1] - temp_s0[0], temp_s0);
    sp3C = (s32)temp_s0;
    sp38 = (u32)D_global_asm_80744470[1]; //framebuffer pointer
    sp4C = (u16*)D_global_asm_80744470[1];
    func_dk64_boot_800024E0((void*)&sp3C, &sp38, &D_global_asm_80744470[1][0xAF00]); //dump red nintendo logo to framebuffer

    var_v1 = &D_global_asm_80744470[0][0x7840];
    for (y = 0; y < 0x30; y++) {
        for (x = 0; x < 0xC0; x++) {
            *var_v1++ = *sp4C++;
        }
        var_v1 += 0x80;
    }

    //@recomp copy nintendo logo out of framebuffer to another buffer to be displayed
    {
        u8* src = (u8*)&D_global_asm_80744470[0][0];
        for (y = 0; y < 320 * 240 * 2; y++) {
            redNintendoLogoTexture[y] = src[y];
        }
    }

    var_v1 = D_global_asm_80744470[1];
    end = &var_v1[0x12C00];
    while (var_v1 < end) {
        *var_v1++ = 1;
    }
}