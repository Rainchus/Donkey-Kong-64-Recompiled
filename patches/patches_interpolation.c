#include "common_structs.h"
#include "enums.h"
#include "debug_config.h"
#include "misc_funcs.h"
#include "options.h"
#include "patches_main.h"
#include "patches_interpolation.h"

#define INTERPOLATION_DEBUG 0

RECOMP_DECLARE_EVENT(recomp_on_cutscene_play(s16 *cutscene, u8 *cutscene_bitfield));
RECOMP_DECLARE_EVENT(recomp_on_autowalk());

s32 interpolation_disable_timer = 0;
s32 persp_interpolation_disable_timer = 0;
s32 sprite_disable_timer = 0;
u8 skip_interpolation = FALSE;
u8 skip_persp_interp = FALSE;
u8 disable_sprite_interpolation = FALSE;
#if INTERPOLATION_DEBUG
    s32 debug_counter = 0;
#endif

void lockdown_handler(s32 value, s32 *addr) {
    if (*addr > value) {
        return;
    }
    *addr = value;
}

void set_interpolation_lockdown(s32 value) {
    lockdown_handler(value, &interpolation_disable_timer);
}

void set_persp_interpolation_lockdown(s32 value) {
    lockdown_handler(value, &persp_interpolation_disable_timer);
}

void set_sprite_interpolation_lockdown(s32 value) {
    lockdown_handler(value, &sprite_disable_timer);
}

void skipInterpHandler(s32 *ticker_addr, u8 *boolean_addr, u8 decrement) {
    *boolean_addr = FALSE;
    if (*ticker_addr > 0) {
        *boolean_addr = TRUE;
        if (decrement) {
            *ticker_addr = *ticker_addr - 1;
        }
    }
}

void set_sprite_interpolation_state(void) {
    disable_sprite_interpolation = FALSE;
    if (sprite_disable_timer > 0) {
        sprite_disable_timer--;
        disable_sprite_interpolation = TRUE;
    }
}

Gfx *handle_interpolation(Gfx * dl, interpolationIDs id, u8 decrement) {
    skipInterpHandler(&interpolation_disable_timer, &skip_interpolation, decrement);
    skipInterpHandler(&persp_interpolation_disable_timer, &skip_persp_interp, decrement);
    if ((is_cutscene_active == 3) || (is_cutscene_active == 4)) {
        // Skip interpolation for Arcade/Jetpac
        skip_interpolation = TRUE;
        skip_persp_interp = TRUE;
    }
    if (id != 0) {
        if (skip_interpolation || skip_persp_interp) {
            gEXMatrixGroupSkipAllAspect(dl++, id, G_EX_NOPUSH, G_MTX_PROJECTION, G_EX_EDIT_NONE, G_EX_ASPECT_AUTO);
        }
        else {
            gEXMatrixGroup(dl++, id, G_EX_INTERPOLATE_SIMPLE, G_EX_NOPUSH, G_MTX_PROJECTION, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_SKIP, G_EX_COMPONENT_INTERPOLATE, G_EX_ORDER_LINEAR, G_EX_EDIT_NONE, G_EX_ASPECT_AUTO, G_EX_COMPONENT_SKIP, G_EX_COMPONENT_AUTO);
        }
    }
    else if (skip_interpolation) {
        gEXMatrixGroupNoInterpolate(dl++, G_EX_NOPUSH, G_MTX_PROJECTION, G_EX_EDIT_NONE);
    }
    else {
        gEXMatrixGroupSimpleNormal(dl++, G_EX_ID_AUTO, G_EX_NOPUSH, G_MTX_PROJECTION, G_EX_EDIT_NONE);
    }
    return dl;
}

