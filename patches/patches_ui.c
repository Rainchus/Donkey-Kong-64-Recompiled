#include "common_structs.h"
#include "ui.h"
#include "patches_ui.h"


void setSpriteAlignment(u8 alignment) {
    D_global_asm_807FDB1D = alignment;
}

Gfx *alignHUDTopBottom(Gfx * dl, u8 alignment, s32 top, s32 bottom) {
    s32 margin_reduction = 0;
    if (alignment == 1) {
        return dl;
    }
    gEXPushScissor(dl++);
    gEXPushViewport(dl++);
    gEXSetScissor(dl++, G_SC_NON_INTERLACE, G_EX_ORIGIN_LEFT, G_EX_ORIGIN_RIGHT, 0, top, 0, bottom);
    if (alignment & ALIGN_POSITION_RIGHT) {
        // Right align
        gEXSetRectAlign(dl++, G_EX_ORIGIN_RIGHT, G_EX_ORIGIN_RIGHT,
            -(D_global_asm_80744490 - margin_reduction) * 4, 0,
            -(D_global_asm_80744490 - margin_reduction) * 4, 0);
        gEXSetViewportAlign(dl++, G_EX_ORIGIN_RIGHT, -(D_global_asm_80744490 - margin_reduction) * 4, 0);
    } else if (alignment & ALIGN_POSITION_LEFT) {
        gEXSetRectAlign(dl++, G_EX_ORIGIN_LEFT, G_EX_ORIGIN_LEFT, 0, -margin_reduction * 4, 0, -margin_reduction * 4);
        gEXSetViewportAlign(dl++, G_EX_ORIGIN_LEFT, 0, 0);
    }
    return dl;
}

Gfx *alignHUD(Gfx * dl, u8 alignment) {
    return alignHUDTopBottom(dl, alignment, 0, D_global_asm_80744494);
}

Gfx *alignHUDGameplay(Gfx *dl, u8 alignment) {
    if (alignment == 1) {
        return dl;
    }
    s32 margin_reduction = recomp_get_ui_pillar();
    gEXPushScissor(dl++);
    gEXPushViewport(dl++);
    gDPSetScissor(dl++, G_SC_NON_INTERLACE, 0, 0, D_global_asm_80744490, D_global_asm_80744494);
    if (alignment & ALIGN_POSITION_RIGHT) {
        // Right align
        gEXSetRectAlign(dl++, G_EX_ORIGIN_RIGHT, G_EX_ORIGIN_RIGHT,
            -(D_global_asm_80744490 - margin_reduction) * 4, 0,
            -(D_global_asm_80744490 - margin_reduction) * 4, 0);
        gEXSetViewportAlign(dl++, G_EX_ORIGIN_RIGHT, -(D_global_asm_80744490 - margin_reduction) * 4, 0);
    } else if (alignment & ALIGN_POSITION_LEFT) {
        gEXSetRectAlign(dl++, G_EX_ORIGIN_LEFT, G_EX_ORIGIN_LEFT, -margin_reduction * 4, 0, -margin_reduction * 4, 0);
        gEXSetViewportAlign(dl++, G_EX_ORIGIN_LEFT, -margin_reduction * 4, 0);
    }
    return dl;
}

Gfx *popHUD(Gfx *dl, u8 alignment) {
    if (alignment == 1) {
        return dl;
    }
    gEXPopScissor(dl++);
    gEXPopViewport(dl++);
    // Clear gEX
    gEXSetRectAlign(dl++, G_EX_ORIGIN_NONE, G_EX_ORIGIN_NONE, 0, 0, 0, 0);
    gEXSetViewportAlign(dl++, G_EX_ORIGIN_NONE, 0, 0);
    return dl;
}

extern Mtx identity_fixed_mtx;
extern s32 cur_drawn_model_transform_id;
extern s32 cur_model_transform_id_offset;
extern u8 disable_sprite_interpolation;
extern u8 cur_drawn_model_skip_interpolation;
Gfx *set_model_matrix_group(Gfx * dl, void *geo_list, u8 skip_rotation, u8 *pushed);
Gfx *pop_model_matrix_group(Gfx *dl);