RECOMP_PATCH void func_global_asm_8061DBD4(Actor* arg0, f32* arg1, f32* arg2, f32* arg3, f32* arg4, f32* arg5, f32* arg6, f32* arg7, f32* arg8, f32* arg9) {
    CameraPaad* AAD;
    OSTime temp_v0_9;
    Actor* temp_v0; // 94
    f32 temp_f0;
    f64 var_f0;
    f32 temp_f14_5;
    FuncBank_value *params;
    f32 sp84, sp80;
    s32 var_s1;
    u16 temp_t0;
    f32 temp_f20;

    AAD = arg0->AAD_as_array[0];
    temp_v0 = AAD->unk0;
    D_global_asm_807F5CD0 = (s16) (s32) (temp_v0->unkAC - temp_v0->floor);
    AAD->unk9E = func_global_asm_806CC190(AAD->unk9E, temp_v0->y_rotation, 4.0f);
    if ((temp_v0->object_properties_bitfield & 0x200) && !(AAD->unkAC & 0x100)) {
        getBonePosition(temp_v0, 8, &AAD->unkD8.x, &AAD->unkD8.y, &AAD->unkD8.z);
    } else {
        AAD->unkD8.x = temp_v0->position.f[0];
        AAD->unkD8.y = temp_v0->position.f[1];
        AAD->unkD8.z = temp_v0->position.f[2];
    }
    temp_f0 = _sqrtf(
        SQ(temp_v0->position.f[0] - AAD->unkCC.x) +
        SQ(temp_v0->position.f[1] - AAD->unkCC.y) +
        SQ(temp_v0->position.f[2] - AAD->unkCC.z) 
    );
    AAD->unk10 = (temp_f0 * 40.0f) * (D_global_asm_80744478 * 0.5);
    if (AAD->unkF1 != 0) {
        AAD->unkF1--;
    }
    if (AAD->unkF5 != 0) {
        AAD->unkF5--;
    }
    if (AAD->unkF6 != 0) {
        AAD->unkF6--;
    }
    if (AAD->unkF7 != 0) {
        AAD->unkF7--;
    }
    func_global_asm_8061D060(AAD);
    func_global_asm_8061D1FC(arg0);
    arg0->z_rotation = func_global_asm_806CC190(arg0->z_rotation, temp_v0->z_rotation, 20.0f);
    func_global_asm_8061C0FC(AAD);
    switch (is_cutscene_active) {
        case 0:
            if ((AAD->unkAC & 0x100) == 0) {
                if (loading_zone_transition_speed > 0.0f) {
                    if (!gameIsInDKTVMode()) {
                        return;
                    }
                }
                if (AAD->unkF3 == 9) {
                    return;
                } else if ((temp_v0->control_state == 0x54) && (temp_v0->control_state_progress < 3)) {
                    return;
                } else if ((temp_v0->control_state == 0x52) && (temp_v0->control_state_progress < 8))  {
                    return;
                } else if ((temp_v0->control_state == 0x53) && (temp_v0->control_state_progress < 3))  {
                    return;
                } else {
                    if (temp_v0->control_state == 0x42) {
                        if ((extra_player_info_pointer->unkBC != 0x11) && (extra_player_info_pointer->unkBC != 0x60) && (extra_player_info_pointer->unkBC != 0x49)) {
                            return;
                        }
                    }
                    if (temp_v0->control_state == 0x43) {
                        return;
                    }
                }
            }
            func_global_asm_8061D6A8(AAD);
            if (AAD->unkAC & 0x80000000) {
                func_global_asm_8061C39C(arg0);
            } else {
                func_global_asm_80622B24(arg0, arg0->position.f, &arg0->position.f[1], &arg0->position.f[2], &AAD->unk78, &AAD->unk7C, &AAD->unk80, temp_v0);
                if ((global_properties_bitfield & 0x2000) || (AAD->unkAC & 0x100000)) {
                    if (AAD->unkAC & 0x100000) {
                        temp_f0 = AAD->unk38;
                        AAD->unkAC &= ~0x00100000;
                        arg0->position.f[0] = temp_f0;
                        *arg1 = temp_f0;
                        AAD->unk84 = temp_f0;
                        temp_f0 = AAD->unk3C;
                        arg0->position.f[1] = temp_f0;
                        *arg2 = temp_f0;
                        AAD->unk88 = temp_f0;
                        temp_f0 = AAD->unk40;
                        arg0->position.f[2] = temp_f0;
                        *arg3 = temp_f0;
                        AAD->unk8C = temp_f0;
                        func_global_asm_80602498();
                    } else {
                        *arg1 = arg0->position.f[0];
                        AAD->unk84 = arg0->position.f[0];
                        temp_f14_5 = arg0->position.f[1];
                        *arg2 = temp_f14_5;
                        AAD->unk88 = temp_f14_5;
                        temp_f14_5 = arg0->position.f[2];
                        *arg3 = temp_f14_5;
                        AAD->unk8C = temp_f14_5;
                    }
                    *arg4 = AAD->unk78;
                    *arg5 = AAD->unk7C;
                    *arg6 = AAD->unk80;
                    global_properties_bitfield ^= 0x2000;
                } else {
                    func_global_asm_80625320(arg0, arg1, arg2, arg3, arg4, arg5, arg6);
                }
            }
            break;
        case 1:
            temp_v0_9 = osGetTime();
            D_global_asm_807476C8 = temp_v0_9 - D_global_asm_807476D0;
            D_global_asm_807476D0 = temp_v0_9;
            if (func_global_asm_8061B4B0()) {
                func_global_asm_8061B4E4();
            }
            func_global_asm_8061B7E0(arg0, AAD, *arg1, *arg3);
            if (D_global_asm_807F5CF4 & 0x40) {
                func_global_asm_8061B660(AAD, arg1, arg2, arg3, 0.2f, 0.1f, 50.0f, 40.0f);
            } else {
                if (D_global_asm_807476E4 != 0) {
                    D_global_asm_807476E4--;
                    arg0->x_rotation = D_global_asm_807476E0 + ((f32) (D_global_asm_807476DC - D_global_asm_807476E0) * ((f32) D_global_asm_807476E4 / (f32) D_global_asm_807476E8));
                }
                D_global_asm_807476F0++;
                do {
                    var_s1 = 0;
                    if (D_global_asm_807F5CEC == 0) {
                        func_global_asm_8061D898();
                        D_global_asm_807F5CF0++;
                        if (D_global_asm_807476FC->camera_bank[D_global_asm_807476F4].point_count < D_global_asm_807F5CF0) {
                            if (D_global_asm_807F5CF4 & 0x10) {
                                if ((gPlayerPointer->unk6A & 0x100) == 0) {
                                    gPlayerPointer->unk6A |= 0x100;
                                }
                            } else {
                                func_global_asm_8061D4E4(arg0);
                            }
                            //@recomp: On cutscene end, disable interpolation for 2f. 1f seems to not be enough
                            set_persp_interpolation_lockdown(3);
                        } else {
                            temp_t0 = D_global_asm_807476FC->camera_bank[D_global_asm_807476F4].length_array[D_global_asm_807F5CF0 - 1];
                            params = &D_global_asm_807476FC->function_bank[D_global_asm_807F5CF2].params[0];
                            switch (D_global_asm_807476FC->function_bank[D_global_asm_807F5CF2].command) {
                                case 10:
                                case 15:
                                case 16:
                                case 17:
                                    global_properties_bitfield |= 0x2000;
                                    break;
                                case 12:
                                    playSong(params[0].valu16_0, 1.0f);
                                    if (D_global_asm_807F5CEC == 0) {
                                        func_global_asm_8061D898();
                                        D_global_asm_807F5CEC = 0;
                                        var_s1 = TRUE;
                                    }
                                    break;
                                case 11:
                                    if (D_global_asm_807F5CF4 & 4) {
                                        D_global_asm_807476FC = &D_global_asm_807F5B10[0];
                                    }
                                    func_global_asm_8061D4E4(arg0);
                                    AAD->unkAC &= ~0x40000;
                                    if ((AAD->unkF3 != 0xB) && (AAD->unkF3 != 3)) {
                                        temp_f14_5 = gPlayerPointer->position.f[1];
                                        AAD->unk70 = temp_f14_5;
                                        AAD->unk6C = temp_f14_5;
                                        AAD->unkA0 = _sqrtf(SQ(*arg3 - gPlayerPointer->position.f[2]) + SQ(*arg1 - gPlayerPointer->position.f[0]));
                                        AAD->unkA4 = AAD->unkA0;
                                        AAD->unkB2 = func_global_asm_80665DE0(gPlayerPointer->position.f[0], gPlayerPointer->position.f[2], *arg1, *arg3);
                                        temp_f14_5 = *arg2 - gPlayerPointer->position.f[1];
                                        AAD->unkB8 = temp_f14_5;
                                        arg0->distance_from_floor = temp_f14_5;
                                        AAD->unk84 = *arg1;
                                        AAD->unk88 = *arg2;
                                        AAD->unk8C = *arg3;
                                        *arg4 = ((*arg4 - *arg1) * 0.1) + *arg1;
                                        *arg5 = ((*arg5 - *arg2) * 0.1) + *arg2;
                                        *arg6 = ((*arg6 - *arg3) * 0.1) + *arg3;
                                        AAD->unk78 = *arg4;
                                        AAD->unk7C = *arg5;
                                        AAD->unk80 = *arg6;
                                        if (D_global_asm_807FBB64 & 1) {
                                            func_global_asm_8062217C(arg0, 2);
                                        } else {
                                            func_global_asm_80622334(arg0, (s16) (s32) AAD->unkA0, arg2);
                                        }
                                        AAD->unk94 = AAD->unkA0 / 3.0;
                                        func_global_asm_8061F164(AAD, 0x1E);
                                        AAD->unkF3 = 1U;
                                        global_properties_bitfield &= ~0x2000;
                                        func_global_asm_8061D6A8(AAD);
                                    }
                                    break;
                                case 14:
                                    func_global_asm_8060098C(func_global_asm_8061DA14,
                                        temp_t0 + 0x80000000, 0, 0, 0);
                                    func_global_asm_8061D898();
                                    D_global_asm_807F5CEC = 0;
                                    var_s1 = TRUE;
                                    break;
                                case 13:
                                    if (temp_t0) {
                                        func_global_asm_8060098C(func_global_asm_80627F04,
                                            temp_t0 + 0x80000000,
                                            params[0].vals32,
                                            params[1].vals32,
                                            params[2].vals32);
                                    } else {
                                        func_global_asm_80627F04(
                                            params[0].vals32,
                                            params[1].vals32,
                                            params[2].vals32,
                                            D_global_asm_807F5CF2);
                                    }
                                    func_global_asm_8061D898();
                                    D_global_asm_807F5CEC = 0;
                                    var_s1 = TRUE;
                                    break;
                                case 6:
                                    D_global_asm_807F5CEE = temp_t0;
                                    func_global_asm_8061D898();
                                    D_global_asm_807F5CEC = 0;
                                    var_s1 = TRUE;
                                    break;
                                case 4:
                                case 5:
                                    //@recomp: On cutscene segment init, disable interpolation for 2f. 1f seems to not be enough
                                    set_persp_interpolation_lockdown(2);
                                    break;                            
                            }
                        }
                    }
                } while (var_s1);
                if (is_cutscene_active) {
                    D_global_asm_807F5CFC = D_global_asm_807476FC->camera_bank[D_global_asm_807476F4].length_array[D_global_asm_807F5CF0 - 1];
                    if (D_global_asm_807F5CFC != 1.0f) {
                        D_global_asm_807F5D00 = (D_global_asm_807F5CEC - 1) / (D_global_asm_807F5CFC - 1.0f);
                    } else {
                        D_global_asm_807F5D00 = 0.0f;
                    }
                    if (D_global_asm_807F5CEC > 0) {
                        D_global_asm_807F5CEC--;
                    }
                    switch (D_global_asm_807476FC->function_bank[D_global_asm_807F5CF2].command) {
                        case 8:
                        case 12:
                            break;
                        case 4:
                        case 5:
                            D_global_asm_807F5D0C->position.f[0] = D_global_asm_807F5CE8->position.f[0];
                            D_global_asm_807F5D0C->position.f[1] = D_global_asm_807F5CE8->position.f[1];
                            D_global_asm_807F5D0C->position.f[2] = D_global_asm_807F5CE8->position.f[2];
                            D_global_asm_807F5D0C->unk15E = MIN(0x14, D_global_asm_807F5CE8->unk15E);
                            D_global_asm_807F5D0C->y_rotation = D_global_asm_807F5CE8->y_rotation;
                            func_global_asm_80622B24(arg0, arg1, arg2, arg3, &AAD->unk78, &AAD->unk7C, &AAD->unk80, D_global_asm_807F5D0C);
                            *arg4 = AAD->unk78;
                            *arg5 = AAD->unk7C;
                            *arg6 = AAD->unk80;
                            break;
                        default:
                            D_global_asm_807476A4 = 0.3f;
                            func_global_asm_80622B24(arg0, arg1, arg2, arg3, &AAD->unk78, &AAD->unk7C, &AAD->unk80, D_global_asm_807F5CE8);
    
                            switch (D_global_asm_807476FC->function_bank[D_global_asm_807F5CF2].command) {
                                case 7:
                                case 0xF:
                                case 0x11:
                                    break;
                                default:
                                    var_f0 = D_global_asm_807F5CE8->distance_from_floor != 0.0f ? 0.3 : 0.1;
                                    func_global_asm_80625994(arg0, var_f0, arg4, arg5, arg6);
                                    break;
                            }
                            if ((global_properties_bitfield & 0x2000) == 0) {
                                if ((D_global_asm_807476FC->function_bank[D_global_asm_807F5CF2].command != 7) && (D_global_asm_807476FC->function_bank[D_global_asm_807F5CF2].command != 0xF) && (D_global_asm_807476FC->function_bank[D_global_asm_807F5CF2].command != 0x11)) {
                                    break;
                                }
                            }
                            global_properties_bitfield &= ~0x2000;
                            *arg4 = AAD->unk78;
                            *arg5 = AAD->unk7C;
                            *arg6 = AAD->unk80;
                    }
                    if (AAD->unkF2) {
                        func_global_asm_8061EDA0(AAD, arg1, arg2, arg3, AAD->unkC0, 0);
                    }
                }
            }
            break;
    }
    AAD->unkAC &= 0x7FFDAF0F;
    if (D_global_asm_807FBB64 & 0x8000) {
        AAD->unkAC |= 4;
    } else if ((AAD->unkAC & 0x10000) == 0) {
        AAD->unkAC &= ~4;
    }
    if (AAD->unkF2 != 0) {
        func_global_asm_8061EDA0(AAD, arg4, arg5, arg6, AAD->unkC0 * 0.333, 1);
    }
    if (!(AAD->unkAC & 0x100)) {
        func_global_asm_80627490(&sp84, &sp80, *arg4, *arg5, *arg6, *arg1, *arg2, *arg3);
        temp_f20 = func_global_asm_80612D10(sp80);
        *arg8 = func_global_asm_80612790(arg0->x_rotation);
        *arg7 = func_global_asm_80612794(arg0->x_rotation) * func_global_asm_80612D10(sp84) * temp_f20;
        *arg9 = func_global_asm_80612794(arg0->x_rotation) * func_global_asm_80612D1C(sp84) * temp_f20;
        arg0->unkEC = arg0->x_rotation;
    }
    AAD->unkCC.x = temp_v0->position.f[0];
    AAD->unkCC.y = temp_v0->position.f[1];
    AAD->unkCC.z = temp_v0->position.f[2];
}

void func_global_asm_8061B840(CameraPaad*, u8);
void func_global_asm_80620628(Actor *, f32, s16, u8);
extern f32 D_global_asm_807476A8;

// @recomp: Camera Flip handler
RECOMP_PATCH s32 func_global_asm_80620F00(Actor* arg0, u8 arg1, u8 arg2) {
    f32 temp_f2;
    CameraPaad* temp_s0;
    PlayerAdditionalActorData* temp_v0;
    u8 var_a1;
    u8 var_a2;
    u8 var_v1;
    u8 var_t7;

    temp_s0 = arg0->AAD_as_array[0];
    temp_f2 = D_global_asm_807476A8 - character_change_array[temp_s0->unkFB].near;
    temp_v0 = temp_s0->unk0->AAD_as_array[0];
    var_v1 = D_global_asm_807476A8 > 40.0f && temp_v0->unk114 < temp_f2;
    var_a1 = temp_v0->unk118 < temp_f2;
    var_a2 = temp_v0->unk11A < temp_f2;
    var_t7 = temp_v0->unk116 < temp_f2;
    if (arg2 != 0 && !var_a1 && !var_a2) {
        var_v1 = FALSE;
    }
    if (var_v1) {
        if (arg1 && (temp_s0->unk0->unkB8 != 0.0f)) {
            if (var_a1 && !var_a2) {
                temp_s0->unkB2 += 0x32;
                func_global_asm_8061B840(temp_s0, 0xA);
            } else if (var_a2 && !var_a1) {
                temp_s0->unkB2 -= 0x32;
                func_global_asm_8061B840(temp_s0, 0xA);
            }
        }
        temp_s0->unkF4++;
        if (var_t7 && (temp_s0->unkF4 >= 0x15)) {
            temp_s0->unkF4 = 0U;
            // @recomp: Disable interpolation globally for a couple frames
            set_persp_interpolation_lockdown(2);
            if ((temp_s0->unk0->unkB8 != 0.0f) && (temp_s0->unk0->control_state != 0x59)) {
                func_global_asm_80620628(arg0, 0.0f, temp_s0->unkB2, 1);
                return TRUE;
            }
            func_global_asm_80620628(arg0, 0.0f, temp_s0->unk0->y_rotation, 1);
        }
        return TRUE;
    }
    temp_s0->unkFF = 0;
    temp_s0->unkF4 = 0U;
    return FALSE;
}


typedef struct {
    s16 unk0;
    s16 unk2;
    f32 unk4; // Used
    u8 unk8; // Used
    u8 unk9;
    u8 unkA;
    u8 unkB;
    s32 unkC[2];
} GlobalASMStruct76;

typedef struct {
    Actor* unk0;
    s32 unk4;
} GlobalASMStruct53;

extern GlobalASMStruct76 D_global_asm_80750100[];
extern GlobalASMStruct53 D_global_asm_807FB930[];
extern s32 D_global_asm_807552F4[];
extern u16 D_global_asm_807FBB34;
extern s32 func_global_asm_8068A3A0(s32 arg0, u32 *arg1);
extern f32 func_global_asm_8065D0FC(f32 arg0);
extern void func_global_asm_8068A404(Actor *arg0, s32 arg1, s32 arg2);

// @recomp: LOD Handler
RECOMP_PATCH void func_global_asm_8068A508(void) {
    PlayerAdditionalActorData *PaaD;
    u32 sp80;
    f32 dz;
    f32 temp_f0_2;
    f32 dy;
    f32 temp_f20;
    f32 dx;
    s32 temp_a2;
    s32 i;
    Actor *temp_s0;

    for (i = 0; i < D_global_asm_807FBB34; i++) {
        temp_s0 = D_global_asm_807FB930[i].unk0;
        if (!(temp_s0->object_properties_bitfield & 0x2000)) {
            if ((temp_s0->object_properties_bitfield & 0x100)) {
                temp_f20 = SQ(temp_s0->z_position - character_change_array->look_at_eye_z) + (
                    SQ(temp_s0->x_position - character_change_array->look_at_eye_x) + 
                    SQ(temp_s0->y_position - character_change_array->look_at_eye_y));
                if (func_global_asm_8068A3A0(temp_s0->unk58, &sp80)) {
                    temp_f0_2 = func_global_asm_8065D0FC(D_global_asm_80750100[sp80].unk4);
                    switch (D_global_asm_80750100[sp80].unk8) {
                        case 0:
                            if (cc_number_of_players >= 2) {
                                PaaD = temp_s0->PaaD;
                                if (D_global_asm_807552F4[PaaD->unk1A4] >= 0) {
                                    func_global_asm_8068A404(temp_s0, sp80, D_global_asm_807552F4[PaaD->unk1A4]);
                                }
                            }
                            break;
                        // case 1:
                        //     if (SQ(temp_f0_2) < temp_f20) {
                        //         if (temp_s0->unk4C == 0) {
                        //             temp_s0->unk4C = func_global_asm_80612E90(temp_s0, D_global_asm_80750100[sp80].unk2, 0);
                        //         }
                        //     } else if (temp_s0->unk4C) {
                        //         func_global_asm_80613794(temp_s0, 1);
                        //     }
                        //     break;
                        // case 2:
                        //     if (temp_f20 < SQ(temp_f0_2)) {
                        //         if (temp_s0->unk4C == 0) {
                        //             temp_s0->unk4C = func_global_asm_80612E90(temp_s0, D_global_asm_80750100[sp80].unk2, 0);
                        //         }
                        //     } else if (temp_s0->unk4C) {
                        //         func_global_asm_80613794(temp_s0, 1);
                        //     }
                        //     break;
                    }
                }
            }
        }
    }
}

Gfx *func_menu_80030340(Actor *actor, s32 arg1, Gfx *dl, s32 arg3);
s16 playSound(s16 arg0, s32 arg1, f32 arg2, f32 arg3, u8 arg4, u8 arg5);
extern s8 D_menu_80033F50;