//@recomp: Drawing sprite gfx function
RECOMP_PATCH Gfx * func_global_asm_80715E94(Struct80717D84* sprite, Gfx *dl, s16 arg2) {
    s32 i;
    u8 sp15B;
    u8 sp15A;
    u8 sp159;
    f32 sp154;
    f32 sp150;
    f32 sp14C;
    Gfx* temp_s0;
    u8 pushed_matrix_group;
    u8 raw_alignment;
    
    sp154 = 1.0f;
    sp150 = 1.0f;
    sp14C = 1.0f;
    if (sprite->unk38C & 0x50) {
        return dl;
    }
    if ((((gPlayerPointer->control_state == 0x83) || (gPlayerPointer->control_state == 0x67)) && !(sprite->unk38C & 0x20)) || (((global_properties_bitfield & 0x100002) == 0x100002) && !(sprite->unk38C & 0x100))) {
        return dl;
    }
    raw_alignment = sprite->unk36F;
    if (sprite->unk36F & ALIGN_HIDING_IN_OVERSCAN) {
        if (((sprite->unk340 * 0.25) < 11) || ((sprite->unk340 * 0.25) > 309)) {
            return dl;
        }
    }
    if (arg2 == -1) {
        arg2 = sprite->unk38A;
    }
    if (sprite->unk388 != -1) {
        if (raw_alignment != 0) {
            // @recomp: disable sprite culling
            if (!func_global_asm_806522CC(sprite->unk340 * 0.25, sprite->unk344 * 0.25, sprite->unk388)) {
                return dl;
            }
        } else {
            if (arg2 != sprite->unk388) {
                return dl;
            }
        }
    }
    if (raw_alignment) {
        dl = alignHUD(dl, raw_alignment);
    }
    temp_s0 = sprite->unk0[sprite->unk21++].unk0[D_global_asm_807444FC];
    // Mtx tag
    cur_drawn_model_transform_id = sprite->sprite_index;
    cur_model_transform_id_offset = 0;
    gSPMatrix(dl++, &identity_fixed_mtx, G_MTX_PUSH | G_MTX_MUL | G_MTX_MODELVIEW);
    cur_drawn_model_skip_interpolation = disable_sprite_interpolation;
    if (sprite->unk36F & ALIGN_NO_INTERP) {
        cur_drawn_model_skip_interpolation = TRUE;
    }
    dl = set_model_matrix_group(dl, NULL, FALSE, &pushed_matrix_group);
    cur_drawn_model_skip_interpolation = FALSE;
    //
    gSPDisplayList(dl++, osVirtualToPhysical(temp_s0));
    gDPPipeSync(temp_s0++);
    // Mtx untag
    gSPPopMatrix(dl++, G_MTX_MODELVIEW);
    if (pushed_matrix_group) {
        dl = pop_model_matrix_group(dl);
    }
    //
    gDPSetCycleType(temp_s0++, G_CYC_1CYCLE);
    gSPLoadGeometryMode(temp_s0++, 0);
    gDPSetColorDither(temp_s0++, G_CD_DISABLE);
    if (sprite->unk36E != 0) {
        gSPSetGeometryMode(temp_s0++, G_ZBUFFER | G_SHADE | G_SHADING_SMOOTH);
        gDPSetRenderMode(temp_s0++, G_RM_ZB_CLD_SURF, G_RM_ZB_CLD_SURF2);
    } else {
        gSPSetGeometryMode(temp_s0++, G_SHADE | G_SHADING_SMOOTH);
        gDPSetRenderMode(temp_s0++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
    }
    gSPTexture(temp_s0++, 0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON);
    gDPSetCombineMode(temp_s0++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
    if ((sprite->unk330->unk14 == 0) && (func_global_asm_80651B64(arg2) != 0)) {
        func_global_asm_8065C334(sprite->unk340, sprite->unk344, sprite->unk348, 0, &sp15B, &sp15A, &sp159, arg2);
        sp154 = (sp15B / 255.0);
        sp150 = (sp15A / 255.0);
        sp14C = (sp159 / 255.0);
    }
    gDPSetPrimColor(temp_s0++, 0, 0, sprite->unk36A * sp154, sprite->unk36B * sp150, sprite->unk36C * sp14C, sprite->unk36D);
    if (raw_alignment != 0) {
        gSPMatrix(temp_s0++, &D_20000C0, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_PROJECTION);
        gSPMatrix(temp_s0++, &D_2000180, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        temp_s0 = func_global_asm_805FD030(temp_s0);
        if (raw_alignment == 1) {
            if (sprite->unk388 == -1) {
                gDPSetScissor(temp_s0++, G_SC_NON_INTERLACE,
                    sprite->unk38E,
                    sprite->unk390,
                    sprite->unk392,
                    sprite->unk394
                );
            }
        }
    }
    gSPMatrix(temp_s0++, osVirtualToPhysical(sprite->unk128[cc_player_index][D_global_asm_807444FC]), G_MTX_PUSH | G_MTX_MUL | G_MTX_MODELVIEW);
    for (i = 0; i < sprite->unk380; i++) {
        if ((D_global_asm_807F6009 != 5) || (D_global_asm_807F600C != sprite->unk370[i])) {
            gDPPipeSync(temp_s0++);
            switch (sprite->unk330->unkB) {
                case 0:
                    gDPLoadTextureBlock_4b(temp_s0++, sprite->unk370[i],
                        sprite->unk330->unkA,
                        sprite->unk330->unkC, sprite->unk330->unkE,
    		            0,
                        G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP,
                        G_TX_NOMASK, G_TX_NOMASK,
                        G_TX_NOLOD, G_TX_NOLOD    
                    );
                    break;
                case 1:
                    gDPLoadTextureBlock(temp_s0++, sprite->unk370[i],
                        sprite->unk330->unkA, G_IM_SIZ_8b,
                        sprite->unk330->unkC, sprite->unk330->unkE,
    		            0,
                        G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP,
                        G_TX_NOMASK, G_TX_NOMASK,
                        G_TX_NOLOD, G_TX_NOLOD
                    );
                    break;
                case 2:
                    gDPLoadTextureBlock(temp_s0++, sprite->unk370[i],
                        sprite->unk330->unkA, G_IM_SIZ_16b,
                        sprite->unk330->unkC, sprite->unk330->unkE,
    		            0,
                        G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP,
                        G_TX_NOMASK, G_TX_NOMASK,
                        G_TX_NOLOD, G_TX_NOLOD
                    );
                    break;
                case 3:
                    gDPLoadTextureBlock(temp_s0++, sprite->unk370[i],
                        sprite->unk330->unkA, G_IM_SIZ_32b,
                        sprite->unk330->unkC, sprite->unk330->unkE,
    		            0,
                        G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP,
                        G_TX_NOMASK, G_TX_NOMASK,
                        G_TX_NOLOD, G_TX_NOLOD
                    );
                    break;
            }
            D_global_asm_807F600C = sprite->unk370[i];
        }
        gSPVertex(temp_s0++, osVirtualToPhysical(sprite->unk28 + (i << 2)), 4, 0);
        gSP2Triangles(temp_s0++, 0, 1, 3, 0, 0, 2, 3, 0);
    }
    gSPPopMatrix(temp_s0++, G_MTX_MODELVIEW);
    gDPPipeSync(temp_s0++);
    gDPSetColorDither(temp_s0++, G_CD_MAGICSQ);
    if (raw_alignment != 0) {
        gSPMatrix(temp_s0++, &D_2000000, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_PROJECTION);
        gSPMatrix(temp_s0++, &D_2000200, G_MTX_NOPUSH | G_MTX_MUL | G_MTX_PROJECTION);
        if (raw_alignment == 1) {
            if (sprite->unk388 == -1) {
                gDPSetScissor(temp_s0++, G_SC_NON_INTERLACE,
                    0,
                    0,
                    D_global_asm_80744490,
                    D_global_asm_80744494
                );
            }
        }
    }
    gDPPipeSync(temp_s0++);
    gSPEndDisplayList(temp_s0++);
    if (raw_alignment > 1) {
        dl = popHUD(dl, raw_alignment);
    }
    D_global_asm_807F6009 = 5;
    return dl;
}

//@recomp: Draw HUD Numbers
RECOMP_PATCH Gfx *func_global_asm_806F9D8C(s32 arg0, Struct806FA504_arg1 *arg1, Gfx *dl) {
    Struct806F9D8C_arg14 *sp74;
    Struct80750948 *temp_v0_3;
    u8 sp6C[4];
    u8 sp6B;
    f32 temp_f0;
    s32 var_a2;
    s32 sp5C;
    u8 alignment;

    sp74 = arg1->unk14;
    temp_f0 = func_global_asm_80612794(arg1->unk10);
    sp6B = temp_f0 * 255.0;
    sp5C = 0;
    alignment = 1;
    if (D_global_asm_80754280->hud_item[arg0].screen_x < 80) {
        alignment = ALIGN_LEFT;
    } else if (D_global_asm_80754280->hud_item[arg0].screen_x > 240) {
        alignment = ALIGN_RIGHT;
    }
    temp_v0_3 = func_global_asm_806C7C94(0U);
    gEXPushScissor(dl++);
    gEXPushViewport(dl++);
    gEXSetScissor(dl++, G_SC_NON_INTERLACE, G_EX_ORIGIN_LEFT, G_EX_ORIGIN_RIGHT, 0, 0, 0, D_global_asm_80744494);
    if (alignment == ALIGN_RIGHT) {
        // Right align
        gEXSetRectAlign(dl++, G_EX_ORIGIN_RIGHT, G_EX_ORIGIN_RIGHT,
            -D_global_asm_80744490 * 4, 0,
            -D_global_asm_80744490 * 4, 0);
        gEXSetViewportAlign(dl++, G_EX_ORIGIN_RIGHT, -D_global_asm_80744490 * 4, 0);
    } else if (alignment == ALIGN_LEFT) {
        gEXSetRectAlign(dl++, G_EX_ORIGIN_LEFT, G_EX_ORIGIN_LEFT, 0, 0, 0, 0);
        gEXSetViewportAlign(dl++, G_EX_ORIGIN_LEFT, 0, 0);
    }
    // dl = alignHUDTopBottom(dl, alignment, temp_v0_3->unk4, temp_v0_3->unk8);
    gSPMatrix(dl++, &D_2000080, G_MTX_PROJECTION | G_MTX_LOAD | G_MTX_NOPUSH);
    guTranslate(&sp74->unk8[D_global_asm_807444FC], arg1->unk4 * (1.0f - temp_f0), arg1->unk8 * (1.0f - temp_f0), 0.0f);

    switch (D_global_asm_80754280->hud_item[arg0].unk_2c) {
        case 1:
            _sprintf(sp6C, "o");
            break;
        case 2:
            _sprintf(sp6C, "NA");
            break;
        default:
            if (arg0 == 5) {
                var_a2 = func_global_asm_80612E40(D_global_asm_80754280->hud_item[arg0].hud_count * 0.006666667f);
            } else {
                var_a2 = D_global_asm_80754280->hud_item[arg0].hud_count;
            }
            _sprintf(sp6C, "%d", var_a2);
            break;
    }
    gSPMatrix(dl++, &sp74->unk8[D_global_asm_807444FC], G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_PUSH);
    gDPPipeSync(dl++);
    gDPSetPrimColor(dl++, 0, 0, 0, 0, 0, sp6B);
    D_global_asm_807FD7E4 = sp6B;
    gSPDisplayList(dl++, &D_1000118);
    gDPSetCombineLERP(dl++, 0, 0, 0, TEXEL0, TEXEL0, 0, PRIMITIVE, 0, 0, 0, 0, TEXEL0, TEXEL0, 0, PRIMITIVE, 0);
    if ((D_global_asm_80744490 * 0.5) < D_global_asm_80754280->hud_item[arg0].screen_x) {
        sp5C = getCenterOfString(0x81, sp6C);
    }
    dl = printStyledText(dl, 0x81, (sp74->unk0 - sp5C) * 4, sp74->unk4 * 4, sp6C, 0U);
    dl = popHUD(dl, alignment);
    gSPPopMatrix(dl++, G_MTX_MODELVIEW);
    gDPPipeSync(dl++);
    return dl;
}

//@recomp: Menu Sprite Init
RECOMP_PATCH void func_menu_80030894(MenuAdditionalActorData *arg0, void *sprite, s32 x, s32 y, f32 scale, u8 arg5, s32 arg6) {
    Struct80717D84 *sp3C;
    Struct80717D84_80030894 *temp_v0;
    f32 dX;
    f32 dY;
    f32 d;
    f32 temp_f2;
    u8 alignment;

    func_global_asm_80714998(arg5);
    alignment = 1;
    switch (arg6) {
        case 0:
            // A/B Buttons (Barrel Screen, File Select, File Info, Delete Select,
            //              Delete Confirm, Multi Type, multi join, multi scores,
            //              sound, options, mystery, minigame scores)
            // LR Joystick (Barrel Screen, Delete Select, Delete Confirm)
            // C Up Button (Multi Join)
            // Start Button (Multi Join)
        case 0xB: // A/B Buttons (Multi Join)
        case 0xD: // A/B/CDown Buttons (Multi Join)
        case 0x10:  // A Button (Mystery Menu)
        case 0x11: // Z Button (Mystery Menu)
            if (x > 240) {
                alignment = ALIGN_RIGHT | ALIGN_HIDING_IN_OVERSCAN;
            } else if (x < 80) {
                alignment = ALIGN_LEFT | ALIGN_HIDING_IN_OVERSCAN;
            }
            break;
        case 2: // GB, Orange (file select screen)
        case 3: // GB (Delete Select)
        case 4: // LR Joystick (file select screen, multi type, multi join, sound, options), Orange (Delete Select)
        case 6: // Menu Icons (inside the barrel)
        case 7: // Fairy menu icon (inside the barrel)
        case 8: // Orange (Delete confirm)
        case 9: // GB (File info screen)
        case 12: // Z Button (sound, options)
        case 14: // Fairy (Mystery Menu)
        case 15: // Kong Heads (file info screen), kong placeholders (multi join)
        case 0x12: // Barrel Bottom
        default:
            break;
    }
    setSpriteAlignment(alignment);
    func_global_asm_807149FC(-1);
    func_global_asm_80714950((s32)arg0);
    func_global_asm_8071498C((void*)func_menu_80030C14);
    func_global_asm_80714A28(1);
    sp3C = drawSpriteAtPosition(sprite, scale, x, y, -10.0f);
    temp_v0 = _malloc(sizeof(Struct80717D84_80030894));
    sp3C->unk384 = (void*)temp_v0;
    temp_v0->unk0 = arg6;
    temp_v0->unk4 = arg0->unk15;
    temp_v0->unk8 = sp3C->unk340;
    temp_v0->unkC = sp3C->unk344;
    switch (arg6) {
        case 14:
            sp3C->unk360 = -sp3C->unk360;
            // fallthrough
        case 0:
        case 1:
        case 11:
        case 12:
        case 13:
        case 16:
        case 17:
        case 19:
            dX = 640.0f - temp_v0->unk8;
            dY = 480.0f - temp_v0->unkC;
            d = _sqrtf(SQ(dX) + SQ(dY));
            temp_f2 = 1040.0f - d;
            temp_v0->unk10 = -(dX / d) * temp_f2;
            temp_v0->unk14 = -(dY / d) * temp_f2;
            temp_v0->unk18 = sp3C->unk360;
            break;
        case 2:
        case 3:
        case 10:
            temp_v0->unk8 = x;
            temp_v0->unk10 = scale;
            temp_v0->unk14 = 0.0f;
            break;
        case 15:
            temp_v0->unk8 = x;
            break;
        default:
            temp_v0->unk10 = scale;
            temp_v0->unk14 = 0.0f;
            break;
    }
    if (D_menu_80033F38 == 0) {
        sp3C->unk36A = 0x80;
        sp3C->unk36B = 0x80;
        sp3C->unk36C = 0x80;
    }
    D_menu_80033F38 = 1;
}

// @recomp: Draw HUD Item
RECOMP_PATCH void func_global_asm_806F9744(Struct806F9744_arg0 *arg0, s32 arg1, f32 x, f32 y, s32 arg4) {
    s32 temp[2]; // TODO: Hmm
    s32 sp2C;
    Struct806F9744_arg0_unk14 *temp_s0;
    u8 alignment;

    temp_s0 = arg0->unk14;
    sp2C = 2;
    alignment = 1;
    if (x < 80) {
        alignment = ALIGN_LEFT;
    } else if (x > 240) {
        alignment = ALIGN_RIGHT;
    }
    setSpriteAlignment(alignment);
    func_global_asm_807149FC(-1);
    if (arg1 == 0xE) {
        sp2C = 1;
    }
    func_global_asm_80714998(sp2C);
    func_global_asm_80714944(arg4);
    func_global_asm_80714950((s32)arg0);
    func_global_asm_8071498C(func_global_asm_8071BE04);
    func_global_asm_80714A28(0x21);
    if (arg1 == 3) {
        changeActorColor(0xFF, 0, 0, 0xFF);
    }
    temp_s0->unk8 = drawSpriteAtPosition(func_global_asm_806FACE8(arg1), 1.0f, x, y, -10.0f);
    temp_s0->unk2 = 0;
    if (arg1 == 7) {
        temp_s0->unk4 = 1;
    } else {
        temp_s0->unk4 = 0;
    }
    // TODO: unk8 is otherSpriteControl*?
    func_global_asm_806F94AC(temp_s0->unk8, arg1);
}

Gfx* displayImage_simple(Gfx* dl, s32 x, s32 y, s32 width, s32 height, s16 texture_index, s32 fmt, s32 size, f32 xScale, f32 yScale, u8 xflip, u8 yflip) {
    void *texture = func_global_asm_8068C12C(texture_index);
    gDPPipeSync(dl++);
    switch (size) {
        case 0:
            gDPLoadTextureTile(dl++, texture, fmt, G_IM_SIZ_4b,
                width, height,
                0, 0, width - 1, height - 1,
                0,
                G_TX_CLAMP, G_TX_CLAMP,
                G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
            break;
        case 1:
            gDPLoadTextureTile(dl++, texture, fmt, G_IM_SIZ_8b,
                width, height,
                0, 0, width - 1, height - 1,
                0,
                G_TX_CLAMP, G_TX_CLAMP,
                G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
            break;
        case 2:
            gDPLoadTextureTile(dl++, texture, fmt, G_IM_SIZ_16b,
                width, height,
                0, 0, width - 1, height - 1,
                0,
                G_TX_CLAMP, G_TX_CLAMP,
                G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
            break;
        case 3:
            gDPLoadTextureTile(dl++, texture, fmt, G_IM_SIZ_32b,
                width, height,
                0, 0, width - 1, height - 1,
                0,
                G_TX_CLAMP, G_TX_CLAMP,
                G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
            break;
    }

    s32 dsdx = (s32)((1 << 10) / xScale);
    s32 dtdy = (s32)((1 << 10) / yScale);
    s32 s_start = 0;
    s32 t_start = 0;
    s32 x0, x1, y0, y1;

    if (xflip) {
        s_start = (width - 1) << 5;
        dsdx = -dsdx;
        x1 = x;
        x0 = x1 - (s32)(width * xScale);
    } else {
        x0 = x;
        x1 = x + (s32)(width * xScale);
    }

    if (yflip) {
        t_start = (height - 1) << 5;
        dtdy = -dtdy;
        y1 = y;
        y0 = y1 - (s32)(height * yScale);
    } else {
        y0 = y;
        y1 = y + (s32)(height * yScale);
    }

    gSPTextureRectangle(dl++,
        x0 << 2, y0 << 2,
        x1 << 2, y1 << 2,
        G_TX_RENDERTILE,
        s_start, t_start, dsdx, dtdy);

    gDPPipeSync(dl++);
    return dl;
}

//@recomp: Demo tv corner borders
RECOMP_PATCH Gfx *func_global_asm_806FF144(Gfx *dl) {
    s32 width, height, pillar;
    recomp_get_ui_bounds(&width, &height);
    // pillar = 10 + ((width - 426) / 1.9f);
    pillar = (width - 320) >> 1;

    gDPSetPrimColor(dl++, 0, 0, 0x00, 0x00, 0x00, 0xFF);
    gDPSetCombineMode(dl++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
    gDPSetRenderMode(dl++, G_RM_CLD_SURF, G_RM_CLD_SURF2);
    gSPMatrix(dl++, &D_2000080, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_PROJECTION);
    gDPSetScissor(dl++, G_SC_NON_INTERLACE, 0, 0, D_global_asm_80744490, D_global_asm_80744494);
    dl = displayImage(dl, 0x3A, 3, 1, 0x40, 0x40, 0x3E - pillar, 0x3E, 2.0f, 2.0f, 0, 0.0f);
    dl = displayImage(dl, 0x3A, 3, 1, 0x40, 0x40, D_global_asm_80744490 + pillar - 0x3E, 0x3E, 2.0f, 2.0f, 0x5A, 0.0f);
    dl = displayImage(dl, 0x3A, 3, 1, 0x40, 0x40, D_global_asm_80744490 + pillar- 0x3E, D_global_asm_80744494 - 0x3E, 2.0f, 2.0f, 0xB4, 0.0f);
    dl = displayImage(dl, 0x3A, 3, 1, 0x40, 0x40, 0x3E - pillar, D_global_asm_80744494 - 0x3E, 2.0f, 2.0f, 0x10E, 0.0f);
    return dl;
}

//@recomp: DKTV "DK TV" text
RECOMP_PATCH Gfx *func_global_asm_8071338C(Gfx *dl) {
    u8 *string;
    f32 temp;
    s32 width, height, pillar;
    recomp_get_ui_bounds(&width, &height);
    pillar = 10 + ((width - 320) >> 1);

    string = getTextString(0xC, 0, 1);
    gDPSetCombineMode(dl++, G_CC_DECALRGBA, G_CC_DECALRGBA);
    gSPMatrix(dl++, &D_global_asm_807FDAC0, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    temp = 20.0f;
    temp *= 2.0f;
    temp *= 4.0f;
    dl = printStyledText(dl, 1, (35 - pillar) * 8, temp, string, 4);
    return dl;
}

extern void func_global_asm_8061134C(void*);
extern Gfx *func_global_asm_807132DC(Gfx *);
extern Gfx *func_global_asm_807135B4(Gfx *);
extern Gfx *func_global_asm_80713438(Gfx *, s32);
extern Gfx *func_global_asm_80713764(Gfx *, s32, f32);
extern void func_global_asm_8062C99C(void *, s32, s32, s32, s32);
extern u32 D_global_asm_80755320;

// @recomp: DKTV Handler
RECOMP_PATCH Gfx *func_global_asm_807138CC(Gfx *dl) {
    void *temp_v0;
    f32 temp_f0;
    u8 temp_t2;

    temp_v0 = _malloc(0x10);
    func_global_asm_8061134C(temp_v0);
    func_global_asm_8062C99C(temp_v0, 0, 0, 320, 240);
    dl = func_global_asm_807132DC(dl);
    dl = func_global_asm_807135B4(dl);
    dl = func_global_asm_8071338C(dl);
    D_global_asm_80755320++;
    if (D_global_asm_80755320 < 0xD3U) {
        temp_f0 = ((f32) (0xD2 - D_global_asm_80755320)) * 0.033333335f;
        temp_f0 = CLAMP(temp_f0, 0.0f, 1.0f);
        temp_t2 = 255.0f * temp_f0;
        dl = func_global_asm_80713438(dl, temp_t2);
        dl = func_global_asm_80713764(dl, temp_t2, 0);
    }
    gSPViewport(dl++, temp_v0);
    dl = func_global_asm_806FF144(dl);
    return dl;
}

//@recomp: Locked camera icon
RECOMP_PATCH Gfx *func_global_asm_8068D8C8(Gfx *dl, s32 arg1) {
    gSPDisplayList(dl++, &D_1000118);
    gDPSetPrimColor(dl++, 0, 0, 0xFF, 0xFF, 0xFF, 0xA0);
    gDPSetCombineMode(dl++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
    gDPSetRenderMode(dl++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
    gSPMatrix(dl++, &D_2000080, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_PROJECTION);
    dl = alignHUD(dl, ALIGN_RIGHT);
    dl = displayImage_simple(dl,
        D_global_asm_80744490 - 50, D_global_asm_80744494 - 40,
        32, 32,
        ((((u32)object_timer >> 1) & 0xF) + 0x8F),
        G_IM_FMT_RGBA, 2,
        1.0f, 1.0f,
        0, 0);
    dl = popHUD(dl, ALIGN_RIGHT);
    return dl;
}

// @recomp: Draw GB Acquisition HUD
RECOMP_PATCH void func_global_asm_806F9B64(s32 arg0) {
    GlobalASMStruct71 **counter;
    GlobalASMStruct71 *previousCounter;
    s32 i;

    // Below is equivalent to &D_global_asm_80754280->hud_item[arg0].counter_pointer
    // but need to change the syntax to fix regalloc.
    counter = (GlobalASMStruct71**)&D_global_asm_80754280->hud_item[arg0].counter_pointer;
    func_global_asm_806F966C(counter);
    func_global_asm_806F96CC(*counter, 0);
    (*counter)->unk10 = 0;
    previousCounter = (*counter)->unk14;
    previousCounter->unk0 = D_global_asm_80754280->hud_item[arg0].screen_x + 20;
    previousCounter->unk4 = D_global_asm_80754280->hud_item[arg0].screen_y - 20;
    for (i = 0; i < 5; i++) {
        setSpriteAlignment(ALIGN_LEFT);
        func_global_asm_807149FC(-1);
        func_global_asm_8071498C(func_global_asm_806F9AF0);
        func_global_asm_80714950(i);
        D_global_asm_807FD7A0[i] = -100.0f;
        drawSpriteAtPosition(D_global_asm_80750518[i], 1.0f, -200.0f, 0.0f, -10.0f);
    }
}

//@recomp: Display remaining menus in various minigames
RECOMP_PATCH void func_bonus_8002733C(Struct8002733C *arg0) {
    s16 i;
    s16 x;

    x = 280;
    for (i = 0; i < 5; i++) {
        if (arg0->unk4[i] != NULL) {
            func_global_asm_80715908(arg0->unk4[i]);
        }
        arg0->unk18[i] = 0;
        setSpriteAlignment(ALIGN_RIGHT);
        func_global_asm_807149FC(-1);
        func_global_asm_80714998(2);
        func_global_asm_80714944(i * 3);
        func_global_asm_8071498C(func_global_asm_8071A038);
        func_global_asm_80714950((s32)&arg0->unk18[i]);
        arg0->unk4[i] = drawSpriteAtPosition(&D_global_asm_8071FFD4, 1.0f, x, 210.0f, -10.0f);
        x -= 30;
    }
    playSound(0x3E4, 0x7FFF, 63.0f, 1.0f, 5, 0);
}

// @recomp: some scissor/viewport alignment code. Only trigger this when we want
RECOMP_PATCH Gfx *func_global_asm_8070068C(Gfx *dl) {
    gSPMatrix(dl++, &D_2000100, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_PROJECTION);
    gSPViewport(dl++, osVirtualToPhysical(&character_change_array->unk250[D_global_asm_807444FC]));
    if (D_global_asm_807FDB1D < 2) {
        gDPSetScissor(
            dl++,
            G_SC_NON_INTERLACE,
            character_change_array[0].unk270[0],
            character_change_array[0].unk270[1],
            character_change_array[0].unk270[2],
            character_change_array[0].unk270[3]
        );
    }
    return dl;
}

//@recomp: Display minigame timer
RECOMP_PATCH Gfx *func_global_asm_806A2B90(Gfx *dl, Actor *arg1) {
    AAD_global_asm_806A2A10 *sp5C;
    s32 sp58;
    s32 sp54;
    s32 sp50;
    s32 sp4C;
    s32 var_v0; // 48
    Struct80754AD0 *temp; // 44
    f32 sp40;
    s32 sp3C;
    f32 sp38;
    f32 sp34;
    s32 sp30;
    u8 alignment;

    sp5C = arg1->AAD_as_array[0];
    if (func_global_asm_805FCA64()) {
        gSPDisplayList(dl++, &D_1000118);
        gDPSetCombineMode(dl++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
        gDPSetPrimColor(dl++, 0, 0, 0xFF, 0xFF, 0xFF, arg1->shadow_opacity);
        gSPMatrix(dl++, &D_2000180, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gDPSetRenderMode(dl++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
        sp30 = arg1->unk15F;
        if (sp30 == 0xB) {
            dl = func_global_asm_8070068C(dl);
            temp = func_global_asm_806FD9B4(sp5C->unk10);
            dl = printStyledText(dl, 0x86, 
                arg1->x_position * 4.0f,
                ((character_change_array->unk270[3] * 4) - 0x3C),
                temp->unk4, 1U);
        } else {
            var_v0 = 8;
            sp40 = 6.2831854820251465 - (2.0 * arg1->unk160);
            if (arg1->unk168 == 3) {
                var_v0 = -7;
            }
            alignment = 1;
            if (arg1->x_position > 200) {
                alignment = ALIGN_RIGHT;
            } else if (arg1->x_position < 120) {
                alignment = ALIGN_LEFT;
            }
            if (alignment != 1) {
                dl = alignHUD(dl, ALIGN_RIGHT);
            }
            setSpriteAlignment(alignment);
            dl = func_global_asm_806FE078(dl, 
                sp5C->unk10, sp30, 
                arg1->x_position + var_v0, 
                arg1->y_position + 5.0f, 0.0f, 1.0f);
            setSpriteAlignment(0);
            if (alignment != 1) {
                dl = popHUD(dl, alignment);
            }
            if (arg1->control_state == 2) {
                dl = func_global_asm_8070068C(dl);
                setSpriteAlignment(alignment);
                sp34 = func_global_asm_80612D1C(sp40);
                sp38 = func_global_asm_80612D10(sp40);
                drawSpriteAtPosition(&D_global_asm_8071FC58, 0.5f,
                    (sp34 * 40.0) + (40.0f + arg1->x_position), 
                    (sp38 * 25.0) + arg1->y_position,
                    0.0f);
            }
        }
    }
    return dl;
}

//@recomp: Displays any counter text, with setting the scissor
RECOMP_PATCH Gfx* func_global_asm_8068DC54(Gfx* dl, s16 arg1, s16 arg2, s16* arg3, s16 arg4, u8* arg5) {
    f32 sp74;
    s16 temp_s0;
    u8 sp70[2];
    s16 temp_v1;
    u8 sp6C[2];
    s16 var_s0;
    u8 sp68[2];
    s16 temp_f6;
    u8 sp64[2];
    s8 sp60[4];
    s16 sp5E;
    s16 var_v1;
    s32 var_v1_2;
    s16 temp_f18;
    u8 temp;

    sp5E = arg4 < *arg3 ? -1 : 1;
    if (arg4 != *arg3) {
        if (*arg5 != 0) {
            temp = *arg5 - 1;
            *arg5 = temp;
            if (!(*arg5 & 0xFF)) {
                *arg3 += sp5E;
                if (arg4 != *arg3) {
                    *arg5 = 0xC;
                }
            }
        } else {
            *arg5 = 0xC;
        }
    }
    dl = func_global_asm_805FD030(dl);
    var_s0 = 0;
    //@recomp: Set scissor x bounds to be the edges of the screen
    gDPSetScissor(dl++, G_SC_NON_INTERLACE,
        character_change_array->unk270[0],
        arg2,
        character_change_array->unk270[2],
        arg2 + 0x1B);
    if (D_global_asm_807FDB1D > 1) {
        dl = alignHUDTopBottom(dl, D_global_asm_807FDB1D, arg2, arg2 + 0x1B);
    }
    for (var_v1 = *arg3; var_v1 >= 0x64; var_v1 -= 0x64) {
        var_s0++;
    }
    if (var_v1 == 0x63) {
        var_s0++;
    }
    sp70[0] = (var_v1 / 10) + 0x30;
    sp70[1] = 0;
    sp6C[0] = (var_v1 % 10) + 0x30;
    sp6C[1] = 0;
    sp68[0] = ((var_v1 + sp5E) / 10) + 0x30;
    sp68[1] = 0;
    sp64[0] = ((var_v1 + sp5E) % 10) + 0x30;
    sp64[1] = 0;
    sp74 = func_global_asm_80612D1C((((*arg5 / 12.0) * MATH_HALF_PI_D) + MATH_HALF_PI_D)) * 96.0;
    if (arg4 == *arg3) {
        dl = printStyledText(dl, 3, arg1 * 4, arg2 * 4, (u8*)&sp70, 1U);
        dl = printStyledText(dl, 3, (s16) ((arg1 * 4) + 0x50), (s16) arg2 * 4, (u8*)&sp6C, 1U);
    } else if (sp70[0] != sp68[0]) {
        temp_f6 = ((arg2 * 4) - 0x60) + sp74;
        temp_f18 = ((arg2 * 4) + 4) + sp74;
        dl = printStyledText(dl, 3, arg1 * 4, temp_f6, (u8*)&sp68, 1U);
        dl = printStyledText(dl, 3, arg1 * 4, temp_f18, (u8*)&sp70, 1U);
        dl = printStyledText(dl, 3, (arg1 * 4) + 0x50, temp_f6, (u8*)&sp64, 1U);
        dl = printStyledText(dl, 3, (arg1 * 4) + 0x50, temp_f18, (u8*)&sp6C, 1U);
    } else {
        dl = printStyledText(dl, 3, arg1 * 4, arg2 * 4, (u8*)&sp70, 1U);
        dl = printStyledText(dl, 3, (arg1 * 4) + 0x50, ((arg2 * 4) - 0x60) + sp74, (u8*)&sp64, 1U);
        dl = printStyledText(dl, 3, (arg1 * 4) + 0x50, ((arg2 * 4) + 4) + sp74, (u8*)&sp6C, 1U);
    }
    if (D_global_asm_807FDB1D > 1) {
        dl = popHUD(dl, D_global_asm_807FDB1D);
    }
    var_v1_2 = arg1 - 0x14;
    if ((*arg3 + sp5E) >= 0x64) {
        if (var_v1_2 < 0) {
            var_v1_2 = 0;
        }
        //@recomp: Set scissor x bounds to be the edges of the screen
        gDPSetScissor(dl++, G_SC_NON_INTERLACE,
            character_change_array->unk270[0],
            arg2,
            character_change_array->unk270[2],
            arg2 + 0x1B
        );
        if (D_global_asm_807FDB1D > 1) {
            dl = alignHUDTopBottom(dl, D_global_asm_807FDB1D, arg2, arg2 + 0x1B);
        }
        _sprintf(sp60, "%d", var_s0);
        if ((*arg3 >= 0x64) && (*arg3 != 0xC7)) {
            dl = printStyledText(dl, 3, ((arg1 * 4) - 0x50), (arg2 * 4), (u8*)&sp60, 1U);
        } else if ((*arg3 == 0x63) || (*arg3 == 0xC7)) {
            temp_s0 = (arg1 * 4) - 0x50;
            dl = printStyledText(dl, 3, temp_s0, ((arg2 * 4) - 0x60) + sp74, (u8*)&sp60, 1U);
            if (*arg3 == 0xC7) {
                dl = printStyledText(dl, 3, temp_s0, ((arg2 * 4) + 4) + sp74, (u8*)"1", 1U);
            }
        }
        if (D_global_asm_807FDB1D > 1) {
            dl = popHUD(dl, D_global_asm_807FDB1D);
        }
    }
    gDPPipeSync(dl++);
    gDPSetScissor(dl++, G_SC_NON_INTERLACE,
        character_change_array->unk270[0],
        character_change_array->unk270[1],
        character_change_array->unk270[2],
        character_change_array->unk270[3]
    );
    return dl;
}

//@recomp: Render the "GET" HUD (various minigames)
RECOMP_PATCH Gfx *func_bonus_80024000(Gfx *dl, Actor *arg1) {
    A178_80024000 *a178;
    a178 = arg1->AAD_as_array[1];
    if (func_global_asm_805FCA64()) {
        gSPDisplayList(dl++, &D_1000118);
        dl = func_global_asm_8070068C(dl);
        gDPSetCombineMode(dl++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
        gDPSetPrimColor(dl++, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF);
        gSPMatrix(dl++, &D_2000180, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        // Header
        dl = alignHUD(dl, ALIGN_LEFT);
        setSpriteAlignment(ALIGN_LEFT);
        dl = func_global_asm_806FE078(dl, a178->unk9, 8, 30.0f, 36.0f, 0.0f, 1.5f);
        setSpriteAlignment(0);
        dl = popHUD(dl, ALIGN_LEFT);
        // Counter
        setSpriteAlignment(ALIGN_LEFT);
        dl = func_global_asm_8068DC54(dl, 0x26, 0x32, &a178->unk2, a178->unk4, &a178->unk8);
        setSpriteAlignment(0);
    }
    return dl;
}

//@recomp: Render the "GET" HUD (Batty BB)
RECOMP_PATCH Gfx *func_bonus_800252A0(Gfx *dl, Actor *arg1) {
    AAD_bonus_800252A0 *aaD;
    aaD = arg1->AAD_as_array[0];
    gDPPipeSync(dl++);
    gDPSetPrimColor(dl++, 0, 0, 0xC8, 0xC8, 0xC8, 0xFF);
    gDPSetCombineMode(dl++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
    gDPSetRenderMode(dl++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
    gSPDisplayList(dl++, &D_1000118);
    gSPMatrix(dl++, &D_2000080, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_PROJECTION);
    gSPMatrix(dl++, &D_2000180, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    // Header
    dl = alignHUD(dl, ALIGN_LEFT);
    setSpriteAlignment(ALIGN_LEFT);
    dl = func_global_asm_806FE078(dl, aaD->unk19, 8, 30.0f, 36.0f, 0.0f, 1.5f);
    setSpriteAlignment(0);
    dl = popHUD(dl, ALIGN_LEFT);
    // Counter
    setSpriteAlignment(ALIGN_LEFT);
    dl = func_global_asm_8068DC54(dl, 0x26, 0x32, &aaD->unk14, aaD->unk16, &aaD->unk18);
    setSpriteAlignment(0);
    return dl;
}

//@recomp: Kremling Kosh Counter
RECOMP_PATCH Gfx *func_bonus_80026940(Gfx *dl, Actor *KoshController) {
    s32 pad7C;
    KremlingKoshInit *init;
    s32 pad74;
    s32 pad70;
    s32 pad68;
    s32 pad68_0;
    u8 *text_str;
    s32 pad[0x6];
    s32 x;
    KremlingKoshAAD *aad;
    s8 *sp64;
    s32 style_height_0;
    s32 style_height_1;
    s32 x_0;
    s32 pad30;
    s32 pad2C;
    

    aad = KoshController->AAD_as_array[0];
    init = KoshController->AAD_as_array[1];
    if ((KoshController->control_state == 0) && (aad->unk26 != 0)) {
        dl = func_bonus_80026690(dl, KoshController);
    }
    gSPDisplayList(dl++, &D_1000118);
    gDPPipeSync(dl++);
    gDPSetPrimColor(dl++, 0, 0, 0xC8, 0xC8, 0xC8, 0xFF);
    gDPSetCombineMode(dl++, G_CC_MODULATEIDECALA_PRIM, G_CC_MODULATEIDECALA_PRIM);
    gDPSetRenderMode(dl++, G_RM_TEX_EDGE, G_RM_TEX_EDGE2);
    gSPMatrix(dl++, &D_2000180, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    if (KoshController->control_state != 1) {
        // Header
        dl = alignHUD(dl, ALIGN_LEFT);
        setSpriteAlignment(ALIGN_LEFT);
        dl = func_global_asm_806FE078(dl, init->unk25, 8, 30.0f, 36.0f, 0.0f, 1.5f);
        setSpriteAlignment(0);
        dl = popHUD(dl, ALIGN_LEFT);
        // Counter
        setSpriteAlignment(ALIGN_LEFT);
        dl = func_global_asm_8068DC54(
            dl,
            0x26,
            0x32,
            &init->hit_requirement,
            init->hit_requirement_hud,
            &init->unk24);
        setSpriteAlignment(0);
    }
    if (aad->unk25 != 0) {
        x = D_global_asm_80744490 >> 1;
        text_str = getTextString(0x1AU, 6, 1);
        gDPPipeSync(dl++);
        gDPSetRenderMode(dl++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
        gDPSetPrimColor(dl++, 0, 0, 0x00, 0x00, 0x00, aad->unk25);
        gDPSetCombineLERP(dl++, 0, 0, 0, TEXEL0, TEXEL0, 0, PRIMITIVE, 0, 0, 0, 0, TEXEL0, TEXEL0, 0, PRIMITIVE, 0);
        x -= (getCenterOfString(1, text_str) >> 1);
        style_height_0 = func_global_asm_806FD894(1);
        dl = printStyledText(
            dl, 1,
            x * 4,
            ((D_global_asm_80744494 - style_height_0) * 2),
            text_str,
            1U);
        aad->unk25 -= MIN(aad->unk25, 8);
    }
    if (aad->unk24 != 0) {
        x_0 = D_global_asm_80744490 >> 1;
        text_str = getTextString(0x1AU, 8, 1);
        gDPSetRenderMode(dl++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
        gDPSetPrimColor(dl++, 0, 0, 0x00, 0x00, 0x00, aad->unk24);
        gDPSetCombineLERP(dl++, 0, 0, 0, TEXEL0, TEXEL0, 0, PRIMITIVE, 0, 0, 0, 0, TEXEL0, TEXEL0, 0, PRIMITIVE, 0);
        x_0 -= getCenterOfString(1, text_str) >> 1;
        dl = printStyledText(
            dl,
            1,
            x_0 * 4,
            ((func_global_asm_806FD894(1) + D_global_asm_80744494) * 2),
            text_str,
            1U);
        aad->unk24 -= MIN(aad->unk24, 8);
    }
    return dl;
}

//@recomp: Krazy KK/PPPanic/BBBash counter code
RECOMP_PATCH Gfx* func_bonus_80029B9C(Gfx* dl, Actor* arg1) {
    s32 pad;
    KrazyKKAAD178* aad178_copy;
    KrazyKKAAD178* aad178;
    KrazyKKAAD* aad;

    aad178_copy = arg1->AAD_as_array[1];
    aad178 = arg1->AAD_as_array[1];
    aad = arg1->AAD_as_array[0];
    if ((arg1->unk58 != ACTOR_FLYSWATTER) && (arg1->control_state == 0) && (aad->unk26)) {
        dl = func_bonus_80026690(dl, arg1);
    }
    gSPDisplayList(dl++, &D_1000118);
    gDPPipeSync(dl++);
    gDPSetPrimColor(dl++, 0, 0, 0xC8, 0xC8, 0xC8, 0xFF);
    gDPSetCombineMode(dl++, G_CC_MODULATEIDECALA_PRIM, G_CC_MODULATEIDECALA_PRIM);
    gDPSetRenderMode(dl++, G_RM_TEX_EDGE, G_RM_TEX_EDGE2);
    gSPMatrix(dl++, &D_2000180, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    switch (arg1->unk58) {
        case ACTOR_BARRELGUN_KRAZYKONGKLAMOUR:
            if (arg1->control_state != 3) {
                // Header
                dl = alignHUD(dl, ALIGN_LEFT);
                setSpriteAlignment(ALIGN_LEFT);
                dl = func_global_asm_806FE078(dl, aad178_copy->unk11, 8, 30.0f, 36.0f, 0.0f, 1.5f);
                setSpriteAlignment(0);
                dl = popHUD(dl, ALIGN_LEFT);
                // Counter
                setSpriteAlignment(ALIGN_LEFT);
                dl = func_global_asm_8068DC54(dl, 0x26, 0x32, &aad178_copy->unk14, aad178_copy->unk16, &aad178_copy->unk12);
                setSpriteAlignment(0);
            }
            break;
        case ACTOR_BARRELGUN_PERILPATHPANIC:
            if (arg1->control_state != 3) {
                // Header
                dl = alignHUD(dl, ALIGN_LEFT);
                setSpriteAlignment(ALIGN_LEFT);
                dl = func_global_asm_806FE078(dl, aad178->unk3, 8, 30.0f, 36.0f, 0.0f, 1.5f);
                setSpriteAlignment(0);
                dl = popHUD(dl, ALIGN_LEFT);
                // Counter
                setSpriteAlignment(ALIGN_LEFT);
                dl = func_global_asm_8068DC54(dl, 0x26, 0x32, &aad178->unk8, aad178->unkA, &aad178->unk6);
                setSpriteAlignment(0);
                break;
            }
            break;
        case ACTOR_FLYSWATTER:
            switch (arg1->control_state) {
                case 7:
                    break;
                case 0:
                case 1:
                case 2:
                    // Header
                    dl = alignHUD(dl, ALIGN_LEFT);
                    setSpriteAlignment(ALIGN_LEFT);
                    dl = func_global_asm_806FE078(dl, aad->unk25, 8, 30.0f, 36.0f, 0.0f, 1.5f);
                    setSpriteAlignment(0);
                    dl = popHUD(dl, ALIGN_LEFT);
                    // Counter
                    setSpriteAlignment(ALIGN_LEFT);
                    dl = func_global_asm_8068DC54(dl, 0x26, 0x32, &aad->unk28, aad->unk2A, &aad->unk26);
                    setSpriteAlignment(0);
                    break;
            }
        default:
            break;
        }
    return dl;
}

//@recomp: Searchlight Seek counter
RECOMP_PATCH Gfx *func_bonus_8002CC08(Gfx *dl, Actor *arg1) {
    AAD_8002CC08 *aaD;
    f32 sp80;
    f32 sp7C;
    f32 temp_f20;
    s16 temp_f18;
    s16 temp_f16;

    aaD = arg1->AAD_as_array[0];
    func_global_asm_80626F8C(arg1->x_position, arg1->y_position, arg1->z_position, &sp80, &sp7C, 0, 1.0f, cc_player_index);
    temp_f18 = (f32)(sp80 * 4.0);
    temp_f16 = (f32)(sp7C * 4.0);
    gDPPipeSync(dl++);
    gDPSetCombineMode(dl++, G_CC_DECALRGBA, G_CC_DECALRGBA);
    gDPSetRenderMode(dl++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
    gSPDisplayList(dl++, &D_1000118);
    gSPMatrix(dl++, &D_20000C0, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_PROJECTION);
    if ((arg1->control_state == 0) || (arg1->control_state == 1)) {
        temp_f20 = D_bonus_8002DEB4;
        dl = displayImage(dl, (((object_timer >> 1) % 12) + 0x83), 0, 2, 0x20, 0x10, (temp_f18 - 0x34), (temp_f16 - 0x34), temp_f20, temp_f20, 0xE1, 0.0f);
        dl = displayImage(dl, (((object_timer >> 1) % 12) + 0x83), 0, 2, 0x20, 0x10, (temp_f18 + 0x34), (temp_f16 - 0x34), temp_f20, temp_f20, 0x13B, 0.0f);
        dl = displayImage(dl, (((object_timer >> 1) % 12) + 0x83), 0, 2, 0x20, 0x10, (temp_f18 + 0x34), (temp_f16 + 0x34), temp_f20, temp_f20, 0x2D, 0.0f);
        dl = displayImage(dl, (((object_timer >> 1) % 12) + 0x83), 0, 2, 0x20, 0x10, (temp_f18 - 0x34), (temp_f16 + 0x34), temp_f20, temp_f20, 0x87, 0.0f);
    }
    gSPMatrix(dl++, &D_2000180, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    switch (arg1->control_state) {
        case 7:
            break;
        case 0:
        case 1:
            // Header
            dl = alignHUD(dl, ALIGN_LEFT);
            setSpriteAlignment(ALIGN_LEFT);
            dl = func_global_asm_806FE078(dl, aaD->unk23, 8, 30.0f, 36.0f, 0.0f, 1.5f);
            setSpriteAlignment(0);
            dl = popHUD(dl, ALIGN_LEFT);
            // Counter
            setSpriteAlignment(ALIGN_LEFT);
            dl = func_global_asm_8068DC54(dl, 0x26, 0x32, &aaD->unk26, aaD->unk28, &aaD->unk24);
            setSpriteAlignment(0);
            break;
    }
    return dl;
}

//@recomp: Rambi/Enguarde Arena counter
RECOMP_PATCH Gfx *func_bonus_8002D010(Gfx *dl, Actor *arg1) {
    s16 pad;
    s16 i;
    s16 y;
    AAD_8002D010 *aaD;
    u8 sp70[17];

    aaD = arg1->AAD_as_array[0];

    gSPDisplayList(dl++, &D_1000118);
    gDPPipeSync(dl++);
    gDPSetPrimColor(dl++, 0, 0, 0xC8, 0xC8, 0xC8, 0xFF);
    gDPSetCombineMode(dl++, G_CC_MODULATEIDECALA_PRIM, G_CC_MODULATEIDECALA_PRIM);
    gDPSetRenderMode(dl++, G_RM_TEX_EDGE, G_RM_TEX_EDGE2);
    gSPMatrix(dl++, &D_2000180, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

    // Header
    dl = alignHUD(dl, ALIGN_LEFT);
    setSpriteAlignment(ALIGN_LEFT);
    dl = func_global_asm_806FE078(dl, aaD->unk3, 8, 30.0f, 36.0f, 0.0f, 1.0f);
    setSpriteAlignment(0);
    dl = popHUD(dl, ALIGN_LEFT);
    // Counter
    setSpriteAlignment(ALIGN_LEFT);
    dl = func_global_asm_8068DC54(dl, 0x26, 0x2D, &aaD->unk8, aaD->unkA, &aaD->unk6);
    setSpriteAlignment(0);

    if (aaD->unk6 > 0) {
        aaD->unk6 -= 2;
    }
    if (current_map != MAP_ENGUARDE_ARENA) {
        if ((current_map == MAP_RAMBI_ARENA) && (arg1->control_state == 2)) {
            y = 480 - (u16)(D_bonus_8002D92C * 48);
            for (i = -1; i < D_bonus_8002D92C; i++) {
                if (i >= 0) {
                    _sprintf(sp70, "HIT %d", D_bonus_8002DEF0[i]);
                } else if (D_bonus_8002D92C >= 2) {
                    _sprintf(sp70, "COMBO x2");
                } else {
                    sp70[0] = '\0';
                }
                dl = printStyledText(dl, 6, 640 - (getCenterOfString(6, sp70) * 2), y, sp70, 1);
                y += 48;
            }
        }
    } else {
        if (aaD->unkC != 0) {
            aaD->unkC--;
            if ((aaD->unkC & 0x1F) < 0x14) {
                dl = func_global_asm_806FE078(dl, aaD->unkD, 8, 100.0f, 100.0f, 0.0f, 1.0f);
            }
        }
    }
    return dl;
}

// @recomp: Display K Rool Round
RECOMP_PATCH Gfx *func_boss_800286B8(Gfx *dl, Actor *arg1) {
    u8 sp3C[13];

    gSPDisplayList(dl++, &D_1000118);
    gDPSetCombineMode(dl++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
    gSPMatrix(dl++, &D_2000180, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPMatrix(dl++, &D_20000C0, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_PROJECTION);
    gDPPipeSync(dl++);
    gDPSetPrimColor(dl++, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF);
    _sprintf(sp3C, "ROUND %d", D_global_asm_80750AD4);
    dl = alignHUD(dl, ALIGN_LEFT);
    dl = printText(dl, 50 * 4, (character_change_array->unk270[3] - 15) * 4, 0.6f, sp3C);
    dl = popHUD(dl, ALIGN_LEFT);
    return dl;
}

extern u8 cc_number_of_players;
extern Gfx *func_global_asm_806FEDB0(Gfx *dl, u8 arg1);
extern u16 func_global_asm_806F8AD4(u8 arg0, u8 playerIndex);
extern s32 func_global_asm_80690F30(u16, s32*, Actor *, u8, u8, u8, s32*, s32*, s32*);

// @recomp: Sniper Scope
RECOMP_PATCH Gfx *func_global_asm_806FF75C(Gfx* dl, Actor *arg1) {
    u8 temp_a2;
    s32 sp70;
    s32 var_v0;
    s32 sp68;
    s32 sp64;
    s32 sp60;
    s32 width, height, pillar;

    var_v0 = 2;

    temp_a2 = arg1->PaaD->unk1A4;
    if (cc_number_of_players >= 2) {
        var_v0 = 3;
    }
    dl = func_global_asm_806FEDB0(dl, temp_a2);
    gDPSetPrimColor(dl++, 0, 0, 0x00, 0x00, 0x00, 0xFF);
    gDPSetScissor(dl++, G_SC_NON_INTERLACE, 0, 0, D_global_asm_80744490, D_global_asm_80744494);
    recomp_get_ui_bounds(&width, &height);
    pillar = (width - 320) >> 1;
    // Inner
    dl = displayImage(dl, 0x3AU, 3, 1, 0x40, 0x40, 0x100, 0x40, 3.0f, 2.0f, 0, 0.0f);
    dl = displayImage(dl, 0x3AU, 3, 1, 0x40, 0x40, 0x40, 0x40, 2.0f, 3.0f, 0x5A, 0.0f);
    dl = displayImage(dl, 0x3AU, 3, 1, 0x40, 0x40, 0x100, 0xB0, 2.0f, 3.0f, 0x10E, 0.0f);
    dl = displayImage(dl, 0x3AU, 3, 1, 0x40, 0x40, 0x40, 0xB0, 3.0f, 2.0f, 0xB4, 0.0f);
    // Outer
    dl = displayImage(dl, 0x3AU, 3, 1, 0x40, 0x40, 0x60 - pillar, 0x60, 3.0f, 3.0f, 0, 0.0f);
    dl = displayImage(dl, 0x3AU, 3, 1, 0x40, 0x40, 0xE0 + pillar, 0x60, 3.0f, 3.0f, 0x5A, 0.0f);
    dl = displayImage(dl, 0x3AU, 3, 1, 0x40, 0x40, 0xE0 + pillar, 0x90, 3.0f, 3.0f, 0xB4, 0.0f);
    dl = displayImage(dl, 0x3AU, 3, 1, 0x40, 0x40, 0x60 - pillar, 0x90, 3.0f, 3.0f, 0x10E, 0.0f);
    if ((func_global_asm_806F8AD4(3U, temp_a2)) && (func_global_asm_80690F30(var_v0, &sp70, arg1, 1, 0, 0, &sp68, &sp64, &sp60))) {
        gDPSetPrimColor(dl++, 0, 0, 0x00, 0xC8, 0x00, 0xFF);
    } else {
        gDPSetPrimColor(dl++, 0, 0, 0xC8, 0x00, 0x00, 0xFF);
    }
    gDPSetRenderMode(dl++, G_RM_CLD_SURF, G_RM_CLD_SURF2);
    return displayImage(dl, 0x38U, 3, 1, 0x40, 0x40, 0xA0, 0x78, 0.5f, 0.5f, 0x2D, 0.0f);
}

typedef struct Struct8002CBEC_AAD_20_0_4 {
    u8 pad0[0x27];
    u8 unk27;
    u8 pad28[0x2C - 0x28];
    Actor *unk2C;
} Struct8002CBEC_AAD_20_0_4;
typedef struct Struct8002CBEC_AAD_20_0 {
    Actor *unk0;
    Struct8002CBEC_AAD_20_0_4 *unk4;
} Struct8002CBEC_AAD_20_0;
typedef struct Struct8002CBEC_AAD_20 {
    Struct8002CBEC_AAD_20_0 unk0[2];
} Struct8002CBEC_AAD_20;
typedef struct Struct8002CBEC_AAD {
    u8 unk0;
    u8 pad1[0x14 - 0x1];
    s16 unk14;
    s16 unk16;
    s16 unk18;
    s16 unk1A;
    u8 pad1C[2];
    u8 unk1E;
    u8 unk1F;
    Struct8002CBEC_AAD_20 *unk20;
} Struct8002CBEC_AAD;

typedef struct {
    f32 unk0;
    f32 unk4;
    f32 unk8;
    f32 unkC;
    f32 unk10;
} AAD_race_8002BDDC;

// @recomp: Draw placement of dots in Tiny Car Race
RECOMP_PATCH Gfx *func_race_8002BDDC(Gfx *dl, Actor *arg1, f32 arg2, f32 arg3, u8 arg4, u8 arg5, u8 arg6) {
    f32 x, y;
    AAD_race_8002BDDC *aaD;

    aaD = arg1->additional_actor_data;
    x = aaD->unkC - arg2;
    y = aaD->unk10 - arg3;
    x *= aaD->unk4;
    y *= aaD->unk8;
    x = ((x * 0.5f) + 50.0f);
    x *= 4.0f;
    y = ((y * 0.5f) + 60.0f);
    y *= 4.0f;
    gDPSetPrimColor(dl++, 0, 0, arg4, arg5, arg6, 0xC8);
    return displayImage_simple(dl, (x / 4.0f) - 2, (y / 4.0f) - 2, 16, 16, 0x4A, G_IM_FMT_IA, 1, 0.25f, 0.25f, 0, 0);
}

extern Gfx *func_race_8002BEE8(Gfx *dl, Actor *arg1);

// @recomp: Render Tiny Car Race Map
RECOMP_PATCH Gfx* func_race_8002CBEC(Gfx* dl, Actor* arg1) {
    s32 var_s3;
    s32 var_v0;
    s32 var_v1;
    Struct8002CBEC_AAD_20_0_4* temp_s0;
    Struct8002CBEC_AAD* temp_s5;
    Struct8002CBEC_AAD_20* temp_s6;
    Actor* temp_t0;

    temp_s5 = arg1->AAD_as_array[0];
    if (temp_s5->unk0 & 1) {
        temp_s6 = temp_s5->unk20;
        gDPPipeSync(dl++);
        gSPTexture(dl++, 0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF);
        gDPSetRenderMode(dl++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
        dl = func_global_asm_805FD030(dl);
        dl = alignHUD(dl, ALIGN_LEFT);
        gDPSetPrimColor(dl++, 0, 0, 0x00, 0x00, 0x3C, 0xA0);
        gDPSetCycleType(dl++, G_CYC_1CYCLE);
        gDPSetCombineMode(dl++, G_CC_PRIMITIVE, G_CC_PRIMITIVE);
        gDPFillRectangle(dl++, 18, 28, 82, 92);
        gDPPipeSync(dl++);
        gDPSetPrimColor(dl++, 0, 0, 0x80, 0x00, 0xFF, 0x64);
        gDPFillRectangle(dl++, 16, 26, 84, 28);
        gDPFillRectangle(dl++, 16, 92, 84, 94);
        gDPFillRectangle(dl++, 16, 28, 18, 92);
        gDPFillRectangle(dl++, 82, 28, 84, 92);
        gDPPipeSync(dl++);
        gDPSetPrimColor(dl++, 0, 0, 0xFF, 0xFF, 0xFF, 0xB4);
        gDPSetCombineMode(dl++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
        gSPMatrix(dl++, &D_2000180, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPMatrix(dl++, &D_20000C0, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_PROJECTION);
        gSPTexture(dl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
        dl = displayImage_simple(dl, (136 >> 2) - 16, (304 >> 2) - 16, 64, 64, temp_s5->unk14, G_IM_FMT_IA, 1, 0.5f, 0.5f, 0, 0);
        dl = displayImage_simple(dl, (264 >> 2) - 16, (304 >> 2) - 16, 64, 64, temp_s5->unk16, G_IM_FMT_IA, 1, 0.5f, 0.5f, 0, 0);
        dl = displayImage_simple(dl, (136 >> 2) - 16, (176 >> 2) - 16, 64, 64, temp_s5->unk18, G_IM_FMT_IA, 1, 0.5f, 0.5f, 0, 0);
        dl = displayImage_simple(dl, (264 >> 2) - 16, (176 >> 2) - 16, 64, 64, temp_s5->unk1A, G_IM_FMT_IA, 1, 0.5f, 0.5f, 0, 0);
        for (var_s3 = 0; var_s3 < temp_s5->unk1E; var_s3++) {
            temp_t0 = temp_s6->unk0[var_s3].unk0;
            if (temp_t0 == NULL) {
                continue;
            }
            temp_s0 = temp_s6->unk0[var_s3].unk4;
            if (temp_s0->unk27 == 0) {
                var_v0 = 0x80;
                var_v1 = 0x80;
            } else if (temp_s0->unk27 == 1) {
                var_v0 = 0xFF;
                var_v1 = 0;
            } else {
                continue;
            }

            dl = func_race_8002BDDC(dl, arg1, temp_t0->x_position, temp_t0->z_position, var_v0, 0, var_v1);
            if (temp_s0->unk2C != NULL) {
                dl = func_race_8002BDDC(dl, arg1, temp_s0->unk2C->x_position, temp_s0->unk2C->z_position, 0xFF, 0xC8, 0);
            }
        }
        dl = popHUD(dl, ALIGN_LEFT);
    }
    return func_race_8002BEE8(dl, arg1);
}

extern s32 D_race_8002FCC0[];
typedef struct Struct8002C67C_48 {
    u8 pad0[0x4];
    s16 unk4;
    u8 pad6[0xA - 0x6];
    s16 unkA;
} Struct8002C67C_48;
typedef struct Struct8002C67C_AAD {
    u8 unk0;
    u8 pad1[0x1E - 0x1];
    u8 unk1E;
    u8 pad1F[0x24 - 0x1F];
    u8 unk24;
} Struct8002C67C_AAD;
typedef struct Struct8002C67C {
    u8 pad0[0x2A];
    u8 unk2A;
    u8 pad2B[0x30 - 0x2B];
    Actor *unk30;
    u8 unk34;
    u8 pad35;
    u8 unk36;
    u8 unk37;
    u8 pad38[0x45 - 0x38];
    u8 unk45;
    u8 unk46;
    u8 unk47;
    Struct8002C67C_48 *unk48;
    u8 pad4C[0x50 - 0x4C];
    Mtx unk50[2];
} Struct8002C67C;

// @recomp: Draw Missles
RECOMP_PATCH Gfx *func_race_8002C14C(Gfx *dl, Struct8002C67C *arg1) {
    // Draw Missiles (Factory Car Race)
    s16 temp_s4;
    s32 base_x;
    s32 y;
    s32 i;

    base_x = arg1->unk48->unk4 + 8;
    y = arg1->unk48->unkA - 38;

    gDPSetPrimColor(dl++, 0, 0, 0xFF, 0xC8, 0x00, 0xB4);
    dl = alignHUD(dl, ALIGN_LEFT);
    for (i = 0; i < arg1->unk2A; i++) {
        dl = displayImage_simple(dl,
            (base_x) + (i * 8),
            y,
            16, 16,
            0x4A, G_IM_FMT_IA, 1,
            0.5f, 0.5f, 0, 0
        );
    }
    dl = popHUD(dl, ALIGN_LEFT);
    if (arg1->unk2A) {
        gSPMatrix(dl++, &D_2000180, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    }
    return dl;
}

// @recomp: Race position UI
RECOMP_PATCH Gfx* func_race_8002C76C(Gfx* dl, Struct8002C67C* arg1) {
    Struct8002C67C_AAD* temp_t1;
    s32 pad;
    s32 pad2;
    u8* sp68;
    u8 var_t0;
    u8 var_v0;
    s32 sp60;
    s32 sp5C;
    u8 str[0x40];

    var_t0 = arg1->unk37;
    temp_t1 = arg1->unk30->AAD_as_array[0];
    sp60 = arg1->unk48->unk4 + 8;
    sp5C = arg1->unk48->unkA - 0x10;
    gDPPipeSync(dl++);
    gDPSetPrimColor(dl++, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF);

    if (temp_t1->unk0 & 4) {
        if (var_t0 < temp_t1->unk24) {
            // @recomp: The below line is in the match for this function, but causes a compilation failure
            // var_t0 &= var_t0; // ????
        } else {
            var_t0 = temp_t1->unk24;
        }
        var_t0 = MAX(var_t0, 1);
        // do while(0) required here as we need the scoped sp50/4C/48 
        do {
            u8 *sp50 = getTextString(0x26U, 0xB, 1);
            u8 *sp4C = getTextString(0x26U, 0xC, 1);
            _sprintf(str, "%s %d %s %d", sp50, var_t0, sp4C, temp_t1->unk24);
            dl = alignHUD(dl, ALIGN_LEFT);
            dl = printStyledText(dl, 1, 2.0f * (sp60 * 4), 2.0f * (sp5C * 4), str, 4U);
            dl = popHUD(dl, ALIGN_LEFT);
            gSPMatrix(dl++, &D_20000C0, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_PROJECTION);
            gSPMatrix(dl++, &arg1->unk50[D_global_asm_807444FC], G_MTX_NOPUSH | G_MTX_MUL | G_MTX_PROJECTION);
            sp5C -= (0.5f * func_global_asm_806FD894(1));
        } while (0);
    }
    if (temp_t1->unk0 & 2) {
        var_v0 = arg1->unk36;
        if (temp_t1->unk1E == (var_v0 + 1)) {
            var_v0 = 3;
        }
        if (arg1->unk45) {
            var_v0 = 4;
        }
        dl = alignHUD(dl, ALIGN_LEFT);
        dl = printStyledText(dl, 1, 2.0f * (sp60 * 4), 2.0f * (sp5C * 4), getTextString(0x26U, D_race_8002FCC0[var_v0], 1), 4U);
        dl = popHUD(dl, ALIGN_LEFT);
    }
    if ((arg1->unk34 > 2) && (arg1->unk34 < 5)) {
        dl = func_global_asm_806FE078(dl, arg1->unk46, 2, 160.0f, 100.0f, 0.0f, 1.5f);
    }
    return dl;
}

// @recomp: Race missed gates UI
RECOMP_PATCH Gfx *func_race_8002C2E8(Gfx *dl, RaceAdditionalActorData *arg1) {
    f32 temp_f20;
    f32 base_x;
    f32 temp_f24;
    f32 w;
    u32 temp_v0;
    f32 temp_f0; // 70
    f32 a;
    f32 c;
    s32 i;
    u8 *temp_v0_2;
    Struct8002C67C_48 *temp_s0;
    f32 temp;

    temp_s0 = (Struct8002C67C_48*)arg1->unk48;
    temp_v0 = func_global_asm_806FD894(1);
    temp_f0 = temp_v0 * 0.5f;
    temp_f20 = temp_s0->unk4 + 8;
    temp_f24 = (temp_s0->unkA - 0x10);
    temp_f24 -= (2 * temp_f0);

    dl = alignHUD(dl, ALIGN_LEFT);
    temp_v0_2 = getTextString(0x26U, 0xA, 1);
    gDPSetPrimColor(dl++, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF);
    dl = printStyledText(dl, 1, (2.0f * (temp_f20 * 4.0f)), (2.0f * (temp_f24 * 4.0f)), temp_v0_2, 4U);
    gSPMatrix(dl++, &D_20000C0, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_PROJECTION);
    gSPMatrix(dl++, &arg1->unk50[D_global_asm_807444FC], G_MTX_NOPUSH | G_MTX_MUL | G_MTX_PROJECTION);
    c = getCenterOfString(1, temp_v0_2);
    temp_f20 += (0.5f * c);
    base_x = temp_f20 + 1.0;
    temp = temp_f24 + (0.4 * temp_f0) - 6;
    a = 1.5f;
    gDPSetPrimColor(dl++, 0, 0, 0xFF, 0xFF, 0x00, 0xFF);
    for (i = 0; i < 5; i++) {
        w = base_x + (i * 12);
        dl = displayImage_simple(dl,
            w, temp,
            64, 64,
            0x4B, G_IM_FMT_IA, 1,
            0.1875f, 0.1875f,
            0, 0
        );
    }
    gDPSetPrimColor(dl++, 0, 0, 0xFF, 0x00, 0x00, 0xFF);
    for (i = 0; i < 5 - arg1->unk44; i++) {
        w = base_x + (i * 12);
        dl = displayImage_simple(dl,
            w, temp,
            64, 64,
            0x45, G_IM_FMT_IA, 1,
            0.1875f, 0.1875f,
            0, 0
        );
    }
    dl = popHUD(dl, ALIGN_LEFT);
    gSPMatrix(dl++, &D_2000180, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    return dl;
}

// @recomp: Batty BB
RECOMP_PATCH void func_bonus_8002570C(void) {
    PlayerAdditionalActorData *sp4C;
    void *temp_a0;
    u8 i;
    s32 t;
    u32 u;
    HandleAAD *aaD;
    u8 div;

    sp4C = gPlayerPointer->additional_actor_data;
    aaD = gCurrentActorPointer->additional_actor_data;
    if (ACTOR_UNINITIALIZED(gCurrentActorPointer)) {
        gCurrentActorPointer->x_position = 52.0f;
        gCurrentActorPointer->z_position = 18.0f;
        aaD->unk19 = func_global_asm_806FDB8C(1, (u8*)"HIT", 8, 0.0f, 0.0f, 0.0f);
        func_global_asm_806FDAB8(aaD->unk19, 0.0f);
        aaD->unk16 = 3;
        aaD->unk14 = 3;
        aaD->reels[0] = func_bonus_800253E4(0x90, -0x15, 1, 0x12);
        aaD->reels[1] = func_bonus_800253E4(0x91, -7, 1, 0x12);
        aaD->reels[2] = func_bonus_800253E4(0x92, 7, 1, 0x12);
        aaD->reels[3] = func_bonus_800253E4(0x93, 0x15, 1, 0x12);
        setAction(0x49, NULL, 0U);
        func_global_asm_8061C6A8(sp4C->unk104, gPlayerPointer, 6, 0, 0xAA, 0, 0, 0, 0, 0, 1.0f);
        playCutscene(NULL, 1, 1);
        switch (current_map) {
            case MAP_BATTY_BARREL_BANDIT_EASY:
                aaD->unk1C = 0x2DU;
                aaD->unk1E = 0x20U;
                aaD->unk1D = 0x20U;
                break;
            case MAP_BATTY_BARREL_BANDIT_EASY_2:
                aaD->unk1C = 0x2DU;
                aaD->unk1E = 0x34U;
                aaD->unk1D = 0x20U;
                break;
            case MAP_BATTY_BARREL_BANDIT_NORMAL:
                aaD->unk1C = 0x28U;
                aaD->unk1E = 0x34U;
                aaD->unk1D = 0x2AU;
                break;
            case MAP_BATTY_BARREL_BANDIT_HARD:
                aaD->unk1C = 0x23U;
                aaD->unk1E = 0x40U;
                aaD->unk1D = 0x37U;
            default:
                break;
        }
    }
    if ((aaD->unk1A != 0) && (gCurrentActorPointer->unk11C->control_state == 5)) {
        gCurrentActorPointer->control_state = 1;
        gCurrentActorPointer->control_state_progress = 0;
    }
    switch (gCurrentActorPointer->control_state) {
        case 0:
            switch (gCurrentActorPointer->control_state_progress) {
                case 0:
                    if (is_cutscene_active != 1) {
                        loadText(gCurrentActorPointer, 0U, 3U);
                        gCurrentActorPointer->control_state_progress += 1;
                    }
                    break;
                case 1:
                    if (!(gCurrentActorPointer->object_properties_bitfield & 0x02000000)) {
                        gCurrentActorPointer->control_state = 3;
                        gCurrentActorPointer->control_state_progress = 0;
                        setSpriteAlignment(ALIGN_LEFT);
                        func_global_asm_80714998(2U);
                        func_global_asm_807149FC(-1);
                        aaD->unk20 = drawSpriteAtPosition(&D_global_asm_80720CF0, 1.0f, 40.0f, 200.0f, 5.0f);
                        break;
                    }
                    break;
            }
            break;
        case 3:
            switch (gCurrentActorPointer->control_state_progress) {
                case 0:
                    func_global_asm_806A2A10(0xDC, 0x2A, aaD->unk1C);
                    gCurrentActorPointer->control_state_progress = 1;
                case 1:
                    if (aaD->reels[3]->control_state == 0) {
                        gCurrentActorPointer->control_state = 2;
                    }
                    break;
            }
            break;
        case 4:
            switch (gCurrentActorPointer->control_state_progress) {
                case 0:
                    aaD->unk10 = 0x400;
                    aaD->unk12 = 0x40;
                    gCurrentActorPointer->control_state_progress += 1;
                    playSoundAtActorPosition(gCurrentActorPointer, 0x179, 0xFFU, 0x7F, 1U);
                    if (aaD->unk1A == 0) {
                        aaD->unk1A = 1U;
                        func_global_asm_806A2B08(gCurrentActorPointer->unk11C);
                        playSong(MUSIC_8_BONUS_MINIGAMES, 1.0f);
                    }
                    break;
                case 1:
                    gCurrentActorPointer->z_rotation = (func_global_asm_80612794(aaD->unk10) * -1024.0f) + 1024.0f;
                    aaD->unk10 += aaD->unk12;
                    if (aaD->unk10 >= 0x800) {
                        aaD->unk12 = -0x40;
                        div = 2;
                        func_bonus_800256C4(aaD,
                            (u32) (aaD->unk1E +
                            ((aaD->unk1D - aaD->unk1E) *
                            ((aaD->unk16 - 1) / (f32) div))));
                        gCurrentActorPointer->unk6E[0] = playSound(0x24D, 0x7FFFU, 64.0f, 1.0f, 0, 0);
                        func_global_asm_8061C464(sp4C->unk104, gCurrentPlayer, 4, 0, 0x78, 0, 0, 0, 0, 0, 0.09f);
                    } else if (aaD->unk10 < 0x400) {
                        gCurrentActorPointer->control_state = 5;
                        gCurrentActorPointer->control_state_progress = 0;
                    }
                    break;
            }
            break;
        case 5:
            if (gCurrentActorPointer->unk168 == 4) {
                gCurrentActorPointer->control_state = 7;
                gCurrentActorPointer->control_state_progress = 0;
                break;
            }
            if (gCurrentActorPointer->control_state_progress != 0) {
                func_bonus_800254B0(
                    aaD->reels[gCurrentActorPointer->unk168]->x_position, 
                    aaD->reels[gCurrentActorPointer->unk168]->y_position, 
                    aaD->reels[gCurrentActorPointer->unk168]->z_position + 20.0f, 0xF);
                aaD->reels[gCurrentActorPointer->unk168]->control_state++;
                gCurrentActorPointer->unk168++;
                gCurrentActorPointer->control_state = 6;
                gCurrentActorPointer->control_state_progress = 0;
            }
            break;
        case 7:
            temp_a0 = D_global_asm_807457E4[gCurrentActorPointer->unk6E[0]];
            if (temp_a0) {
                func_global_asm_80737924(temp_a0);
            }
        case 1:
            if (gCurrentActorPointer->unk11C->control_state != 5) {
                if ((func_bonus_80025480(aaD, 0) == 0) && 
                    (func_bonus_80025480(aaD, 0) == func_bonus_80025480(aaD, 1)) &&
                    (func_bonus_80025480(aaD, 1) == func_bonus_80025480(aaD, 2)) &&
                    (func_bonus_80025480(aaD, 2) == func_bonus_80025480(aaD, 3))) {
                    func_global_asm_806FDAB8(aaD->unk19, MATH_PI_F);
                    aaD->unk16--;
                    if (aaD->unk16 == 0) {
                        gPlayerPointer->control_state_progress = 1;
                        func_bonus_800264E0(0U, 0U);
                        gCurrentActorPointer->control_state = 8;
                        aaD->unk10 = 0;
                        aaD->unk1A = 0U;
                        playCutscene(NULL, 0, 0x11);
                    } else {
                        func_global_asm_8069D2AC(0U, 0, 0xB4, (u8*)getTextString(0x1AU, 0xB, 1), 0U, 0x28U, 8U, 8U);
                        func_bonus_8002563C(aaD);
                    }
                } else {
                    func_global_asm_8069D2AC(0U, 0, 0xB4, (u8*)getTextString(0x1AU, 0xC, 1), 0U, 0x28U, 8U, 8U);
                    func_bonus_8002563C(aaD);
                }
            } else {
                gPlayerPointer->control_state_progress = 2U;
                func_bonus_800265C0(0, 1);
                gCurrentActorPointer->control_state = 9;
                aaD->unk10 = 0;
                aaD->unk1A = 0U;
                playCutscene(NULL, 0, 0x11);
            }
            break;
        case 8:
            for (i = 0; i < 4; i++) {
                if (!((s32)((aaD->unk10 - (i * 8)) + 0x18) % 32)) {
                    func_bonus_800254B0(
                        aaD->reels[i]->x_position,
                        aaD->reels[i]->y_position,
                        aaD->reels[i]->z_position + 20.0f, 0xF);
                }
            }
        case 9:
            aaD->unk10++;
            break;
    }
    if ((gCurrentActorPointer->control_state > 0) && (gCurrentActorPointer->control_state < 8)) {
        addActorToTextOverlayRenderArray(func_bonus_800252A0, gCurrentActorPointer, 3U);
    }
    if (gCurrentActorPointer->control_state >= 8) {
        if (aaD->unk20) {
            func_global_asm_80715908(aaD->unk20);
            aaD->unk20 = NULL;
        }
    }
    renderActor(gCurrentActorPointer, 0U);
}

typedef struct Struct80755264 {
    u8 unk0;
    u8 unk1;
    u8 unk2;
    u8 unk3;
    u16 *unk4;
    u8 unk8;
    u8 unk9;
    u8 unkA;
    u8 unkB;
    Vtx *unkC;
    void *unk10;
} Struct80755264;

typedef struct Struct807FD9E0_unk4 {
    s16 unk0;
    u8 pad2[0x8 - 0x2];
    f32 unk8;
    f32 unkC;
    f32 unk10;
    u8 pad14[0x20 - 0x14];
    s16 unk20;
    s16 unk22;
    s16 unk24;
    u8 pad26[2];
    f32 unk28;
    u8 pad2C[0x30 - 0x2C];
    Mtx unk30[2];
    u8 unkB0;
    u8 unkB1;
    u8 unkB2;
    u8 padB3[0xB8 - 0xB3];
} Struct807FD9E0_unk4;

typedef struct Struct807FD9E0 {
    u16 unk0;
    u8 pad2[2];
    Struct807FD9E0_unk4 *unk4;
} Struct807FD9E0;

Gfx* func_global_asm_807105D4(Gfx*, u8);
extern Struct807FD9E0* D_global_asm_807550E0;
extern Struct80755264 D_global_asm_80755264[];
extern f32 D_global_asm_807FD9EC;
extern u8 D_global_asm_807FDA1A;
extern u8 D_global_asm_807FDA1B;
extern u8 D_global_asm_807FDA1C;
extern s16 D_global_asm_807FDA1E;
extern void* D_global_asm_807FDA20;
extern s32 (*D_global_asm_807FDA24)(Struct807FD9E0_unk4*);
extern u8 D_global_asm_807FDA30[];
extern Struct807FD9E0_unk4* D_global_asm_807FDAB0;
extern s16 D_global_asm_807FDAB4;
extern Actor *D_global_asm_807F5D10;
extern PlayerAdditionalActorData *extra_player_info_pointer;
extern Gfx **D_1000020;
void func_global_asm_80612CA0(f32 (*arg0)[4], f32 arg1);

f32 morphWeather(f32 val) {
    s32 width, height;
    s32 pillar_left;
    recomp_get_ui_bounds(&width, &height);
    pillar_left = -((width - 320) >> 1);
    return (val * ((f32)width / 320.0f)) + pillar_left;
}

//@recomp: Weather renderer
RECOMP_PATCH Gfx* func_global_asm_80710CA0(Gfx* dl, Actor* arg1) {
    f64 var_f2_2;
    f32 sp108[4][4];
    f32 spC8[4][4];
    f32 temp_f0;
    f32 var_f2;
    u8 spBF;
    u8 spBE;
    f64 temp_f0_2;
    u16 temp_s0;
    u16 var_s6;
    s8 var_s5;
    CameraPaad* temp_a2;
    Struct807FD9E0_unk4* temp_s0_2;

    temp_s0 = D_global_asm_807550E0->unk0;
    temp_a2 = D_global_asm_807F5D10->AAD_as_array[0];
    spBE = FALSE;
    spBF = FALSE;
    var_s6 = 0;
    if ((gPlayerPointer->control_state == 3) || (gPlayerPointer->control_state == 5)) {
        temp_f0 = D_global_asm_807F5D10->distance_from_floor;
        if (temp_f0 > 180.0f) {
            var_f2 = 0.0f;
        } else {
            var_f2 = 180.0f - temp_f0;
        }
        D_global_asm_807FD9EC = var_f2 * 0.02 * 240.0;
    } else {
        D_global_asm_807FD9EC = 240.0f;
    }
    if (gPlayerPointer->control_state == 0x42) {
        if ((extra_player_info_pointer->unkBC == 0x62) || (extra_player_info_pointer->unkBC == 0x88)) {
            spBE = TRUE;
        }
    }
    if ((temp_a2->unkFA != 0) && (character_change_array->look_at_eye[1] < (temp_a2->unk90 + 3.0f))) {
        spBF = TRUE;
    }
    gDPPipeSync(dl++);
    gSPDisplayList(dl++, &D_1000118);
    gSPDisplayList(dl++, &D_1000020);
    gSPMatrix(dl++, &D_20000C0, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_PROJECTION);
    gSPTexture(dl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
    gDPSetCombineMode(dl++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
    gDPSetTextureFilter(dl++, G_TF_BILERP);
    gDPSetRenderMode(dl++, G_RM_CLD_SURF, G_RM_CLD_SURF2);
    // 
    dl = alignHUD(dl, ALIGN_LEFT);
    // 
    if (D_global_asm_80755264[D_global_asm_807FDA1C].unk0 == 1) {
        dl = func_global_asm_807105D4(dl, 0);
    }
    var_s5 = 0x7F;
    if (var_s5 >= 0) {
        while ((var_s5 >= 0) && (temp_s0 != var_s6)) {
            temp_s0_2 = &D_global_asm_807550E0->unk4[var_s5];
            D_global_asm_807FDAB0 = temp_s0_2;
            D_global_asm_807FDAB4 = D_global_asm_807FDA1E;
            if ((temp_s0_2->unkB0 != 0) && (((temp_s0_2->unk0 == -1)) || (func_global_asm_806522CC(temp_s0_2->unk8, temp_s0_2->unkC, temp_s0_2->unk0))) && (!spBF) && (!spBE)) {
                gDPSetPrimColor(dl++, 0, 0, D_global_asm_807FDA1B, D_global_asm_807FDA1B, D_global_asm_807FDA1B, temp_s0_2->unkB1);
                if (D_global_asm_807FDA1C == 4) {
                    switch (temp_s0_2->unk20) {
                        case 8:
                        case 10:
                        case 12:
                            dl = func_global_asm_807105D4(dl, ((temp_s0_2->unkB2++ >> 2) % 8) + 2);
                            break;
                        case 15:
                        case 18:
                        case 22:
                            dl = func_global_asm_807105D4(dl, 0);
                            break;
                        default:
                            dl = func_global_asm_807105D4(dl, 1);
                            break;
                    }
                } else {
                    if (D_global_asm_80755264[D_global_asm_807FDA1C].unk0 > 1) {
                        dl = func_global_asm_807105D4(dl, temp_s0_2->unkB2++ % D_global_asm_80755264[D_global_asm_807FDA1C].unk0);
                    }
                }
                temp_f0_2 = temp_s0_2->unk10 * 4.0;
                var_f2_2 = ABS_D(temp_f0_2);
                guScaleF(sp108, temp_f0_2, var_f2_2, 1.0f);
                switch (D_global_asm_807FDA1C) {
                    case 3:
                        temp_s0_2->unk22 += temp_s0_2->unk28;
                        func_global_asm_80612CA0(spC8, temp_s0_2->unk22);
                        break;
                    case 4:
                        func_global_asm_80612CA0(spC8, temp_s0_2->unk24);
                        break;
                    default:
                        func_global_asm_80612CA0(spC8, D_global_asm_807FDA1E);
                        break;
                }
                guMtxCatF(sp108, spC8, sp108);
                guTranslateF(spC8, morphWeather(temp_s0_2->unk8) * 4.0, temp_s0_2->unkC * 4.0, -50.0f);
                guMtxCatF(sp108, spC8, sp108);
                guMtxF2L(sp108, &temp_s0_2->unk30[D_global_asm_807444FC]);
                gSPMatrix(dl++, &temp_s0_2->unk30[D_global_asm_807444FC], G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
                gSPVertex(dl++, osVirtualToPhysical(D_global_asm_807FDA20), 4, 0);
                gSP2Triangles(dl++, 0, 1, 2, 0, 0, 2, 3, 0);
                gDPPipeSync(dl++);
            }
            if (temp_s0_2->unkB0) {
                var_s6++;
                if (D_global_asm_807FDA24(temp_s0_2) || (temp_s0_2->unk10 < 0.01)) {
                    temp_s0_2->unkB0 = 0U;
                    D_global_asm_807FDA1A++;
                    D_global_asm_807FDA30[D_global_asm_807FDA1A] = var_s5;
                    D_global_asm_807550E0->unk0--;
                }
            }
            var_s5--;
        }
    }

    dl = popHUD(dl, ALIGN_LEFT);
    return dl;
}

//@recomp Reposition where items go when they fly to the HUD.
RECOMP_PATCH void func_global_asm_806F8170(s32 HUDItemIndex, f32 *xOut, f32 *yOut, f32 *zOut) {
    s32 temp_v0;

    if (HUDItemIndex < 0) {
        func_global_asm_806F8004(16.5f, 0.0f, xOut, yOut, zOut);
        return;
    }
    if (HUDItemIndex == 8) {
        func_global_asm_806F8004(1.5 - (D_global_asm_80754280->hud_item[HUDItemIndex].hud_count * 7), 26.0f, xOut, yOut, zOut);
        return;
    }
    s32 width, height;
    recomp_get_ui_bounds(&width, &height);
    *xOut = (D_global_asm_80754280->hud_item[HUDItemIndex].unk_10 * width) / 320.0f;
    *yOut = D_global_asm_80754280->hud_item[HUDItemIndex].unk_14;
    *zOut = D_global_asm_80754280->hud_item[HUDItemIndex].unk_18;
}

//@recomp Fairy Camera shutter animation scaled to screen width.
RECOMP_PATCH Gfx *func_global_asm_806FFB2C(Gfx *dl, Actor *arg1) {
    f32 sp3C;
    sp3C = arg1->control_state_progress * 1.4;
    dl = func_global_asm_806FEDB0(dl, arg1->PaaD->unk1A4);
    gDPSetRenderMode(dl++, G_RM_CLD_SURF, G_RM_CLD_SURF2);
    gDPSetPrimColor(dl++, 0, 0, 0x00, 0x00, 0x00, 0xFF);
    s32 width, height;
    recomp_get_ui_bounds(&width, &height);
    // We need to bump the scale up a bit to account for float imprecision,
    // otherwise it leaves a 1px gap on the left.
    return displayImage(dl, 0x3C, 3, 1, 0x40, 0x40, 0xA0, 0x78, (5.0f * width) / 320.0f + 1.0f, (5.0f * width) / 320.0f + 1.0f, 0, sp3C);
}


Gfx* func_global_asm_806ABA6C(Gfx*, void*, s32);
extern s8 D_global_asm_80750530[];
extern s8 D_global_asm_80750538;
extern s8 D_global_asm_80750560[];
extern s16 D_global_asm_8075056C[];
extern s8 D_global_asm_807505CC;
extern u8 **D_global_asm_807FC7E0;
typedef struct Struct807FC7F0 {
    f32 unk0;
    f32 unk4;
    s16 unk8;
    s16 unkA;
    s16 unkC;
    s16 unkE;
    s8 unk10;
    u8 unk11[7];
} Struct807FC7F0;
extern Struct807FC7F0 *D_global_asm_807FC7F0[];
extern s8 D_global_asm_807FC7F8[];
extern s8 D_global_asm_807FC80C;
extern s8 D_global_asm_807FC818[];
extern f32 D_global_asm_807FC840[];
extern f32 D_global_asm_807FC868[];
extern f32 D_global_asm_807FC890[];
extern s8 D_global_asm_807FCC4B;
extern u8 D_global_asm_807FCC4C;
typedef struct character_progress {
    u8 moves;
    u8 simian_slam;
    u8 weapon;
    u8 ammo_belt;
    u8 instrument;
    u8 unk5;
    u16 coins;
    u16 instrument_ammo;
    u16 coloured_bananas[14];
    u16 coloured_bananas_fed_to_tns[14];
    u16 golden_bananas[14];
} CharacterProgress;
typedef struct PlayerProgress {
    union {
        CharacterProgress character_progress[6];
        u8 character_progress_as_bytes[6][0x5E];
        u16 character_progress_as_shorts[6][0x2F];
    };
    u8 unk234[0x2F0 - 0x234];
    u16 standardAmmo;
    u16 homingAmmo;
    u16 oranges;
    u16 crystals;
    u16 film;
    s8 unk2FA;
    s8 health;
    u8 melons;
    s8 unk2FD;
    u16 unk2FE[(0x306 - 0x2FE) / 2];
} PlayerProgress;

typedef struct PauseAAD {
    f32 unk0;
    f32 unk4;
    s16 unk8[2];
    s16 unkC[2];
    s16 unk10;
    s8 unk12;
    u8 unk13;
    s8 unk14;
    s8 unk15;
    s8 unk16;
    s8 unk17;
    u8 unk18;
} PauseAAD;

extern void *_malloc(s32);
extern PlayerProgress D_global_asm_807FC950[];
extern u8 getLevelIndex(u8 map, u8 arg1);
extern u8 D_global_asm_8076A0AB;
extern Gfx *func_global_asm_806A921C(Gfx *dl);
extern Gfx *displayImage(Gfx *dl, u16 textureIndex, s32 arg3, u32 codec, s32 width, s32 height, s16 x, s16 y, f32 xScale, f32 yScale, s32 arg11, f32 arg12);
extern Gfx *printText(Gfx *dl, s16 x, s16 y, f32 scale, u8 *string);
extern f32 func_global_asm_80612790(s16 arg0);
extern f32 func_global_asm_80612794(s16 arg0);
extern Gfx *func_global_asm_806AA09C(s16 x, s16 y, s16 arg2, s16 arg3, Gfx *dl, s8 arg5, f32 scale);
extern s16 D_global_asm_807FC828[];
extern u8 func_global_asm_80712628(void);
extern void set_sprite_interpolation_lockdown(s32 value);

// @recomp: Pause Menu Text renderer
RECOMP_PATCH Gfx* func_global_asm_806A92B4(Gfx *dl, Actor *arg1) {
    u8 sp160[0x40];
    PauseAAD* sp15C;
    s16 i;
    s16 j;
    PlayerProgress *player;
    s32 world;
    Gfx* local_dl;
    f32 temp_f0;
    s16 temp_f18;
    Gfx* dl_copy;
    f32 temp_f20;
    f32 temp_f22;
    f32 temp_f24;
    f32 temp_f8;
    f32 var_f0;
    f32 var_f14;
    f32 var_f2;
    s16 temp_s0;
    s16 temp_s4_2;
    s16 temp_t1;
    s16 var_s1;
    s32 var_s2_2;
    s16 var_s4;
    s32 gb_count;
    f32 temp_f8_2;
    s32 temp_t7;
    u8 temp_t8; // a8
    s32 temp_t8_2;
    s32 temp_t9_2;
    s32 var_v1;
    s8 health;
    Struct807FC7F0 *temp_v0_2;
    s32 gb_count_0;
    s32 x;
    s32 y;
    s32 offset;
    u8 update_alignment;

    dl_copy = dl;
    sp15C = arg1->AAD_as_array[0];
    player = &D_global_asm_807FC950[0];
    world = getLevelIndex(D_global_asm_8076A0AB, 1U);
    local_dl = _malloc(8000);
    func_global_asm_8061134C(local_dl);
    gSPDisplayList(dl_copy++, local_dl);
    dl = func_global_asm_806A921C(local_dl);
    for (j = 0; j < 2; j++) {
        if (D_global_asm_807FC7F0[j]) {
            for (i = 0; i < D_global_asm_807FC7F8[j]; i++) {
                var_f0 = sp15C->unk0;
                temp_v0_2 = &D_global_asm_807FC7F0[j][i];
                if (temp_v0_2->unk10 == 2) {
                    var_f0 += sp15C->unk4;
                }
                x = (temp_v0_2->unk8 * 4) + (s32)(var_f0 * temp_v0_2->unk0) + (s16)(temp_v0_2->unkC * 4);
                y = (temp_v0_2->unkA * 4) + (s32)(var_f0 * temp_v0_2->unk4) + (temp_v0_2->unkE * 4);
                y += (sp15C->unkC[j] * 4);
                update_alignment = FALSE;
                if (x > (240 * 4)) {
                    dl = alignHUD(dl, ALIGN_RIGHT);
                    update_alignment = TRUE;
                } else if (x < (80 * 4)) {
                    dl = alignHUD(dl, ALIGN_LEFT);
                    update_alignment = TRUE;
                }
                dl = printStyledText(dl, 1, x, y, temp_v0_2->unk11, 1U);
                if (update_alignment) {
                    if (x > (240 * 4)) {
                        dl = popHUD(dl, ALIGN_RIGHT);
                    } else if (x < (80 * 4)) {
                        dl = popHUD(dl, ALIGN_LEFT);
                    }
                }
            }
        }
    }
    switch (sp15C->unk12) {
    case 0:
        temp_t8 = (player->melons * 0x34) - 0x34;
        health = player->health;
        temp_s4_2 = 0x140;
        temp_s4_2 = 0xA0 - (s32) (temp_s4_2 * sp15C->unk0);
        for (i = 0; i < player->melons; i++) {
            temp_s0 = (i * 1024) + (object_timer * 200);
            temp_f24 = (func_global_asm_80612794(temp_s0) * 0.3f) + 3.0f;
            temp_f20 = (func_global_asm_80612794(temp_s0 + 0x800) * 0.3f) + 3.0f;
            temp_f22 = func_global_asm_80612794((temp_s0 >> 1) + 0x200) * 10.0f;
            temp_f8 = ((func_global_asm_80612794(temp_s0) * 0.5f) + 0.5f) * 30.0f;
            temp_f24 *= 0.75f;
            temp_f8 *= 0.75f;
            gDPPipeSync(dl++);
            gDPSetPrimColor(dl++, 0, 0, 0xFF, 0xFF, 0xFF, 0x50);
            temp_f8_2 = (((i * 0x69) + 0x280) + (temp_f22 * 0.75f)) - temp_t8;
            dl = displayImage(dl, 0x5DU, G_IM_FMT_RGBA, G_IM_SIZ_16b, 0x30, 0x2A,
                temp_f8_2, temp_s4_2 + (s16)temp_f8,
                temp_f24, -(temp_f20 * 0.75f), 0, 0);
            gDPPipeSync(dl++);
            gDPSetPrimColor(dl++, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF);
            if (health > 0) {
                var_v1 = MIN(health - 1, 3);
                dl = displayImage(dl, var_v1 + 0x5A, G_IM_FMT_RGBA, G_IM_SIZ_16b, 0x30, 0x2A,
                    temp_f8_2, temp_s4_2 + temp_f8,
                    temp_f24, -(temp_f20 * 0.75f), 0, 0);
            }
            health = MAX(0, health - 4);
        }
        gDPPipeSync(dl++);
        var_s1 = 0x198;
        var_s4 = (1.0f - sp15C->unk0) * 255.0f;
        if (var_s4 < 0) {
            var_s4 = 0;
        }
        if (var_s4 >= 0x100) {
            var_s4 = 0xFF;
        }
        if (sp15C->unk17 != 3) {
            for (i = 0; i < 3; i++) {
                if (i == sp15C->unk17) {
                    gDPSetPrimColor(dl++, 0, 0, 0xFF, 0xFF, 0xFF, var_s4);
                } else {
                    gDPSetPrimColor(dl++, 0, 0, 0xFF, 0xFF, 0xFF, var_s4 >> 1);
                }
                if ((i == 2) && ((world == 7) || ((world == 8) && (func_global_asm_80712628()) && (D_global_asm_807505CC == 0)))) {
                    i++;
                }
                j = i;
                if ((i == 1) && (D_global_asm_807505CC != 0)) {
                    j = 4;
                }
                if ((j == 2) && (D_global_asm_807505CC != 0)) {
                    j = 5;
                }
                dl = printText(dl, 0x280, var_s1, 0.7f, D_global_asm_807FC7E0[j]);
                var_s1 += 0x44;
            }
        } else {
            gDPSetPrimColor(dl++, 0, 0, 0xFF, 0xFF, 0xFF, var_s4);
            dl = printText(dl, 0x280, 0x198, 0.7f, D_global_asm_807FC7E0[7]);
            _sprintf(sp160, "q %s", D_global_asm_807FC7E0[0x10]);
            dl = printText(dl, 0x280, 0x1E8, 1, sp160);
            _sprintf(sp160, "b %s", D_global_asm_807FC7E0[0x11]);
            dl = printText(dl, 0x280, 0x238, 1, sp160);
        }
        break;
    case 1:
        for (j = 0; j < 2; j++) {
            temp_t8_2 = sp15C->unkC[j] << 2;
            var_s2_2 = -sp15C->unk10;
            temp_f0 = 1.0f - sp15C->unk0;
            if ((temp_t8_2 < 0x3B6) && (temp_t8_2 >= -0x3B5)) {
                if (temp_f0 > 0.05f) {
                    temp_f20 = 180.0f * temp_f0;
                    temp_f0 *= 3;
                    i = 0;
                    while (i < 5) {
                        temp_s0 = (s32) (func_global_asm_80612794(var_s2_2) * temp_f20) + 0x280;
                        temp_f18 = func_global_asm_80612790(var_s2_2) * temp_f20;
                        offset = i + (j * 5);
                        D_global_asm_807FC840[offset] = (s16) (temp_s0 + 32) - (6 * temp_f0);
                        D_global_asm_807FC868[offset] = (s16) ((s32) (((32 * temp_f0) + 480) - temp_f18) + temp_t8_2) - (30 * temp_f0);
                        D_global_asm_807FC890[offset] = temp_f0 * 1.33;
                        if (D_global_asm_80750530[i++]) {
                            D_global_asm_807FC890[offset] *= 0.75;
                        }
                        var_s2_2 += 0x333;                       
                    }
                    gDPPipeSync(dl++);
                }
                dl = func_global_asm_806ABA6C(dl, sp15C, j);
            }
            temp_t1 = sp15C->unk8[j];
            if (temp_t1 != 8) {
                var_f0 = MAX(sp15C->unk0, sp15C->unk4);
                var_f14 = 780.0f;
                if (temp_t8_2 == 0) {
                    var_f14 += (var_f0 * 400.0);
                }
                gb_count_0 = *(s16*)(&player->character_progress[sp15C->unk14].golden_bananas[D_global_asm_80750560[temp_t1]]);
                dl = func_global_asm_806AA09C(0x2A8,
                    temp_t8_2 + var_f14,
                    gb_count_0,
                    5, dl, 0, 0.8f);
            }
        }
        D_global_asm_80750538++;
        if (D_global_asm_80750538 >= 0xE) {
            D_global_asm_80750538 = 0;
        }
        break;
    case 2:
        for (j = 0; j < 2; j++) {
            temp_t9_2 = sp15C->unkC[j] * 4;
            if ((temp_t9_2 < 0x3B6) && (temp_t9_2 >= -0x3B5)) {
                dl = func_global_asm_806ABA6C(dl, sp15C, j);
            }
            gb_count = 0;
            for (i = 0; i < 5; i++) {
                gb_count += player->character_progress[i].golden_bananas[D_global_asm_80750560[sp15C->unk8[j]]];
            }
            if (sp15C->unk8[j] != 8) {
                var_f0 = MAX(sp15C->unk0, sp15C->unk4);
                var_f14 = 780.0f;
                if (temp_t9_2 == 0) {
                    var_f14 += (var_f0 * 400.0);
                }
                dl = func_global_asm_806AA09C(0x2B0, temp_t9_2 + var_f14, gb_count, 0x19, dl, 0, 0.8f);
            }
        }
        break;
    case 3:
        dl = printText(dl, 0x280, 0x74, 0.5f, D_global_asm_807FC7E0[0x12 + D_global_asm_807FC818[D_global_asm_807FC80C]]);
        dl = printText(dl, 0x280, 0x3C, 0.65, D_global_asm_807FC7E0[6]);
        var_f14 = MAX(sp15C->unk4, sp15C->unk0);
        var_f2 = (1.0f - var_f14) * 0.65f;
        if (var_f2 < 0.01f) {
            var_f2 = 0.01f;
        }
        dl = func_global_asm_806AA09C(0x280, 0x33C, D_global_asm_807FC828[D_global_asm_807FC818[D_global_asm_807FC80C]], D_global_asm_8075056C[D_global_asm_807FC818[D_global_asm_807FC80C]], dl, 1, var_f2);
        break;
    }
    gSPEndDisplayList(dl++);
    return dl_copy;
}

void func_global_asm_806AB808(void*, s32, s32, s32, s32, s32, s32, s32);
u8 func_global_asm_806F6E58(s16 arg0);
s32 func_global_asm_806FA7A4(s32 arg0);
s32 func_global_asm_80731A04(s32 arg0, s32 arg1, s32 arg2, s32 arg3);
s32 func_global_asm_806C8DE0(s32 playerIndex);
s32 func_global_asm_807319D8(s32 arg0, s32 arg1, s32 arg2);
extern u8 isFlagSet(s16 flagIndex, u8 flagType);
extern SpriteData *D_global_asm_80750518[];
extern s8 D_global_asm_80750530[];
extern s16 D_global_asm_807505A4[];
extern u16 D_global_asm_807505B8[];
extern s16 D_global_asm_807505CA;
extern s8 D_global_asm_807505D0;
extern SpriteData *D_global_asm_807505D8[];
extern SpriteData D_global_asm_80721518;
extern SpriteData D_global_asm_807211D0;
extern SpriteData D_global_asm_80720C34;
extern SpriteData D_global_asm_80720D80;
extern SpriteData D_global_asm_80720D5C;
extern SpriteData D_global_asm_80721474;
extern SpriteData D_global_asm_807210EC;
extern SpriteData D_global_asm_80721094;
extern SpriteData D_global_asm_80720D38;
extern SpriteData D_global_asm_8072104C;
typedef struct Struct80755DA8 {
    u8 unk0;
    u8 pad1;
    s16 unk2;
    s16 unk4;
    u8 unk6;
    u8 unk7;
} Struct80755DA8;

extern Struct80755DA8 D_global_asm_80755DA8[];
extern s8 D_global_asm_807FC7F8[];
extern s8 D_global_asm_807FC800;
extern f32 D_global_asm_807FC804;
extern s8 D_global_asm_807FC80C;
extern s8 D_global_asm_807FC80D;
extern s16 D_global_asm_807FC82A;
extern s16 D_global_asm_807FC82E;
extern s16 D_global_asm_807FC83A;
extern s8 D_global_asm_807FC83C;
extern u16 D_global_asm_807FCC40;
extern u16 D_global_asm_807FCC44;
extern u16 D_global_asm_807FCC46;
extern u16 D_global_asm_807FCC48;
extern u8 current_character_index[];
extern s8 D_global_asm_807FC80F;
f32 func_global_asm_806119FC(void);
typedef struct Struct80717D84_unk384_806AB4EC {
    f32 unk0;
    f32 unk4;
    f32 unk8;
    f32 unkC;
    f32 unk10;
    f32 unk14;
    f32 unk18;
    f32 unk1C;
    f32 unk20;
    f32 unk24;
} Struct80717D84_unk384_806AB4EC;

extern rgba D_global_asm_8075054C[];
extern f32 D_global_asm_807FC808;

RECOMP_PATCH void func_global_asm_806AC07C(Struct80717D84* arg0, s8* arg1) {
    PauseAAD* temp_t6; // 54
    f32* temp_a2; // 50
    f32 var_f14; // 4C
    f32 var_f2;
    f32 temp_f2_2;
    s32 sp40;
    s32 temp_t2; // 3C
    s32 temp_v0;
    s32 temp_f10;
    s32 var_v0_2;
    f32 temp_f;

    f32 old_x, old_y, dx, dy;

    temp_t6 = (PauseAAD*)arg0->unk35C;
    temp_a2 = (f32*)arg0->unk384;
    if (!temp_a2) {
        return;
    }
    old_x = arg0->unk340;
    old_y = arg0->unk344;
    var_f14 = temp_t6->unk0;
    sp40 = temp_t6->unkC[(s32)temp_a2[1]] * 4.0;
    temp_f10 = temp_a2[0];
    switch (temp_f10) {
        case 0:
        case 4:
        case 11:
        case 12:
            var_f14 = 1.0 - var_f14;
            if (var_f14 < 0.0f) {
                var_f14 = 0.0f;
            }
            if (var_f14 > 1.0f) {
                var_f14 = 1.0f;
            }
            arg0->unk36D = var_f14 * 255.0f;
            if (temp_f10 == 4) {
                arg0->unk360 = (temp_a2[4] * var_f14) * 4.0;
                arg0->unk364 = arg0->unk360;
            }
            var_f14 = 0.0f;
            arg0->unk340 = temp_a2[2];
            arg0->unk344 = temp_a2[3] + sp40;
            if (temp_f10 == 0xB) {
                arg0->unk340 = 600.0 - D_global_asm_807FC808;
            }
            if (temp_f10 == 0xC) {
                arg0->unk340 = D_global_asm_807FC808 + 680.0;
            }
            break;
        case 1:
        case 2:
        case 7:
            if (temp_f10 != 1) {
                var_f14 += temp_t6->unk4;
            }
            if (temp_f10 == 7) {
                arg0->unk364 = temp_a2[6] + (func_global_asm_80612794(((object_timer * 0x78) & 0xFFF)) * 0.35);
            }
            arg0->unk340 = (temp_a2[4] * var_f14) + temp_a2[2];
            arg0->unk344 = (temp_a2[5] * var_f14) + temp_a2[3] + sp40;
            break;
        case 3:
        case 5:
        case 8:
        case 15:
            var_f14 = 1.0 - var_f14;
            if (var_f14 < 0) {
                var_f14 = 0;
            }
            if (var_f14 > 1) {
                var_f14 = 1;
            }
            temp_v0 = (s32) (temp_a2[2] * 0.25) - temp_t6->unk10;
            var_f2 = 180.0f;
            if (temp_f10 != 3) {
                var_f2 = 240.0f;
            }
            var_f2 *= var_f14;
            arg0->unk360 = (temp_a2[4] * var_f14) * 4.0;
            arg0->unk364 = arg0->unk360;
            arg0->unk340 = (func_global_asm_80612794(temp_v0) * var_f2) + 640.0;
            var_f14 = 0;
            arg0->unk344 = (480.0 - (func_global_asm_80612790(temp_v0) * var_f2)) + sp40;
            if (temp_f10 == 8) {
                var_f14 = 0;
                arg0->unk344 += (func_global_asm_80612794((s16) ((object_timer * 0x78) & 0xFFF)) * 10.0f);
            }
            if (temp_f10 == 0xF) {
                var_f14 = 0;
                arg0->unk364 *= (1.0 + (func_global_asm_80612794(((object_timer * 0x78) & 0xFFF)) * 0.2));
            }
            if ((temp_f10 == 5) || (temp_f10 == 8) || (temp_f10 == 0xF)) {
                var_v0_2 = 0xFF - (s32) ((arg0->unk344 - 240.0) * 1.5);
                if (var_v0_2 < 0x5F) {
                    var_v0_2 = 0x5F;
                }
                if (var_v0_2 > 0xFF) {
                    var_v0_2 = 0xFF;
                }
                arg0->unk36A = var_v0_2;
                arg0->unk36B = var_v0_2;
                arg0->unk36C = var_v0_2;
            }
            break;
        case 6:
            if ((SQ(temp_a2[4] - temp_a2[6]) + SQ(temp_a2[5] - temp_a2[7])) < 400.0f) {
                temp_a2[9] -= 1.0;
                if (temp_a2[9] < 0.0) {
                    temp_a2[8] += (1000.0 + (func_global_asm_806119FC() * 2000.0));
                    temp_t2 = (s32) temp_a2[8] & 0xFFF;
                    temp_a2[8] = temp_t2;
                    temp_f2_2 = ((func_global_asm_806119FC() * 40.0) + 20.0) * 4.0;
                    temp_a2[6] = (func_global_asm_80612794(temp_t2) * (temp_f2_2 * 0.8));
                    temp_a2[7] = func_global_asm_80612790(temp_t2) * temp_f2_2;
                    if (func_global_asm_806119FC() < 0.25) {
                        temp_a2[9] = (func_global_asm_806119FC() * 40.0) + 15.0;
                    } else {
                        temp_a2[9] = 0.0f;
                    }
                }
            }
            var_f2 = (temp_a2[6] - temp_a2[4]) * 0.1;
            if (var_f2 < -8.0) {
                var_f2 = -8.0;
            }
            if (var_f2 > 8.0) {
                var_f2 = 8.0;
            }
            temp_a2[4] += var_f2;
            var_f2 = (temp_a2[7] - temp_a2[5]) * 0.1;
            if (var_f2 < -8.0) {
                var_f2 = -8.0;
            }
            if (var_f2 > 8.0) {
                var_f2 = 8.0;
            }
            temp_a2[5] += var_f2;
            arg0->unk340 = temp_a2[4] + temp_a2[2];
            temp_f = func_global_asm_80612794((object_timer * 0x78) & 0xFFF) * 10.0f;
            arg0->unk344 = (temp_f + (temp_a2[3] + temp_a2[5] + sp40 + (960.0f * var_f14)));
            var_f14 = 0;
            break;
        case 10:
            var_v0_2 = (s32) temp_a2[2];
            arg0->unk340 = D_global_asm_807FC840[var_v0_2];
            arg0->unk344 = D_global_asm_807FC868[var_v0_2];
            arg0->unk360 = D_global_asm_807FC890[var_v0_2];
            arg0->unk364 = D_global_asm_807FC890[var_v0_2];
            D_global_asm_807FC840[var_v0_2] = -200.0f;
            D_global_asm_807FC868[var_v0_2] = -200.0f;
            if (var_v0_2 >= 5) {
                var_v0_2 -= 5;
            }
            if (D_global_asm_80750530[var_v0_2] == 0) {
                arg0->unk36A = D_global_asm_8075054C[var_v0_2].red;
                arg0->unk36B = D_global_asm_8075054C[var_v0_2].green;
                arg0->unk36C = D_global_asm_8075054C[var_v0_2].blue;
            }
            break;
    }
    if ((temp_t6->unk15 != (s32) temp_a2[1]) && ((sp40 < -0x3C0) || (sp40 >= 0x3C1))) {
        *arg1 = 1;
    }
    if ((var_f14 > 1.0f) || (temp_t6->unk0 > 1.0f)) {
        *arg1 = 1;
    }
    dx = ABS_F(arg0->unk340 - old_x);
    dy = ABS_F(arg0->unk344 - old_y);
    if ((dx > 80.0f) || (dy > 80.0f)) {
        // Disable interpolation
        arg0->unk36F |= ALIGN_NO_INTERP;
    } else {
        // Re-enable interpolation
        arg0->unk36F &= ~ALIGN_NO_INTERP;
    }
}

// @recomp: Pause menu sprite draw function
RECOMP_PATCH void func_global_asm_806AB4EC(PauseAAD *arg0, SpriteData *arg1, s32 arg2, s32 arg3, f32 arg4, u8 arg5, s32 arg6) {
    Struct80717D84* var_v0;
    Struct80717D84_unk384_806AB4EC* temp_v0;
    f32 temp_f14;
    f32 temp_f16;
    f32 temp_f0_3;
    s32 var_v0_2;
    void* temp_a0;

    func_global_asm_80714998(arg5);
    if (D_global_asm_807FDB1D == 0) {
        func_global_asm_8071495C();
    }
    func_global_asm_807149FC(-1);
    func_global_asm_80714950((s32)arg0);
    func_global_asm_8071498C(&func_global_asm_806AC07C);
    func_global_asm_80714A28(0x101U);
    if ((s32)arg1 < 0) {
        var_v0 = drawSpriteAtPosition(arg1, arg4, arg2, arg3, -10.0f);
    } else {
        temp_a0 = func_global_asm_806FACE8((u32) arg1);
        var_v0 = drawSpriteAtPosition(temp_a0, arg4, arg2, arg3, -10.0f);
    }
    var_v0_2 = 7;
    if (arg6 == 6) {
        var_v0_2 = 0xA;
    }
    temp_v0 = _malloc(var_v0_2 * 4);
    var_v0->unk384 = temp_v0;
    if (arg6 == 9) {
        arg6 = 7;
        var_v0->unk360 = -var_v0->unk360;
    }
    temp_v0->unk0 = arg6;
    temp_v0->unk4 = arg0->unk15;
    temp_v0->unk8 = var_v0->unk340;
    temp_v0->unkC = var_v0->unk344;
    switch (arg6) {
    case 1:
    case 2:
    case 7:
    case 13:
        temp_f14 = 640.0f - temp_v0->unk8;;
        temp_f16 = 480.0f - temp_v0->unkC;
        temp_f0_3 = _sqrtf((temp_f14 * temp_f14) + (temp_f16 * temp_f16));
        temp_v0->unk10 = (f32) (-(temp_f14 / temp_f0_3) * (1040.0f - temp_f0_3));
        temp_v0->unk14 = (f32) (-(temp_f16 / temp_f0_3) * (1040.0f - temp_f0_3));
        temp_v0->unk18 = (f32) var_v0->unk364;
        if (arg6 == 0xD) {
            temp_v0->unk0 = 2.0f;
            var_v0->unk36C = 0;
            var_v0->unk36B = 0;
            var_v0->unk36A = 0xFF;
        }
        break;
    case 6:
        temp_v0->unk10 = 0.0f;
        temp_v0->unk14 = 0.0f;
        temp_v0->unk18 = 0.0f;
        temp_v0->unk1C = 0.0f;
        temp_v0->unk20 = (f32) (func_global_asm_806119FC() * 4096.0f);
        if (arg2 < 0xA0) {
            var_v0->unk360 = -var_v0->unk360;
        }
        temp_v0->unk24 = 0.0f;
        break;
    case 10:
        temp_v0->unk8 = arg2;
        break;
    case 14:
        temp_v0->unk0 = 3.0f;
        var_v0->unk36C = 0;
        var_v0->unk36B = 0;
        var_v0->unk36A = 0xFF;
    default:
        temp_v0->unk10 = arg4;
        temp_v0->unk14 = 0.0f;
        break;
    }
    if (D_global_asm_807FC80F == 0) {
        var_v0->unk36A = 0x64;
        var_v0->unk36B = 0x64;
        var_v0->unk36C = 0x64;
    }
    D_global_asm_807FC80F = 1;
}


// @recomp: Pause Menu Sprite Initialization Wrapper
RECOMP_PATCH void func_global_asm_806AA304(PauseAAD* arg0, s32 arg1) {
    s32 sp8C;
    f32 temp_f6;
    f32 var_f0;
    SpriteData* var_a1;
    s32 sp7C;
    s32 sp74;
    s8 var_v1; // sp6B
    s8 sp64[4];
    s32 sp58;
    CharacterProgress* temp_t0;
    s16 temp_a0;
    s16 temp_t7_2;
    s32 temp_a2;
    s32 temp_v0;
    s32 var_a2;
    s32 var_t0;
    s32 var_t1;
    CharacterProgress* temp_s0;
    s32 i;
    s32 var_v1_2;

    sp8C = (s32) current_character_index[0];
    temp_v0 = func_global_asm_806C8DE0(0);
    current_character_index[0] = arg0->unk14;
    D_global_asm_807FC7F8[arg0->unk15] = 0;
    temp_t7_2 = arg0->unk8[arg0->unk15];
    temp_s0 = &D_global_asm_807FC950->character_progress[temp_v0];
    temp_t0 = &D_global_asm_807FC950->character_progress[arg0->unk14];
    sp7C = D_global_asm_80750560[temp_t7_2];
    var_v1 = 0;
    set_sprite_interpolation_lockdown(2);
    if (arg1 == 0) {
        D_global_asm_807FC800 = arg0->unk14;
        D_global_asm_807FC80D = D_global_asm_807FC80C;
        arg0->unk10 = 0;
        arg0->unk4 = 0.0f;
        D_global_asm_807FC804 = 0.0f;
    }
    D_global_asm_807FC80F = 1;
    switch (arg0->unk12) {
        case 0:
            current_character_index[0] = sp8C;
            sp7C = getLevelIndex(D_global_asm_8076A0AB, 1U);
            if (sp7C >= 9) {
                sp7C = 7;
            }
            if ((sp7C != 7) && (isFlagSet(0x18B, 0U) != 0)) {
                func_global_asm_806AB808(arg0, 0x1E, 0x1E, 0xC, -0xA, temp_s0->coloured_bananas[sp7C], 1, 0);
                setSpriteAlignment(ALIGN_LEFT);
                func_global_asm_806AB4EC(arg0, 0, 0x1E, 0x1E, 0.75f, 2, 1);
            }
            if (isFlagSet(0x184, 0U) != 0) {
                func_global_asm_806AB808(arg0, 0x1E, 0x5A, 0xC, -0xA, D_global_asm_807FCC44, 1, 1);
                setSpriteAlignment(ALIGN_LEFT);
                func_global_asm_806AB4EC(arg0, (SpriteData* )4, 0x1E, 0x5A, 0.75f, 2, 1);
            }
            if (temp_s0->weapon != 0) {
                func_global_asm_806AB808(arg0, 0x1E, 0x7D, 0xC, -0xA, D_global_asm_807FCC40, 1, 1);
                setSpriteAlignment(ALIGN_LEFT);
                func_global_asm_806AB4EC(arg0, (SpriteData* )2, 0x1E, 0x7D, 0.65f, 2, 1);
            }
            if (func_global_asm_806F6E58((s16) current_character_index[0]) != 0) {
                temp_f6 = func_global_asm_806FA7A4(5);
                func_global_asm_806AB808(arg0, 0x1E, 0xA0, 0xC, -0xA, func_global_asm_80612E40(D_global_asm_807FC950[0].crystals / temp_f6), 1, 1);
                setSpriteAlignment(ALIGN_LEFT);
                func_global_asm_806AB4EC(arg0, (SpriteData* )5, 0x1E, 0xA0, 0.65f, 2, 1);
            }
            if (isFlagSet(0x18C, 0U)) {
                func_global_asm_806AB808(arg0, 0x122, 0x5A, -0xF, -0xA, temp_s0->coins, 1, 0x80);
                setSpriteAlignment(ALIGN_RIGHT);
                func_global_asm_806AB4EC(arg0, (SpriteData* )1, 0x122, 0x5A, 0.75f, 2, 1);
            }
            if (temp_t0->instrument & 1) {
                func_global_asm_806AB808(arg0, 0x122, 0x7D, -0xC, -0xA, temp_t0->instrument_ammo, 2, 0x81);
                setSpriteAlignment(ALIGN_RIGHT);
                func_global_asm_806AB4EC(arg0, (SpriteData* )7, 0x122, 0x7D, 0.75f, 2, 7);
            }
            if (isFlagSet(0x179, 0U)) {
                func_global_asm_806AB808(arg0, 0x122, 0xA0, -0xC, -0xA, D_global_asm_807FCC48, 1, 0x81);
                setSpriteAlignment(ALIGN_RIGHT);
                func_global_asm_806AB4EC(arg0, (SpriteData* )6, 0x122, 0xA0, 0.65f, 2, 1);
            }
            if (D_global_asm_807FC828[0] > 0) {
                func_global_asm_806AB808(arg0, 0x91, 0xBE, 0xF, -0xA, D_global_asm_807FC828[0], 1, 0);
                func_global_asm_806AB4EC(arg0, (SpriteData* )9, 0x91, 0xBE, 1.0f, 2, 1);
            }
            current_character_index[0] = arg0->unk14;
            break;
        case 1:
            if (sp7C != 7) {
                i = sp7C;
                if (sp7C != 8) {
                    func_global_asm_806AB808(arg0, 0x1E, 0x2A, 0xC, -0xA, temp_t0->coloured_bananas[sp7C] + temp_t0->coloured_bananas_fed_to_tns[sp7C], 2, 0);
                    setSpriteAlignment(ALIGN_LEFT);
                    func_global_asm_806AB4EC(arg0, 0, 0x1E, 0x2A, 0.75f, 2, 2);
                } else {
                    i = 7;
                }
                if (D_global_asm_807FC82E > 0) {
                    if (sp7C != 8) {
                        D_global_asm_807FC80F = (D_global_asm_807FC950[0].character_progress[current_character_index[0]].coloured_bananas[sp7C] + D_global_asm_807FC950[0].character_progress[current_character_index[0]].coloured_bananas_fed_to_tns[sp7C]) >= 0x4B;
                    } else {
                        D_global_asm_807FC80F = func_global_asm_80731A04(0x225, i, i, (s32) current_character_index[0]);
                    }
                    setSpriteAlignment(ALIGN_RIGHT);
                    func_global_asm_806AB4EC(arg0, (SpriteData* )0xA, 0x113, 0x7D, 0.75f, 2, 2);
                    if (func_global_asm_80731A04(0x225, i, i, (s32) current_character_index[0]) != 0) {
                        setSpriteAlignment(ALIGN_RIGHT);
                        func_global_asm_806AB4EC(arg0, &D_global_asm_80721518, 0x113, 0x7D, 0.35f, 2, 0xD);
                    }
                }
            }
            if (arg1 == 0) {
                for (i = 0; i < 5; i++) {
                    if (D_global_asm_80750530[i] != 0) {
                        var_a1 = D_global_asm_80750518[i];
                    } else {
                        var_a1 = &D_global_asm_807211D0;
                    }
                    func_global_asm_806AB4EC(arg0, var_a1, i, i, 0.75f, 2, 0xA);
                    temp_a2 = i + 5;
                    func_global_asm_806AB4EC(arg0, var_a1, temp_a2, temp_a2, 0.75f, 2, 0xA);
                }
            }
            if (sp7C != 8) {
                func_global_asm_806AB4EC(arg0, (SpriteData* )9, 0x91, 0xCB, 1.0f, 2, 2);
                var_t0 = FALSE;
                for (i = 0; i < 5; i++) {
                    if (func_global_asm_80731A04(0x1D5, 0, 7, i)) {
                        var_t0 = TRUE;
                    }
                }
                for (i = 0; i < 5; i++); // Used for getting the above loop to match
                if (var_t0) {
                    D_global_asm_807FC80F = isFlagSet(func_global_asm_807319D8(0x1D5, sp7C, (s32) current_character_index[0]), 0U);
                    setSpriteAlignment(ALIGN_LEFT);
                    func_global_asm_806AB4EC(arg0, (SpriteData* )0xC, 0x26, 0x7D, 0.75f, 2, 2);
                    if (isFlagSet(func_global_asm_807319D8(0x1FD, sp7C, (s32) current_character_index[0]), 0U) != 0) {
                        setSpriteAlignment(ALIGN_LEFT);
                        func_global_asm_806AB4EC(arg0, &D_global_asm_80721518, 0x26, 0x7D, 0.35f, 2, 0xD);
                    }
                }
            }
            if (arg1 == 0) {
                arg0->unk10 = D_global_asm_807505A4[arg0->unk14];
                func_global_asm_806AB4EC(arg0, &D_global_asm_80720C34, 0xA0, 0x78, 0.75f, 2, 0);
                if (D_global_asm_807505D0 > 0) {
                    func_global_asm_806AB4EC(arg0, &D_global_asm_80720D80, 0xA0, 0x19, 0.5f, 2, 0xB);
                    func_global_asm_806AB4EC(arg0, &D_global_asm_80720D5C, 0xA0, 0x19, 0.5f, 2, 0xC);
                }
            }
            break;
        case 2:
            var_t0 = 0;
            var_v1_2 = 0;
            for (i = 0; i < 5; i++) {
                var_t0 += D_global_asm_807FC950[0].character_progress[i].coloured_bananas[sp7C];
                var_t0 += D_global_asm_807FC950[0].character_progress[i].coloured_bananas_fed_to_tns[sp7C];
                var_v1_2 += D_global_asm_807FC950[0].character_progress[i].golden_bananas[sp7C];
            }
            var_v1 = 0;
            for (i = 0; i < 4; i++) {
                sp64[i] = 0;
            }
            for (i = 0; D_global_asm_80755DA8[i].unk0 != 0xFF; i++) {
                if (sp7C == getLevelIndex(D_global_asm_80755DA8[i].unk0, 1U)) {
                    temp_a0 = D_global_asm_80755DA8[i].unk4;
                    if ((temp_a0 >= 0x24D) && (temp_a0 < 0x261) && (var_v1 < 4)) {
                        sp64[var_v1++] = isFlagSet(temp_a0, 0U);
                    }
                }
            }
            if (sp7C != 7) {
                sp58 = sp7C;
                i = 0;
                if (sp7C != 8) {
                    func_global_asm_806AB808(arg0, 0x24, 0x2A, 0xC, -0xA, var_t0, 1, 0);
                    setSpriteAlignment(ALIGN_LEFT);
                    func_global_asm_806AB4EC(arg0, &D_global_asm_80721474, 0x24, 0x2A, 0.75f, 2, 1);
                } else {
                    sp58 = 7;
                }
                for (i = 0; i < 5; i++) {
                    if (sp7C != 8) {
                        D_global_asm_807FC80F = (D_global_asm_807FC950[0].character_progress[i].coloured_bananas_fed_to_tns[sp7C] + D_global_asm_807FC950[0].character_progress[i].coloured_bananas[sp7C]) >= 0x4B;
                    } else {
                        D_global_asm_807FC80F = func_global_asm_80731A04(0x225, sp58, sp58, i);
                    }
                    func_global_asm_806AB4EC(arg0, (SpriteData* )0xA, (s32) D_global_asm_807505A4[i], 0, 0.75f, 2, 3);
                    if (func_global_asm_80731A04(0x225, sp58, sp58, i) != 0) {
                        func_global_asm_806AB4EC(arg0, &D_global_asm_80721518, (s32) D_global_asm_807505A4[i], 0, 0.35f, 2, 0xE);
                    }
                }
                if (D_global_asm_807FC82A > 0) {
                    D_global_asm_807FC80F = isFlagSet(D_global_asm_807505B8[sp7C], 0U);
                    func_global_asm_806AB4EC(arg0, &D_global_asm_807210EC, 0xA0, 0x78, 1.0f, 2, 4);
                }
                if (isFlagSet(0x179, 0U) != 0) {
                    D_global_asm_807FC80F = sp64[0];
                    setSpriteAlignment(ALIGN_LEFT);
                    func_global_asm_806AB4EC(arg0, &D_global_asm_80721094, 0x3C, 0x78, 1.0f, 2, 6);
                    D_global_asm_807FC80F = sp64[1];
                    setSpriteAlignment(ALIGN_RIGHT);
                    func_global_asm_806AB4EC(arg0, &D_global_asm_80721094, 0x104, 0x78, 1.0f, 2, 6);
                }
            } else {
                if (D_global_asm_807FC82A > 0) {
                    D_global_asm_807FC80F = isFlagSet(D_global_asm_807505B8[sp7C], 0U);
                    func_global_asm_806AB4EC(arg0, &D_global_asm_807210EC, 0xA0, 0x55, 1.0f, 2, 4);
                    D_global_asm_807FC80F = isFlagSet(D_global_asm_807505CA, 0U);
                    func_global_asm_806AB4EC(arg0, &D_global_asm_807210EC, 0xA0, 0x91, 1.0f, 2, 4);
                }
                if (isFlagSet(0x179, 0U)) {
                    D_global_asm_807FC80F = sp64[0];
                    setSpriteAlignment(ALIGN_LEFT);
                    func_global_asm_806AB4EC(arg0, &D_global_asm_80721094, 0x3C, 0x5A, 1.0f, 2, 6);
                    D_global_asm_807FC80F = sp64[1];
                    setSpriteAlignment(ALIGN_LEFT);
                    func_global_asm_806AB4EC(arg0, &D_global_asm_80721094, 0x46, 0x96, 1.0f, 2, 6);
                    D_global_asm_807FC80F = sp64[2];
                    setSpriteAlignment(ALIGN_RIGHT);
                    func_global_asm_806AB4EC(arg0, &D_global_asm_80721094, 0x104, 0x5A, 1.0f, 2, 6);
                    D_global_asm_807FC80F = sp64[3];
                    setSpriteAlignment(ALIGN_RIGHT);
                    func_global_asm_806AB4EC(arg0, &D_global_asm_80721094, 0xFA, 0x96, 1.0f, 2, 6);
                }
            }
            if (sp7C != 8) {
                func_global_asm_806AB4EC(arg0, (SpriteData* )9, 0x91, 0xCB, 1.0f, 2, 2);
            }
            if ((arg1 == 0) && (D_global_asm_807505D0 > 0)) {
                func_global_asm_806AB4EC(arg0, &D_global_asm_80720D80, 0xA0, 0x19, 0.5f, 2, 0xB);
                func_global_asm_806AB4EC(arg0, &D_global_asm_80720D5C, 0xA0, 0x19, 0.5f, 2, 0xC);
            }
            break;
        case 3:
            D_global_asm_807FC83C = 0;
            for (i = 0; i < 8; i++) {
                if (D_global_asm_807FC828[i] > 0) {
                    D_global_asm_807FC83C++;
                }
            }
            D_global_asm_807FC83A = (s16) (0x1000 / (s8) D_global_asm_807FC83C);
            var_a2 = 0;
            var_t1 = 0;
            for (i = 0; i < 8; i++) {
                if (D_global_asm_807FC828[i] > 0) {
                    if (i != 5) {
                        var_f0 = 0.75f;
                        var_t0 = 5;
                    } else {
                        var_f0 = 1.0f;
                        var_t0 = 8;
                    }
                    func_global_asm_806AB4EC(arg0, D_global_asm_807505D8[i], var_a2, 0, var_f0, 2, var_t0);
                    D_global_asm_807FC818[var_t1] = i;
                    var_t1 += 1;
                    var_a2 += D_global_asm_807FC83A;
                }
            }
            arg0->unk10 = (s16) (D_global_asm_807FC83A * D_global_asm_807FC80C);
            func_global_asm_806AB4EC(arg0, &D_global_asm_80720C34, 0xA0, 0x78, 0.75f, 2, 0);
            break;
    }
    if ((arg1 == 0) && (D_global_asm_807FC828[0] > 0)) {
        setSpriteAlignment(ALIGN_LEFT);
        func_global_asm_806AB4EC(arg0, &D_global_asm_80720D38, 0x2D, 0xC8, 1.0f, 2, 1);
        setSpriteAlignment(ALIGN_RIGHT);
        func_global_asm_806AB4EC(arg0, &D_global_asm_8072104C, 0x113, 0xC8, 1.0f, 2, 1);
    }
    current_character_index[0] = sp8C;
}

typedef struct Struct805FD088 {
    Mtx unk0;
    Mtx unk40;
    Mtx unk80;
    Mtx unkC0;
    Mtx unk100;
    Mtx unk140;
    Mtx unk180;
    Mtx unk1C0;
    Mtx unk200;
    u8 pad240[0xDB0 - 0x240];
    Gfx dl[0x80];
} Struct805FD088;

Gfx *func_global_asm_8062BF24(Gfx *, Struct805FD088 *);
Gfx *func_global_asm_8068C20C(Gfx *, u8);
Struct80750948 *func_global_asm_806C7C94(u8);
Gfx *func_global_asm_80701CA0(Gfx *);
Gfx *func_global_asm_80714060(Gfx *);
Gfx *func_global_asm_806FAB20(Gfx *);
Gfx *func_global_asm_80709344(Gfx *);
Gfx *func_global_asm_805FCFD8(Gfx *);
Gfx *func_global_asm_805FE398(Gfx *);
Gfx *func_global_asm_805FE4D4(Gfx *);
Gfx *func_global_asm_80704960(Gfx *);
Gfx *func_global_asm_807007B8(Gfx *);
Gfx *func_global_asm_80629300(Gfx *);
Gfx *func_global_asm_8062A844(Gfx *);
Gfx *func_global_asm_8062CA0C(Gfx *dl, f32 arg1, f32 arg2, f32 arg3);
void func_global_asm_80722280(void);
void func_global_asm_8068C0B4(void);
void func_global_asm_806FBC34(void);
f32 func_global_asm_8062A850(void);
int func_global_asm_80714464(void);

extern u8 D_global_asm_8074447C;
extern u8 D_global_asm_80744480;
extern u8 D_global_asm_80744484;
extern u8 D_global_asm_80744488;
extern u8 D_global_asm_8074448C;
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
extern f32 D_global_asm_807444B8;
extern f32 D_global_asm_807444BC;
extern f32 D_global_asm_807444C4;
extern f32 D_global_asm_807444C8;
extern u8 D_global_asm_807467E4;
extern u8 D_global_asm_80750AB8;
extern void *D_global_asm_8076A060;
extern u16 D_global_asm_8076A09C;
extern u8 D_global_asm_8076A0B1;
extern u8 D_global_asm_8076A0B3;
extern Gfx* D_global_asm_8076A050[];
extern u8 D_global_asm_807444FC;
extern void *D_global_asm_8076A080;
extern Gfx **D_1000090;
extern void set_sprite_interpolation_state(void);

// @recomp: General gfx loop
RECOMP_PATCH void func_global_asm_805FD088(Struct805FD088 *arg0, Gfx **arg1, Gfx **arg2) {
    Gfx *dl; // 114
    Gfx *dl_0;

    dl = D_global_asm_8076A050[D_global_asm_807444FC];
    dl_0 = &arg0->dl[0];
    gSPPerspNormalize(dl_0++, D_global_asm_8076A09C);
    gSPClipRatio(dl_0++, FRUSTRATIO_3);
    gSPSegment(dl++, 0x00, 0x00000000);
    gSPSegment(dl++, 0x02, osVirtualToPhysical(arg0));
    gSPSegment(dl++, 0x01, osVirtualToPhysical(D_global_asm_8076A080));
    gSPSegment(dl_0++, 0x00, 0x00000000);
    gSPSegment(dl_0++, 0x02, osVirtualToPhysical(arg0));
    gSPSegment(dl_0++, 0x01, osVirtualToPhysical(D_global_asm_8076A080));
    gSPDisplayList(dl++, &D_1000090);
    gSPDisplayList(dl_0++, &D_1000090);
    {
        f32 temp_f0;
        f32 temp0;
        f32 temp_f2;
        u32 vf;
        s16 n, d;
        Struct80750948 *temp_v0_6;
        s16 x, y;
        s32 width, height;
        set_sprite_interpolation_state();
        dl = func_global_asm_805FCFD8(dl);
        dl_0 = func_global_asm_805FCFD8(dl_0);
        gDPSetScissorFrac(dl++, G_SC_NON_INTERLACE, 0, 0, D_global_asm_80744490 * 4.0f, D_global_asm_80744494 * 4.0f);
        gDPSetScissorFrac(dl_0++, G_SC_NON_INTERLACE, 0, 0, D_global_asm_80744490 * 4.0f, D_global_asm_80744494 * 4.0f);
        guTranslate(&arg0->unk180, 0.0f, 0.0f, 0.0f);
        guOrtho(&arg0->unk80, 0.0f, (f32) D_global_asm_80744490 - 1.0, (f32) D_global_asm_80744494 - 1.0, 0.0f, -20000.0f, 20000.0f, 1.0f);
        guOrtho(&arg0->unkC0, 0.0f, (f32) D_global_asm_80744490 * 4.0 - 1.0, (f32) D_global_asm_80744494 * 4.0 - 1.0, 0.0f, -20000.0f, 20000.0f, 1.0f);
        guOrtho(&arg0->unk140, 0.0f, 2.0 * (f32) D_global_asm_80744490 - 1.0, 2.0 * (f32) D_global_asm_80744494 - 1.0, 0.0f, -20000.0f, 20000.0f, 1.0f);
        guOrtho(&arg0->unk100, (f32) character_change_array->unk270[0] * 4.0, (f32) character_change_array->unk270[2] * 4.0 - 1.0, (f32) character_change_array->unk270[3] * 4.0 - 1.0, (f32) character_change_array->unk270[1] * 4.0, -20000.0f, 20000.0f, 1.0f);
        temp0 = func_global_asm_8062A850();
        guPerspective(&arg0->unk0, &D_global_asm_8076A09C, D_global_asm_807444B8, temp0 * D_global_asm_807444BC, D_global_asm_807444C8, D_global_asm_807444C4, 1.0f);
        gSPMatrix(dl_0++, &D_2000000, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_PROJECTION);
        gSPMatrix(dl_0++, &D_2000200, G_MTX_NOPUSH | G_MTX_MUL | G_MTX_PROJECTION);
        gSPMatrix(dl_0++, &D_2000180, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        dl_0 = func_global_asm_805FD030(dl_0);
        dl = func_global_asm_805FE398(dl);
        gDPPipeSync(dl++);
        dl = func_global_asm_805FE4D4(dl);
        gDPSetColorDither(dl++, G_CD_MAGICSQ);
        gDPSetAlphaDither(dl++, G_AD_PATTERN);
        dl = func_global_asm_80704960(dl);
        gSPClipRatio(dl++, FRUSTRATIO_3);
        gDPPipeSync(dl++);
        gDPSetCycleType(dl++, G_CYC_1CYCLE);
        gSPClearGeometryMode(dl++, G_ZBUFFER | G_SHADE | G_CULL_BOTH | G_FOG | G_LIGHTING | G_TEXTURE_GEN | G_TEXTURE_GEN_LINEAR | G_LOD | G_SHADING_SMOOTH | G_CLIPPING | 0x0040F9FA)
        gSPSetGeometryMode(dl++, G_SHADE | G_CULL_BACK | G_SHADING_SMOOTH);
        dl = func_global_asm_8062BF24(dl, arg0);
        dl = func_global_asm_80701CA0(dl);
        if (cc_number_of_players == 1) {
            dl = func_global_asm_807007B8(dl);
        }
        dl = func_global_asm_8068C20C(dl, 1);
        dl = func_global_asm_80629300(dl);
        dl = func_global_asm_8062A844(dl);
        if ((current_map == MAP_DK_RAP) || (current_map == MAP_MAIN_MENU) || ((func_global_asm_80714464() != 0) && (current_map != MAP_BLOOPERS_ENDING))) {
            D_global_asm_8076A0B1 &= 0xFFEF;
            D_global_asm_8076A0B3 = 0;
        }
        if (((D_global_asm_8076A0B1 & 0x10) || (D_global_asm_8076A0B3 != 0)) && ((cc_number_of_players == 1) || (D_global_asm_80750AB8 == 1))) {
            gDPPipeSync(dl++);
            gDPSetCycleType(dl++, G_CYC_FILL);
            gDPSetRenderMode(dl++, G_RM_NOOP, G_RM_NOOP2);
            gSPClearGeometryMode(dl++, G_ZBUFFER);
            gDPSetFillColor(dl++, 0x00010001);
            gDPSetScissorFrac(dl++, G_SC_NON_INTERLACE,
                D_global_asm_80744498 * 4.0f,
                D_global_asm_8074449C * 4.0f,
                D_global_asm_807444A0 * 4.0f,
                D_global_asm_807444A4 * 4.0f);

            if (D_global_asm_8076A0B3) {
                D_global_asm_8076A0B3 -= 0xF;
                temp_f2 = (1.0 - (D_global_asm_8076A0B3 / 255.0));
                temp_f0 = (D_global_asm_807444AC - D_global_asm_807444A8) * temp_f2;
                gDPFillRectangle(dl++,
                    0,
                    MAX(D_global_asm_807444A8 - temp_f0, 0.0f),
                    MAX(D_global_asm_80744490, 0),
                    MAX(D_global_asm_807444AC - temp_f0, 0.0f));
                gDPFillRectangle(dl++, 0,
                    D_global_asm_807444B0 + temp_f0,
                    D_global_asm_80744490,
                    D_global_asm_807444B4 + temp_f0);
                character_change_array->unk270[1] = D_global_asm_807444AC - temp_f0;
                character_change_array->unk270[3] = D_global_asm_807444B0 + temp_f0;
            } else {
                gDPFillRectangle(dl++, 0, D_global_asm_807444A8, D_global_asm_80744490, D_global_asm_807444AC);
                gDPFillRectangle(dl++, 0, D_global_asm_807444B0, D_global_asm_80744490, D_global_asm_807444B4);
                character_change_array->unk270[1] = D_global_asm_807444AC;
                character_change_array->unk270[3] = D_global_asm_807444B0;
            }
            character_change_array->unk27A = character_change_array->unk270[3] - character_change_array->unk270[1];
            character_change_array->unk280 = (f32) character_change_array->unk278 / (f32) character_change_array->unk27A;
            gDPPipeSync(dl++);
            gDPSetRenderMode(dl++, G_RM_OPA_SURF, G_RM_OPA_SURF2);
        }
        if ((D_global_asm_8074447C) || (D_global_asm_80744480) || (D_global_asm_80744484) || (D_global_asm_80744488) || (D_global_asm_8074448C)) {
            temp_v0_6 = func_global_asm_806C7C94(0);
            character_change_array->unk270[0] = temp_v0_6->unk4 + D_global_asm_8074447C;
            character_change_array->unk270[2] = temp_v0_6->unk8 - D_global_asm_80744480;
            character_change_array->unk270[1] = temp_v0_6->unk6 + D_global_asm_80744484;
            character_change_array->unk270[3] = temp_v0_6->unkA - D_global_asm_80744488;
            gDPPipeSync(dl++);
            gDPSetCycleType(dl++, G_CYC_FILL);
            gDPSetRenderMode(dl++, G_RM_NOOP, G_RM_NOOP2);
            gSPClearGeometryMode(dl++, G_ZBUFFER);
            gDPSetFillColor(dl++, 0x00010001);
            gDPSetScissorFrac(dl++,
                G_SC_NON_INTERLACE,
                D_global_asm_80744498 * 4.0f,
                D_global_asm_8074449C * 4.0f,
                D_global_asm_807444A0 * 4.0f,
                D_global_asm_807444A4 * 4.0f);
            // @recomp: Change bounds. Don't use gEX as we don't want this to respect the UI setting
            recomp_get_ui_bounds(&width, &height);
            if (D_global_asm_8074447C) {
                dl = alignHUDGameplay(dl, ALIGN_LEFT);
                gDPFillRectangle(dl++,
                    temp_v0_6->unk4,
                    temp_v0_6->unk6,
                    character_change_array->unk270[0],
                    temp_v0_6->unkA);
                dl = popHUD(dl, ALIGN_LEFT);
            }
            if (D_global_asm_80744480) {
                dl = alignHUDGameplay(dl, ALIGN_RIGHT);
                gDPFillRectangle(dl++,
                    character_change_array->unk270[2],
                    temp_v0_6->unk6,
                    temp_v0_6->unk8,
                    temp_v0_6->unkA);
                dl = popHUD(dl, ALIGN_RIGHT);
            }
            if (D_global_asm_80744484) {
                gDPFillRectangle(dl++,
                    character_change_array->unk270[0],
                    temp_v0_6->unk6,
                    character_change_array->unk270[2],
                    character_change_array->unk270[1]);
            }
            if (D_global_asm_80744488) {
                gDPFillRectangle(dl++,
                    character_change_array->unk270[0],
                    character_change_array->unk270[3],
                    character_change_array->unk270[2],
                    temp_v0_6->unkA);
            }
            gDPPipeSync(dl++);
            character_change_array->unk278 = character_change_array->unk270[2] - character_change_array->unk270[0];
            character_change_array->unk27A = character_change_array->unk270[3] - character_change_array->unk270[1];
            if (
                (!D_global_asm_8074447C) &&
                (!D_global_asm_80744480) &&
                (!D_global_asm_80744484) &&
                (!D_global_asm_80744488)) {
                D_global_asm_8074448C = 0;
            } else {
                D_global_asm_8074448C = 1;
            }
            D_global_asm_8074447C = 0;
            D_global_asm_80744480 = 0;
            D_global_asm_80744484 = 0;
            D_global_asm_80744488 = 0;
            n = temp_v0_6->unk8 - temp_v0_6->unk4;
            d = temp_v0_6->unkA - temp_v0_6->unk6;
            character_change_array->unk280 = (f32)(n) / (f32)(d);
            gDPSetRenderMode(dl++, G_RM_OPA_SURF, G_RM_OPA_SURF2);
        }
        dl = func_global_asm_805FD030(dl);
        dl = func_global_asm_8068C20C(dl, 3);
        dl = func_global_asm_8062CA0C(dl, character_change_array->look_at_eye_x, character_change_array->look_at_eye_y, character_change_array->look_at_eye_z);
        dl = func_global_asm_8068C20C(dl, 5);
        dl = func_global_asm_8068C20C(dl, 7);
        if (D_global_asm_807467E4) {
            D_global_asm_807467E4--;
        }
        dl = func_global_asm_80709344(dl);
        dl = func_global_asm_806FAB20(dl);
        dl = func_global_asm_80714060(dl);
        func_global_asm_806FBC34();
        func_global_asm_8068C0B4();
        func_global_asm_80722280();
        *arg1 = dl;
        *arg2 = dl_0;
    }
}

typedef struct struct80750814 {
    u8 data[4];
} struct80750814;

typedef struct {
    s16 duration;
    s16 cooldown;
    s8 squish_from;
    s8 text_count;
} EndSequenceCardStruct;

extern EndSequenceCardStruct D_global_asm_807506D0[];
extern s8 D_global_asm_80750754;
extern s16 D_global_asm_80750758;

extern f32 D_global_asm_807FC8E8;
extern f32 D_global_asm_807FC8EC;
extern u8 *D_global_asm_807FC8F0;

extern struct80750814 D_global_asm_80750814;
extern f32 D_global_asm_80750818[];
extern f32 D_global_asm_80750828[];
extern void *_malloc(s32);
extern void func_global_asm_8061134C(void *);
extern void func_global_asm_805FEE84(u8 arg0, u8 arg1, u8 arg2, u8 arg3);

// @recomp: Render squish text, do some management
RECOMP_PATCH Gfx *func_global_asm_806C75A4(Gfx *dl, Actor *arg1) {
    s16 i;
    s16 y;
    u8 *str;
    f32 var_f12;
    f32 var_f2;
    f32 var_f0;
    f32 sp108;
    struct80750814 sp104;
    f32 spC4[4][4];
    f32 sp84[4][4];
    Mtx *temp_v0_2;
    s16 var_v1;
    s16 temp_a2;
    s32 width, height;

    sp104 = D_global_asm_80750814;
    if (D_global_asm_80750754 < 0) {
        return dl;
    }
    // @recomp: Amend these figures to account for (lack of) overscan
    D_global_asm_80750818[0] = 120.0f;
    D_global_asm_80750818[1] = 160.0f;
    D_global_asm_80750818[2] = 120.0f;
    D_global_asm_80750818[3] = 160.0f;
    D_global_asm_80750828[1] = 60.0f;
    D_global_asm_80750828[3] = 60.0f;
    i = D_global_asm_807506D0[D_global_asm_80750754].squish_from;

    var_v1 = (((D_global_asm_807FC8E8 - 0.5) * D_global_asm_80750818[i]) + (D_global_asm_80750828[i] * D_global_asm_807FC8EC));
    if (var_v1 < 0) {
        var_v1 = 0;
    }
    sp104.data[i] = var_v1;
    func_global_asm_805FEE84(sp104.data[1], sp104.data[3], sp104.data[0], sp104.data[2]);

    if (D_global_asm_807FC8E8 == 0.0f) {
        return dl;
    }
    temp_v0_2 = _malloc(sizeof(Mtx));
    func_global_asm_8061134C(temp_v0_2);
    gDPPipeSync(dl++);
    gSPDisplayList(dl++, &D_1000118);
    gDPSetCycleType(dl++, G_CYC_1CYCLE);
    gDPSetRenderMode(dl++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
    gDPSetPrimColor(dl++, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF);
    gDPSetCombineMode(dl++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
    gSPMatrix(dl++, &D_20000C0, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_PROJECTION);
    gSPMatrix(dl++, temp_v0_2, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    var_f2 = 1.0f;
    var_f12 = 1.0f;
    recomp_get_ui_bounds(&width, &height);

    if (i & 1) {
        // left/right
        var_f2 = D_global_asm_807FC8EC;
    } else {
        // top/bottom
        var_f12 = D_global_asm_807FC8EC;
    }
    guScaleF(spC4, var_f2 * 0.55, var_f12 * 0.55, 1.f);
    switch (i) {
        case 0:
            // Top
            var_f0 = 160.0f;
            sp108 = (D_global_asm_80750818[i] * D_global_asm_807FC8E8) + -55.0;
            break;
        case 2:
            // Bottom
            var_f0 = 160.0f;
            sp108 = 265.0 - (D_global_asm_80750818[i] * D_global_asm_807FC8E8);
            break;
        case 1:
            // Left
            var_f0 = (D_global_asm_80750818[i] * D_global_asm_807FC8E8) + -70.0;
            var_f0 -= 12.0f; // @recomp: Move this 20 units to the left (lack of overscan, shortened borders) (was +8)
            var_f0 -= ((width - 320) >> 1);
            sp108 = 120.0f;
            break;
        case 3:
            // Right
            var_f0 = 390.0 - (D_global_asm_80750818[i] * D_global_asm_807FC8E8);
            var_f0 += 12.0f; // @recomp: Move this 20 units to the right (lack of overscan, shortened borders) (was -8)
            var_f0 += ((width - 320) >> 1);
            sp108 = 120.0f;
            break;
    }
    guTranslateF(sp84, var_f0 * 4.0, sp108 * 4.0, 0.0f);
    guMtxCatF(spC4, sp84, spC4);
    guMtxF2L(spC4, temp_v0_2);
    str = D_global_asm_807FC8F0;
    for (i = 0; i < D_global_asm_80750758; str++) {
        if (*str == '\0') {
            i++;
        }
    }
    y = -D_global_asm_807506D0[D_global_asm_80750754].text_count * 35;
    for (i = 0; i < D_global_asm_807506D0[D_global_asm_80750754].text_count; str++, i++) {
        temp_a2 = getCenterOfString(1, str) * 2;
        dl = printStyledText(dl, 1, -temp_a2, y, str, 0);
        y += 0x60;
        while (*str) {
            str++;
        }
    }
    gSPPopMatrix(dl++, G_MTX_MODELVIEW);
    gDPPipeSync(dl++);
    return dl;
}

typedef struct {
    s32 unk0;
    s32 unk4;
    s32 unk8;
    s32 unkC;
    s32 unk10;
    s32 unk14;
} Struct8068DBA4_arg1;

// @recomp: Number Game Timer
RECOMP_PATCH Gfx *func_global_asm_8068DBA4(Gfx *dl, Struct8068DBA4_arg1 *arg1) {
    u8 sp34[12];

    gSPDisplayList(dl++, &D_1000118);
    gDPSetCombineMode(dl++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
    gDPSetPrimColor(dl++, 0, 0, 0xFF, 0xFF, 0xFF, 0x96);

    _sprintf(sp34, "%d", arg1->unk14);
    dl = alignHUD(dl, ALIGN_RIGHT);
    dl = printStyledText(dl, 3, 0x370, 0x50, sp34, 1);
    dl = popHUD(dl, ALIGN_RIGHT);
    return dl;
}

// @recomp: Draw Cannon Game lens HUD
RECOMP_PATCH Gfx *func_global_asm_806FEF7C(Gfx *dl, Actor *arg1) {
    dl = func_global_asm_806FEDB0(dl, arg1->PaaD->unk1A4);
    gDPSetRenderMode(dl++, G_RM_CLD_SURF, G_RM_CLD_SURF2);
    gDPSetPrimColor(dl++, 0, 0, 0x00, 0x00, 0x00, 0xFF);

    s32 width, height;
    recomp_get_ui_bounds(&width, &height);

    return displayImage(dl, 0x3C, 3, 1, 0x40, 0x40, 0xA0, 0x78, (5.0f * width) / 320.0f + 1.0f, (5.0f * width) / 320.0f + 1.0f, 0, 0.001f);
}

// @recomp: Cannon game shots remaining HUD
RECOMP_PATCH Gfx *func_global_asm_8068DAF4(Gfx *dl, u8 *arg1) {
    u8 sp38[8];

    gSPDisplayList(dl++, &D_1000118);
    gDPSetCombineMode(dl++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
    gDPSetPrimColor(dl++, 0, 0, 0xFF, 0xFF, 0xFF, 0x96);

    _sprintf(sp38, "%d", *arg1);
    dl = alignHUD(dl, ALIGN_LEFT);
    dl = printStyledText(dl, 3, 260, 80, sp38, 1);
    dl = popHUD(dl, ALIGN_LEFT);
    return dl;
}