// @recomp: Menu screen switcher
RECOMP_PATCH void func_menu_8002FC1C(Actor *arg0, MenuAdditionalActorData *MaaD, s32 arg2) {
    if (MaaD->unk16 == 0) {
        MaaD->unk0 += 0.13f;
        if (1.3f < MaaD->unk0) {
            MaaD->unk0 = 1.0f;
            MaaD->unk12 = MaaD->unk13;
            MaaD->unk16 = -1;
            func_menu_80030340(arg0, 0, NULL, 0);
            set_interpolation_lockdown(2); // @recomp: Disable interp during screen transitions for 2f
            playSound(0x2C9, 0x7FFF, 63.0f, 1.25f, 0, 0);
        }
    } else {
        if (MaaD->unk0 > 0.0f) {
            MaaD->unk0 -= 0.2f;
            if (MaaD->unk0 < 0.0f) {
                MaaD->unk0 = 0.0f;
                if (arg2 != 0) {
                    playSound(0x3C, 0x61A8, 63.0f, 1.0f, 0, 0);
                    D_menu_80033F50 = 3;
                }
            }
        }
    }
}

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

void func_global_asm_806AA304(PauseAAD*, u8);
extern s8 D_global_asm_8075052C;
extern s8 D_global_asm_807505D0;
extern u8 D_global_asm_807FC7F8[2];

// @recomp: Pause screen switcher
RECOMP_PATCH s32 func_global_asm_806ABC94(PauseAAD* arg0, s32 arg1, s32 arg2) {
    s16 var_v1_2;
    s32 var_v1;
    s32 var_v0;

    if (arg1 == 0) {
        var_v1 = 0;
        if (arg2 & 8) {
            var_v1 = 1;
        } else if (arg2 & 4) {
            var_v1 = -1;
        }
        if (var_v1 != 0) {
            var_v0 = arg0->unk8[arg0->unk15] + var_v1;
            if (var_v0 < 0) {
                var_v0 = D_global_asm_807505D0;
            }
            if (D_global_asm_807505D0 < var_v0) {
                var_v0 = 0;
            }
            if (var_v0 != arg0->unk8[arg0->unk15]) {
                arg0->unk15 = 1 - arg0->unk15;
                arg0->unk8[arg0->unk15] = var_v0;
                arg0->unkC[arg0->unk15] = var_v1 * 250;
                func_global_asm_806AA304(arg0, 0);
                playSound(0x2C9, 0x7FFF, 63, 1, 0, 0);
            }
        }
    }
    if (arg1 != 0) {
        var_v1_2 = arg0->unkC[arg0->unk15];
        if (var_v1_2 >= 0x15) {
            var_v1_2 = 0x14;
        }
        if (var_v1_2 < -0x14) {
            var_v1_2 = -0x14;
        }
        arg0->unkC[0] -= var_v1_2;
        arg0->unkC[1] -= var_v1_2;
        if (arg0->unkC[arg0->unk15] == 0) {
            arg0->unk8[1 - arg0->unk15] = arg0->unk8[arg0->unk15];
            arg1 = 0;
            D_global_asm_807FC7F8[1 - arg0->unk15] = 0;
            playSound(0x97, 0x4650, 63, 1, 0, 0);
            set_interpolation_lockdown(2);
            D_global_asm_8075052C = 3;
        }
    }
    return arg1;
}


typedef struct Struct80755690_unk4 Struct80755690_unk4;

typedef struct {
    s16 unk0;
    s16 unk2;
    s16 unk4;
    u8 unk6;
    s8 unk7;
    s16 unk8;
} Struct80755690_unk4_unk14;

struct Struct80755690_unk4 {
    s32 unk0;
    s32 unk4;
    s32 unk8;
    s16 unkC;
    u8 unkE;
    u8 unkF;
    s16 unk10;
    s16 unk12;
    Struct80755690_unk4_unk14 *unk14;
    u8 unk18;
    u8 unk19;
    u8 unk1A;
    u8 unk1B;
    Struct80755690_unk4 *unk1C;
    s8 unk20;
    s8 unk21;
    s8 unk22;
    s8 unk23;
};

typedef struct {
    s16 unk0;
    s16 unk2;
    Struct80755690_unk4 *unk4;
    s32 unk8;
    s32 unkC;
    s32 unk10;
    s32 unk14;
    s32 unk18;
    s32 unk1C;
} Struct80755690;

extern void set_actor_interpolation_lockdown(Actor * ac, u16 value);
extern Struct80755690 *D_global_asm_80755690;
// @recomp: Warp actor to pen point
RECOMP_PATCH void func_global_asm_80724B5C(u8 arg0, u8 arg1, f32 *x, f32 *y, f32 *z) {
    Struct80755690_unk4 *var_v0;
    s16 i;

    var_v0 = D_global_asm_80755690->unk4;
    // @recomp: Very hacky, but since warp to spawner always uses an actor,
    // we can confidently just determine the actor pointer by counter-offsetting the x set address
    set_actor_interpolation_lockdown((Actor*)((s32)(x) - 0x7C), 2);
    for (i = 0; i < D_global_asm_80755690->unk0; i++) {
        if (var_v0->unk18 == arg0) {
            if (arg1 < var_v0->unk10) {
                *x = var_v0->unk14[arg1].unk0;
                *y = var_v0->unk14[arg1].unk2;
                *z = var_v0->unk14[arg1].unk4;
            }
            break;
        }
        var_v0++;
    }
}

void *func_global_asm_80612E90(Actor *arg0, s32 arg1, u8 arg2);
extern void func_global_asm_80613794(Actor *arg0, u8 arg1);

// @recomp: Set actor model
RECOMP_PATCH void func_global_asm_80613194(Actor *actor, s16 arg1) {
    if (actor->unk50 == 0) {
        actor->unk50 = func_global_asm_80612E90(actor, arg1, 0);
        set_actor_interpolation_lockdown(actor, 2);
    }
}

// @recomp: Set actor model (but different)
RECOMP_PATCH void func_global_asm_806131D4(Actor *actor, s16 arg1) {
    if (actor->unk50 == 0) {
        actor->unk50 = func_global_asm_80612E90(actor, arg1, 1);
        set_actor_interpolation_lockdown(actor, 2);
    }
}

// @recomp: Clear given model mask
RECOMP_PATCH void func_global_asm_80613214(Actor *actor) {
    if (actor->unk50 != 0) {
        func_global_asm_80613794(actor, 2);
        actor->unk50 = 0;
        set_actor_interpolation_lockdown(actor, 2);
    }
}

typedef struct {
    s16 count;
    s16 unk2;
    EnemySpawner *firstSpawner;
} EnemySpawnerLocator;
typedef struct {
    u16 unk0;
    u16 unk2;
    s32 unk4;
    s32 unk8;
    s16 unkC;
    u8 unkE;
    u8 unkF;
    s32 unk10;
    u8 unk14;
    u8 unk15;
    u8 unk16;
    u8 unk17;
} Struct8075EB80;


extern void func_global_asm_8067AB20(Actor *arg0, Actor *arg1, s32 arg2, u8 arg3, void *arg4, u8 arg5);
extern s32 func_global_asm_807317FC(s16 arg0, s16 arg1);
extern s32 spawnActor(Actors actorIndex, s32 modelIndex);
extern void func_global_asm_80726744(Actor *, EnemySpawner *);
extern EnemySpawnerLocator* D_global_asm_80755694;
extern GlobalASMStruct35 D_global_asm_807FBB70;
extern Struct8075EB80 D_global_asm_8075EB80[];
extern Actor *gLastSpawnedActor;
extern Actor *gCurrentActorPointer;

// @recomp: Initiate Char Spawner item action
RECOMP_PATCH void func_global_asm_80727678(void) {
    s32 pad;
    s16 i, j;
    EnemySpawner *var_s0;
    Struct807FBB70_unk278 *temp_t0;

    var_s0 = D_global_asm_80755694->firstSpawner;
    for (i = 0; i < D_global_asm_80755694->count; i++) {
        for (j = 0; j < D_global_asm_807FBB70.unk254; j++) {
            temp_t0 = D_global_asm_807FBB70.unk278[j];
            if (temp_t0->unk0 != var_s0->init.spawn_trigger) {
                continue;
            }
            switch (D_global_asm_807FBB70.unk258[j]) {
            case 1:
                if (
                    (var_s0->spawn_state == 0) && 
                    func_global_asm_807317FC(current_map, var_s0->init.spawn_trigger) && 
                    spawnActor(D_global_asm_8075EB80[var_s0->alternative_enemy_spawn].unk0, D_global_asm_8075EB80[var_s0->alternative_enemy_spawn].unk2)) {
                    func_global_asm_80726744(gLastSpawnedActor, var_s0);
                } else if ((var_s0->spawn_state == 3) && 
                    func_global_asm_807317FC(current_map, var_s0->init.spawn_trigger)) {
                    if (spawnActor(D_global_asm_8075EB80[var_s0->alternative_enemy_spawn].unk0, D_global_asm_8075EB80[var_s0->alternative_enemy_spawn].unk2)) {
                        func_global_asm_80726744(gLastSpawnedActor, var_s0);
                        gLastSpawnedActor->control_state = 0x36;
                    }
                }
                break;
            case 2:
                if (var_s0->spawn_state == 7) {
                    var_s0->spawn_state = var_s0->init.something_spawn_state;
                    break;
                }
                break;
            case 3:
            case 4:
                if (var_s0->spawn_state == 5) {
                    if ((D_global_asm_807FBB70.unk258[j] == 3) && (D_global_asm_807FBB70.unk278[j]->unk2 == 6)) {
                        // @recomp: If unk2 == 6, then (according to 806bfdbc) it's going to do a position warp
                        // Set interpolation lockdown for the relevant actor
                        set_actor_interpolation_lockdown(var_s0->tied_actor, 2);
                    }
                    func_global_asm_8067AB20(gCurrentActorPointer, var_s0->tied_actor, 0x01000000, D_global_asm_807FBB70.unk258[j], temp_t0, 0U);
                    break;
                }
                break;
            }
        }
        var_s0++;
    }
}

void func_global_asm_80613CA8(Actor*, s16, f32, f32);      /* extern */
void func_global_asm_8068A858(u8*, u8*, u8*);          /* extern */
void func_global_asm_807238D4(u8, f32*, f32*, f32*);   /* extern */
s32 func_global_asm_8072881C(s32, s32*);              /* extern */
void func_global_asm_8072A450(void);                       /* extern */
extern void *D_global_asm_80746B80[];
extern u16 D_global_asm_80747750[];
extern rgb D_global_asm_807478F4[];
extern u16 D_global_asm_80747904[];
extern rgb D_global_asm_80747B00[];
extern f32 D_global_asm_807502E8;
extern u8 D_global_asm_807FBDC4;

typedef struct Struct807FDC90 Struct807FDC90;
struct Struct807FDC90 {
    Struct807FDC90 *unk0;
    Actor *unk4;
    s16 unk8;
    s16 unkA;
    s16 unkC;
    s16 unkE;
    s16 unk10;
    s16 unk12;
    s16 unk14;
    s16 unk16;
    u16 unk18;
    u16 unk1A;
    union {
        struct {
            u16 unk1C;
            u8 unk1E;
            u8 unk1F;
        };
        s32 unk1C_s32;
    };
    u8 unk20;
    u8 unk21;
    u8 unk22;
    u8 unk23;
    u8 unk24;
    u8 unk25;
    s16 unk26;
    s32 unk28;
    s16 unk2C;
    s16 unk2E;
    f32 unk30;
    u8 unk34;
    u8 unk35;
    u8 unk36;
    u8 unk37;
    u8 unk38;
};

typedef struct Struct806BFBF4_AAD17C {
    u16 unk0;
    s16 unk2;
    s16 unk4;
    s16 unk6;
    s16 unk8;
    s16 unkA;
    s16 unkC;
    s16 unkE;
    s16 unk10;
    u8 pad12[0x14 - 0x12];
    f32 unk14;
    s16 unk18;
    s16 unk1A;
    u16 unk1C;
    u16 unk1E;
    u8 pad20[0x22 - 0x20];
    u8 unk22;
    u8 unk23;
    u8 unk24;
    u8 unk25;
    u8 unk26;
    u8 unk27;
    u8 unk28;
    u8 unk29;
    u8 unk2A;
    u8 pad2B[0x2D - 0x2B];
    u8 unk2D;
    u8 unk2E;
    u8 unk2F;
    u8 unk30;
    u8 unk31;
    u8 unk32;
    u8 unk33;
} Struct806BFBF4_AAD17C;

typedef struct {
    s16 unk0;
    s16 unk2;
    s16 unk4;
    u8 unk6;
    u8 unk7;
    u8 unk8;
    u8 unk9;
} Struct807FDCA0_unk14;

typedef struct Struct807FDCA0 {
    s32 unk0;
    s32 unk4;
    s32 unk8;
    s32 unkC;
    s16 unk10;
    s16 unk12;
    Struct807FDCA0_unk14 *unk14;
    s8 unk18;
    u8 unk19;
    s16 unk1A;
    Actor *unk1C;
    u8 unk20;
} Struct807FDCA0;
void func_global_asm_806F09F0(Actor *arg0, u16 arg1);
void func_global_asm_8072B324(Actor *arg0, s32 arg1);
s32 func_global_asm_80723020(Actor *arg0, s32 arg1, s32 arg2, f32 arg3, f32 arg4, f32 arg5, u8 arg6);
void func_global_asm_80723284(s32 arg0, u8 arg1);
void func_global_asm_80723320(s32 arg0, s32 arg1);
void func_global_asm_8072334C(s32 arg0, u8 arg1);
void playActorAnimation(Actor *arg0, s32 arg1);
void func_global_asm_80614D00(Actor *arg0, f32 arg1, f32 arg2);
void func_global_asm_806BFA8C(u16 arg0);
void func_global_asm_80611408(void *ptr);
extern void *_malloc(s32);
void func_global_asm_807248B0(Actor *arg0, f32 arg1);
Actor *getSpawnerTiedActor(s16 spawn_trigger, u16 arg1);
s16 func_global_asm_806CC284(s16 arg0, s16 arg1, f32 arg2);
u8 func_global_asm_80723C98(s32 arg0);
Struct80717D84 *drawSpriteAtPosition(void *sprite, f32 scale, f32 x, f32 y, f32 z);
void func_global_asm_80723484(s32 arg0);
void func_global_asm_80714950(s32 arg0);
void func_global_asm_8071498C(void *arg0);
void func_global_asm_807149A8(s16 arg0);
void func_global_asm_807149B8(u8 arg0);
void func_global_asm_80714998(u8 arg0);
void changeActorColor(u8 red, u8 green, u8 blue, u8 alpha);
void func_global_asm_8072DC7C(u8 arg0);
void func_global_asm_806653C0(Actor *arg0, f32 arg1, f32 arg2);
void func_global_asm_80729E6C(void);
u8 func_global_asm_8072D13C(u8 arg0, s32 arg1);
void func_global_asm_80717D4C(Struct80717D84 *arg0, s32 arg1);
void func_global_asm_8072E320(f32 arg0);
u8 func_global_asm_8072AB74(u8 arg0, f32 x, f32 z, u16 arg3, f32 arg4);
extern Struct807FDC90 *D_global_asm_807FDC90;
extern Actor *D_global_asm_807FDC94;
extern EnemySpawner *D_global_asm_807FDC98;
extern CharacterSpawner *D_global_asm_807FDC9C;
extern Struct807FDCA0 *D_global_asm_807FDCA0;

RECOMP_PATCH void func_global_asm_806BFBF4(void) {
    Struct806BFBF4_AAD17C* temp_s2;
    s16 sp92;
    Struct807FBB70_unk278* temp_s0;
    Struct807FDCA0_unk14* temp_a3;
    Struct807FDCA0_unk14* temp_v1;
    f32 temp_f0_2;
    f32 var_f10;
    f32 var_f16;
    f32 sp74;
    f32 sp70;
    f32 sp6C;
    f32 sp68;
    f32 var_f18_3;
    f32 var_f6;
    s16 var_s1;

    f32 old_x, old_y, old_z;

    temp_s2 = gCurrentActorPointer->AAD_as_array[2];
    if (!(gCurrentActorPointer->object_properties_bitfield & 0x10)) {
        temp_s2->unk1A = 0;
        temp_s2->unk18 = 0;
        temp_s2->unk30 = 0xFF;
        temp_s2->unk33 = 0x12;
        temp_s2->unk22 = 0x64;
        func_global_asm_806F09F0(gCurrentActorPointer, gCurrentActorPointer->unk58);
        func_global_asm_8072B324(gCurrentActorPointer, 0);
        gCurrentActorPointer->object_properties_bitfield &= ~0x20000;
        if (current_map == MAP_DK_RAP) {
            gCurrentActorPointer->object_properties_bitfield &= ~0x1000000;
            gCurrentActorPointer->object_properties_bitfield |= 0x400;
        }
        gCurrentActorPointer->y_acceleration = D_global_asm_807502E8;
    }
    if ((is_cutscene_active != 1) && (current_map != MAP_DK_RAP) && (gCurrentActorPointer->unk58 != ACTOR_CUTSCENE_OBJECT)) {
        gCurrentActorPointer->object_properties_bitfield &= ~0x10000000;
        return;
    }
    if (current_map == MAP_DK_RAP) {
        func_global_asm_8068A858(&gCurrentActorPointer->unk16A, &gCurrentActorPointer->unk16B, &gCurrentActorPointer->unk16C);
    }
    for (sp92 = 0; sp92 < D_global_asm_807FBDC4; sp92++) {
        if (D_global_asm_807FBB70.unk258[sp92] == 3) {
            temp_s0 = D_global_asm_807FBB70.unk278[sp92];
            switch (temp_s0->unk2) {                  /* switch 1 */
            case 25:                            /* switch 1 */
                temp_s2->unk33 = temp_s0->unk4;
                temp_s2->unk22 = temp_s0->unk6;
                break;
            case 24:                            /* switch 1 */
                temp_s2->unk30 = func_global_asm_80723020(gCurrentActorPointer, temp_s2->unk2D, 0, 0.0f, 0.0f, 0.0f, 0U);
                temp_s2->unk31 = temp_s0->unk4;
                temp_s2->unk32 = temp_s0->unk6;
                func_global_asm_80723284(temp_s2->unk30, temp_s2->unk2E / temp_s0->unk4);
                func_global_asm_80723320(temp_s2->unk30, 1);
                func_global_asm_8072334C(temp_s2->unk30, 1U);
                break;
            case 23:                            /* switch 1 */
                temp_s2->unk2D = temp_s0->unk4;
                temp_s2->unk2E = temp_s0->unk6;
                temp_s2->unk2F = func_global_asm_80723020(gCurrentActorPointer, temp_s2->unk2D, 0, 0.0f, 0.0f, 0.0f, 0U);
                func_global_asm_80723284(temp_s2->unk2F, temp_s2->unk2E);
                func_global_asm_80723320(temp_s2->unk2F, 1);
                func_global_asm_8072334C(temp_s2->unk2F, 1U);
                gCurrentActorPointer->control_state = 0xC;
                gCurrentActorPointer->control_state_progress = 0;
                break;
            case 0:                             /* switch 1 */
                gCurrentActorPointer->y_velocity = temp_s0->unk4;
                break;
            case 7:                             /* switch 1 */
                gCurrentActorPointer->unkB8 = temp_s0->unk4;
                gCurrentActorPointer->unkEE = (s16) ((s32) (temp_s0->unk6 << 0xC) / 360);
                break;
            case 2:                             /* switch 1 */
                gCurrentActorPointer->animation_state->unk0->unk10 = -1;
                playActorAnimation(gCurrentActorPointer, D_global_asm_80747750[temp_s0->unk4]);
                if (temp_s0->unk6) {
                    func_global_asm_80614D00(gCurrentActorPointer, (temp_s0->unk6 / 100.0), 0.0f);
                }
                break;
            case 3:                             /* switch 1 */
                if (gCurrentActorPointer->animation_state->unk64) {
                    playActorAnimation(gCurrentActorPointer, 0);
                }
                func_global_asm_80613CA8(gCurrentActorPointer, temp_s0->unk6 + D_global_asm_80747904[temp_s0->unk4], 0.0f, 0.0f);
                func_global_asm_80614D00(gCurrentActorPointer, 1.0f, 0.0f);
                func_global_asm_806BFA8C(D_global_asm_80747904[temp_s0->unk4] + temp_s0->unk6);
                break;
            case 13:                            /* switch 1 */
                if (gCurrentActorPointer->animation_state->unk64 != 0) {
                    playActorAnimation(gCurrentActorPointer, 0);
                }
                func_global_asm_80613CA8(gCurrentActorPointer, temp_s0->unk6 + D_global_asm_80747904[temp_s0->unk4], 0.0f, 8.0f);
                func_global_asm_80614D00(gCurrentActorPointer, 1.0f, 0.0f);
                func_global_asm_806BFA8C(temp_s0->unk6 + D_global_asm_80747904[temp_s0->unk4]);
                break;
            case 4:                             /* switch 1 */
                func_global_asm_80614D00(gCurrentActorPointer, temp_s0->unk4 / 100.0, temp_s0->unk6);
                break;
            case 5:                             /* switch 1 */
                gCurrentActorPointer->control_state = 2;
                gCurrentActorPointer->control_state_progress = 0;
                if (D_global_asm_807FDC98->unk20 != NULL) {
                    func_global_asm_80611408(D_global_asm_807FDC98->unk20);
                }
                D_global_asm_807FDC9C->unk11 = 1;
                D_global_asm_807FDC98->unk20 = _malloc(2);
                D_global_asm_807FDC98->unk20->unk1 = 0;
                D_global_asm_807FDC98->unk20->unk0 = temp_s0->unk4;
                D_global_asm_807FDC90->unk25 = 0;
                func_global_asm_8072B324(gCurrentActorPointer, (s32) temp_s0->unk6);
                break;
            case 6:                             /* switch 1 */
                gCurrentActorPointer->unkB8 = 0.0f;
                gCurrentActorPointer->y_velocity = 0.0f;
                gCurrentActorPointer->x_position = D_global_asm_807FDCA0->unk14[temp_s0->unk4].unk0;
                gCurrentActorPointer->y_position = D_global_asm_807FDCA0->unk14[temp_s0->unk4].unk2;
                gCurrentActorPointer->z_position = D_global_asm_807FDCA0->unk14[temp_s0->unk4].unk4;
                gCurrentActorPointer->y_rotation = (temp_s0->unk6 << 0xC) / 360;
                gCurrentActorPointer->unkEE = gCurrentActorPointer->y_rotation;
                break;
            case 8:                             /* switch 1 */
                if (temp_s0->unk6) {
                    temp_s2->unk2 = gCurrentActorPointer->y_rotation;
                    temp_s2->unk4 = (temp_s0->unk4 << 0xC) / 360;
                    temp_s2->unk23 = temp_s0->unk6;
                    temp_s2->unk24 = temp_s0->unk6;
                } else {
                    gCurrentActorPointer->y_rotation = (temp_s0->unk4 << 0xC) / 360;
                }
                break;
            case 19:                            /* switch 1 */
                if (temp_s0->unk6) {
                    temp_s2->unk6 = gCurrentActorPointer->x_rotation;
                    temp_s2->unk8 = ((s32) (temp_s0->unk4 << 0xC) / 360);
                    temp_s2->unk25 = temp_s0->unk6;
                    temp_s2->unk26 = temp_s0->unk6;
                } else {
                    gCurrentActorPointer->x_rotation = (temp_s0->unk4 << 0xC) / 360;
                }
                break;
            case 20:                            /* switch 1 */
                if (temp_s0->unk6) {
                    temp_s2->unkA = gCurrentActorPointer->z_rotation;
                    temp_s2->unkC = (temp_s0->unk4 << 0xC) / 360;
                    temp_s2->unk27 = temp_s0->unk6;
                    temp_s2->unk28 = temp_s0->unk6;
                } else {
                    gCurrentActorPointer->z_rotation = (temp_s0->unk4 << 0xC) / 360;
                }
                break;
            case 21:                            /* switch 1 */
                gCurrentActorPointer->y_acceleration = temp_s0->unk4;
                gCurrentActorPointer->y_velocity = 0.0f;
                gCurrentActorPointer->control_state = 0x1D;
                break;
            case 16:                            /* switch 1 */
                if (temp_s0->unk6) {
                    temp_s2->unk14 = gCurrentActorPointer->animation_state->scale[1];
                    D_global_asm_807FDC90->unk30 = temp_s0->unk4 * 0.01;
                    temp_s2->unk29 = temp_s0->unk6;
                    temp_s2->unk2A = temp_s0->unk6;
                } else {
                    func_global_asm_807248B0(gCurrentActorPointer, temp_s0->unk4 * 0.01);
                }
                break;
            case 9:                             /* switch 1 */
                D_global_asm_807FDC90->unk4 = getSpawnerTiedActor(temp_s0->unk4, 0U);
                break;
            case 11:                            /* switch 1 */
                temp_s2->unk1C = temp_s0->unk4;
                break;
            case 12:                            /* switch 1 */
                temp_s2->unkE = temp_s0->unk4;
                temp_s2->unk10 = temp_s0->unk6;
                temp_s2->unk1E = temp_s2->unk1C;
                gCurrentActorPointer->control_state = 0xE;
                gCurrentActorPointer->control_state_progress = 0;
                break;
            case 14:                            /* switch 1 */
                temp_s2->unkE = temp_s0->unk4;
                temp_s2->unk10 = temp_s0->unk6;
                temp_s2->unk1E = temp_s2->unk1C;
                gCurrentActorPointer->control_state = 0xF;
                gCurrentActorPointer->control_state_progress = 0;
                break;
            case 10:                            /* switch 1 */
                temp_s2->unk18 = (temp_s0->unk4 << 0xC) / 360;
                temp_s2->unk1A = temp_s0->unk6;
                break;
            case 15:                            /* switch 1 */
                gCurrentActorPointer->unk16A = D_global_asm_807478F4[temp_s0->unk4].red;
                gCurrentActorPointer->unk16B = D_global_asm_807478F4[temp_s0->unk4].green;
                gCurrentActorPointer->unk16C = D_global_asm_807478F4[temp_s0->unk4].blue;
                break;
            case 17:                            /* switch 1 */
                gCurrentActorPointer->object_properties_bitfield &= ~0x8000;
                gCurrentActorPointer->control_state = 0x37;
                gCurrentActorPointer->control_state_progress = 0;
                break;
            case 18:                            /* switch 1 */
                gCurrentActorPointer->object_properties_bitfield &= ~0x800000;
                break;
            case 26:                            /* switch 1 */
                D_global_asm_807FDC90->unk4 = getSpawnerTiedActor(temp_s0->unk4, 0U);
                gCurrentActorPointer->unk15F = temp_s0->unk6;
                gCurrentActorPointer->control_state = 0xD;
                gCurrentActorPointer->control_state_progress = 0;
                break;
            case 22:                            /* switch 1 */
                gCurrentActorPointer->control_state = 2;
                gCurrentActorPointer->control_state_progress = 0;
                break;
            }
        }
    }
    if (temp_s2->unk28) {
        var_f16 = temp_s2->unk28;
        var_f6 = temp_s2->unk27;
        temp_s2->unk28--;
        gCurrentActorPointer->z_rotation = func_global_asm_806CC284(temp_s2->unkC, temp_s2->unkA, var_f16 / var_f6);
    }
    if (temp_s2->unk26) {
        var_f16 = temp_s2->unk26;
        var_f6 = temp_s2->unk25;
        temp_s2->unk26--;
        gCurrentActorPointer->x_rotation = func_global_asm_806CC284(temp_s2->unk8, temp_s2->unk6, var_f16 / var_f6);
    }
    if (temp_s2->unk24) {
        var_f16 = temp_s2->unk24;
        var_f6 = temp_s2->unk23;
        temp_s2->unk24--;
        gCurrentActorPointer->y_rotation = func_global_asm_806CC284(temp_s2->unk4, temp_s2->unk2, var_f16 / var_f6);
        gCurrentActorPointer->unkEE = gCurrentActorPointer->y_rotation;
    }
    if (temp_s2->unk2A) {
        var_f16 = temp_s2->unk2A;
        var_f6 = temp_s2->unk29;
        temp_s2->unk2A--;
        func_global_asm_807248B0(gCurrentActorPointer, D_global_asm_807FDC90->unk30 + ((temp_s2->unk14 - D_global_asm_807FDC90->unk30) * (var_f16 / var_f6)));
    }
    if (temp_s2->unk30 != 0xFF) {
        for (var_s1 = 0; var_s1 < temp_s2->unk31 && temp_s2->unk30 != 0xFF; var_s1++) {
            sp74 = ((((RANDNUM() >> 0xF) % 32767) % 51) + 0x4B) * 0.01;
            if (temp_s2->unk30 == 0xFE) {
                getBonePosition(gCurrentActorPointer, 4, &sp70, &sp6C, &sp68);
            } else if (func_global_asm_80723C98(temp_s2->unk30)) {
                func_global_asm_80723484(temp_s2->unk30);
                func_global_asm_807238D4(temp_s2->unk30, &sp70, &sp6C, &sp68);
                sp6C += 10.0f;
            } else {
                temp_s2->unk30 = 0xFFU;
            }
            if ((temp_s2->unk30 == 0xFE) || ((temp_s2->unk30 != 0xFF) && (func_global_asm_80723C98(temp_s2->unk30) != 0))) {
                func_global_asm_807149A8(0x7D0);
                func_global_asm_807149B8(1U);
                func_global_asm_80714998(2U);
                changeActorColor(
                    D_global_asm_80747B00[temp_s2->unk32].red,
                    D_global_asm_80747B00[temp_s2->unk32].green,
                    D_global_asm_80747B00[temp_s2->unk32].blue,
                    0xFF
                );
                func_global_asm_8071498C(func_global_asm_80717D4C);
                func_global_asm_80714950(-0xA - ((RANDNUM() >> 0xF) % 30));
                drawSpriteAtPosition(D_global_asm_80746B80[temp_s2->unk33], (f32) ((f64) (f32) temp_s2->unk22 * 0.01 * (f64) sp74), sp70, sp6C, sp68);
            }
        }
    }
    switch (gCurrentActorPointer->control_state) {                      /* switch 2; irregular */
        case 0xD:                                       /* switch 2 */
            if (gCurrentActorPointer->unk15F) {
                getBonePosition(D_global_asm_807FDC90->unk4, gCurrentActorPointer->unk15F, gCurrentActorPointer->position.f, &gCurrentActorPointer->position.f[1], &gCurrentActorPointer->position.f[2]);
            } else {
                gCurrentActorPointer->x_position = D_global_asm_807FDC94->x_position;
                gCurrentActorPointer->y_position = D_global_asm_807FDC94->y_position;
                gCurrentActorPointer->z_position = D_global_asm_807FDC94->z_position;
            }
            gCurrentActorPointer->y_rotation = D_global_asm_807FDC90->unk4->y_rotation;
            break;
        case 0xC:                                       /* switch 2 */
            func_global_asm_80723484(temp_s2->unk2F);
            func_global_asm_807238D4(temp_s2->unk2F, gCurrentActorPointer->position.f, &gCurrentActorPointer->position.f[1], &gCurrentActorPointer->position.f[2]);
            func_global_asm_8072E320(8.0f);
            break;
        case 0xE:                                       /* switch 2 */
        case 0xF:                                       /* switch 2 */
            if (temp_s2->unk1E) {
                temp_a3 = &D_global_asm_807FDCA0->unk14[temp_s2->unkE];
                temp_v1 = &D_global_asm_807FDCA0->unk14[temp_s2->unk10];
                var_f10 = temp_s2->unk1E;
                var_f18_3 = temp_s2->unk1C;
                temp_f0_2 = var_f10 / var_f18_3;
                temp_s2->unk1E--;
                old_x = gCurrentActorPointer->x_position;
                old_y = gCurrentActorPointer->y_position;
                old_z = gCurrentActorPointer->z_position;
                gCurrentActorPointer->x_position = temp_v1->unk0 + ((temp_a3->unk0 - temp_v1->unk0) * temp_f0_2);
                gCurrentActorPointer->z_position = temp_v1->unk4 + ((temp_a3->unk4 - temp_v1->unk4) * temp_f0_2);
                if (gCurrentActorPointer->control_state == 0xF) {
                    gCurrentActorPointer->y_position = temp_v1->unk2 + ((temp_a3->unk2 - temp_v1->unk2) * temp_f0_2);
                } else {
                    func_global_asm_8072AB74(0U, temp_v1->unk0, temp_v1->unk4, 0x212U, 0.0f);
                }
                if (
                    (ABS_F(gCurrentActorPointer->x_position - old_x) > 100) || 
                    (ABS_F(gCurrentActorPointer->y_position - old_y) > 100) || 
                    (ABS_F(gCurrentActorPointer->z_position - old_z) > 100)
                ) {
                    set_actor_interpolation_lockdown(gCurrentActorPointer, 2);
                }
            } else {
                gCurrentActorPointer->control_state = 0;
                gCurrentActorPointer->control_state_progress = 0;
                func_global_asm_8072B324(gCurrentActorPointer, 0);
            }
            break;
        case 0x2:                                       /* switch 2 */
            func_global_asm_8072AB74(2U, D_global_asm_807FDC90->unkA, D_global_asm_807FDC90->unkE, 0x210U, 0.0f);
            if (func_global_asm_8072D13C(2U, 0)) {
                gCurrentActorPointer->control_state = 0;
                gCurrentActorPointer->control_state_progress = 0;
                func_global_asm_8072B324(gCurrentActorPointer, 0);
            }
            break;
        case 0x0:                                       /* switch 2 */
            func_global_asm_8072AB74(0U, D_global_asm_807FDC90->unkA, D_global_asm_807FDC90->unkE, 0x210U, 0.0f);
            break;
        case 0x1D:                                      /* switch 2 */
            func_global_asm_80729E6C();
            func_global_asm_806653C0(gCurrentActorPointer, 0.0f, gCurrentActorPointer->y_velocity);
            break;
        case 0x37:                                      /* switch 2 */
            if (gCurrentActorPointer->control_state_progress) {
                gCurrentActorPointer->control_state = 0x40;
            } else {
                func_global_asm_8072DC7C(0xAU);
            }
            break;
    }
    func_global_asm_8072A450();
    if ((D_global_asm_807FDC98->properties_bitfield & 0x1000) && (func_global_asm_8072881C(0, &D_global_asm_807FDC90->unk28)) && (((temp_s2->unk0 == 1)) || (temp_s2->unk0 == 9) || (temp_s2->unk0 == 0x68))) {
        func_global_asm_8072881C(0x81, &D_global_asm_807FDC90->unk28);
    }
}

extern u8 D_global_asm_8076A0B1;
extern u8 D_global_asm_8076A0B3;
void func_global_asm_8061C464(Actor *arg0, Actor *arg1, u8 arg2, s16 arg3, s16 arg4, s16 arg5, s16 arg6, s16 arg7, s16 arg8, s16 arg9, f32 argA);
void func_global_asm_8061C6A8(Actor *arg0, Actor *arg1, u8 arg2, s16 arg3, s16 arg4, s16 arg5, s16 arg6, s16 arg7, s16 arg8, s16 arg9, f32 arg10);

void clearCutsceneBarInterp(void) {
    if (recomp_get_cutscene_bordering() > 0) {
        set_persp_interpolation_lockdown(2);
    }
}

RECOMP_PATCH void func_global_asm_8061C518(Actor *arg0, Actor *arg1, u8 arg2, s16 arg3, s16 arg4, s16 arg5, s16 arg6, s16 arg7, s16 arg8, s16 arg9, f32 argA) {
    f32 sp3C;

    sp3C = arg1->animation_state->scale[1];
    if (is_cutscene_active == 1) {
        func_global_asm_8061D4E4(arg0);
    }
    D_global_asm_8076A0B3 = 0;
    clearCutsceneBarInterp();
    D_global_asm_8076A0B1 |= 0x10;
    arg1->animation_state->scale[1] = 0.15f;
    func_global_asm_8061C6A8(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, argA);
    arg1->animation_state->scale[1] = sp3C;
    global_properties_bitfield &= ~1;
}

RECOMP_PATCH void func_global_asm_8061C600(Actor *arg0, Actor *arg1, u8 arg2, s16 arg3, s16 arg4, s16 arg5, s16 arg6, s16 arg7, s16 arg8, s16 arg9, f32 argA) {
    if (is_cutscene_active == 1) {
        func_global_asm_8061D4E4(arg0);
    }
    D_global_asm_8076A0B3 = 0;
    clearCutsceneBarInterp();
    D_global_asm_8076A0B1 |= 0x10;
    func_global_asm_8061C464(
        arg0,
        arg1,
        arg2,
        arg3,
        arg4,
        arg5,
        arg6,
        arg7,
        arg8,
        arg9,
        argA
    );
}

extern OSTime D_global_asm_807F5CE0;
#define D_global_asm_807476D0 *(volatile OSTime *)(0x807476D0)
u8 isIntroStoryPlaying(void);
void func_global_asm_806119F0(s32 arg0);
u16 func_global_asm_8061C804(s16);
void func_global_asm_80629174(void);
void func_boss_80029140(s16* arg0);
s32 deleteActor(Actor*);
extern s16 D_global_asm_807476F8;
extern s8 D_global_asm_807F5CFA;
extern u8 D_global_asm_807F5D14;
extern u8 D_global_asm_807476EC;
extern Actor *D_global_asm_807F5D10;
extern u8 D_global_asm_807F5CF6;
extern u8 D_global_asm_807476D8;
extern u8 D_global_asm_80770DC9;
extern u8 current_character_index[];
extern u16 D_global_asm_807FBB34;

RECOMP_PATCH s32 playCutscene(Actor *arg0, s16 arg1, u8 arg2) {
    u16 sp26;
    s32 i;
    Actor *ac;

    sp26 = 0;
    if ((is_cutscene_active == 1) && (D_global_asm_807F5CF4 & 0x80)) {
        return 0;
    }

    if ((arg2 & 4) && (current_map != MAP_TEST_MAP)) {
        D_global_asm_807476FC = &D_global_asm_807F5B10[1];
    } else {
        D_global_asm_807476FC = &D_global_asm_807F5B10[0];
    }

    // @recomp: Patch cutscene controller duping, which fixes a bug if you're not  so good at Owl Race (https://www.youtube.com/watch?v=A0bWoo1h8FQ)
    for (i = 0; i < D_global_asm_807FBB34; i++) {
        ac = D_global_asm_807FB930[i].unk0;
        if (ac->unk58 == 173) {
            // @recomp: Delete any lingering cutscene controllers
            deleteActor(ac);
        }
    }

    if (spawnActor(ACTOR_CUTSCENE_CONTROLLER, 0)) {
        D_global_asm_807F5D0C = gLastSpawnedActor;
        gLastSpawnedActor->noclip_byte = 1;
    } else {
        return 0;
    }
    
    recomp_on_cutscene_play(&arg1, &arg2);

    if ((!(arg2 & 4)) && (D_global_asm_807FBB64 & 1)) {
        func_boss_80029140(&arg1);
    }

    if (arg0 != NULL) {
        D_global_asm_807F5CE8 = arg0;
    } else {
        D_global_asm_807F5CE8 = character_change_array->playerPointer;
    }

    is_cutscene_active = 1;

    if (!(arg2 & 8)) {
        D_global_asm_8076A0B1 |= 0x10;
        D_global_asm_8076A0B3 = 0;
        clearCutsceneBarInterp();
    }

    D_global_asm_807476D0 = osGetTime();
    D_global_asm_807476F4 = arg1;
    D_global_asm_807476F8 = arg1;
    D_global_asm_807F5CF4 = arg2;
    D_global_asm_807F5CFA = 0;
    D_global_asm_807476D8 = 0;
    D_global_asm_807476E4 = 0;
    D_global_asm_807F5CEC = 0;
    D_global_asm_807F5CF0 = 0;
    D_global_asm_807F5CF2 = 0;
    D_global_asm_807F5CEE = 0;
    D_global_asm_807476F0 = 0;
    D_global_asm_807F5CF6 = D_global_asm_80770DC9;
    global_properties_bitfield |= 0x2000;
    global_properties_bitfield &= ~0x1001;
    gPlayerPointer->unkB8 = 0.0f;
    if (current_character_index[0] == 7) {
        gPlayerPointer->y_velocity = 0.0f;
    }
    gPlayerPointer->object_properties_bitfield |= 0x400;
    extra_player_info_pointer->unk10 = 0;
    D_global_asm_807F5D10->x_rotation = 0;
    func_global_asm_80629174();
    if (D_global_asm_807476EC != 0) {
        sp26 = func_global_asm_8061C804(arg1);
    }
    D_global_asm_807476EC = 0;
    if ((arg1 == 0) && (current_map == MAP_DK_ISLES_DK_THEATRE)) {
        func_global_asm_806119F0(0x8E32B6F7U);
        D_global_asm_807F5CE0 = osGetTime();
        D_global_asm_807F5D14 = 0;
    } else if (!isIntroStoryPlaying()) {
        D_global_asm_807F5CE0 = 0;
    }
    return sp26;
}

typedef struct AutowalkRDRAM AutowalkRDRAM;
struct AutowalkRDRAM {
    s16 count;
    AutowalkRDRAM *items;
};
extern AutowalkRDRAM *D_global_asm_80753E90;
extern u8 is_autowalking;
extern void *D_global_asm_807FD70C;
extern AutowalkRDRAM *D_global_asm_807FD708;
extern Actor *D_global_asm_807FD710;
extern s16 D_global_asm_807FD714;
extern s16 D_global_asm_807FD718;
extern Actor *D_global_asm_807FD71C;
void func_global_asm_806F37BC(Actor *arg0, void *arg1);

RECOMP_PATCH void func_global_asm_806F386C(u8 arg0, Actor *arg1, Actor *arg2, s16 arg3, u8 arg4) {
    PlayerAdditionalActorData *temp_v0;

    temp_v0 = arg1->PaaD;
    if (D_global_asm_80753E90[0].count >= arg0) {
        is_autowalking = 3;
        D_global_asm_8076A0B1 |= 0x10;
        clearCutsceneBarInterp();
        D_global_asm_807FD710 = arg1;
        temp_v0->unk1F0 &= ~1;
        D_global_asm_807FD714 = 0;
        D_global_asm_807FD708 = &D_global_asm_80753E90->items[arg0];
        D_global_asm_807FD70C = D_global_asm_807FD708->items;
        D_global_asm_807FD718 = arg3;
        D_global_asm_807FD71C = arg2;
        if (arg4 == 0) {
            func_global_asm_806F37BC(arg1, D_global_asm_807FD70C);
        }
        recomp_on_autowalk();
    }
}