#include "common_structs.h"
#include "debug_config.h"
#include "input.h"
#include "sound.h"
#include "options.h"

extern s8 D_global_asm_80745840;
extern s8 D_global_asm_8074583C;
extern void func_global_asm_8060A398(s32 arg0);
extern void func_global_asm_80737B58(u8 arg0, u16 arg1);
RECOMP_DECLARE_EVENT(recomp_on_file_start());
RECOMP_DECLARE_EVENT(recomp_on_new_file_start());
RECOMP_DECLARE_EVENT(recomp_on_dirty_file_start());
RECOMP_DECLARE_EVENT(recomp_on_flag_check(s16 *flag, u8 *flag_type));
RECOMP_DECLARE_EVENT(recomp_on_flag_change(s16 *flag, u8 *target_state, u8 *flag_type));

void AlterVolumes(void) {
    s32 i;
    s8 new_bgm_vol;
    s8 new_sfx_vol;

    //@recomp: Music Volume
    new_bgm_vol = recomp_get_bgm_volume();
    if (D_global_asm_80745840 != new_bgm_vol) {
        D_global_asm_80745840 = new_bgm_vol;
        func_global_asm_8060A398(0);
        func_global_asm_8060A398(2);
    }
    new_sfx_vol = recomp_get_sfx_volume();
    if (D_global_asm_8074583C != new_sfx_vol) {
        D_global_asm_8074583C = new_sfx_vol;
        for (i = 0; i < 4; i++) {
            func_global_asm_80737B58(i, new_sfx_vol * 625);
        }
    }
}

extern u16 D_global_asm_80744734[];
extern u8 isFlagSet(s16 flagIndex, u8 flagType);
extern void setFlag(s16 flagIndex, u8 newValue, u8 flagType);
extern void func_global_asm_805FF378(Maps nextMap, s32 nextExit);
extern void func_global_asm_80712524(Maps newMap, s32 cutsceneIndex);
extern Maps current_map;
extern u8 game_mode;

// @recomp: Level Intro Story Skip reads
RECOMP_PATCH s32 func_global_asm_8068ABE0(s16 arg0) {
    s16 cutsceneIndex;
    s32 sp20;
    s32 found;
    s32 i;
    Maps map;
    s32 exit; // 20

    cutsceneIndex = -1;
    i = 0, found = 0;
    while (!found && i < 8) {
        if (current_map == D_global_asm_80744734[i]) {
            found = 1;
        } else {
            i++;
        }
    }
    if (found) {
        if (!isFlagSet(PERMFLAG_LEVEL_ENTERED_JAPES + i, FLAG_TYPE_PERMANENT)) {
            exit = 0;
            switch (arg0) {
                case 0x7:
                    map = MAP_HELM_LEVEL_INTROS_GAME_OVER;
                    cutsceneIndex = 0xF;
                    break;
                case 0x26:
                    map = MAP_HELM_LEVEL_INTROS_GAME_OVER;
                    cutsceneIndex = 0x10;
                    break;
                case 0x1A:
                    map = MAP_HELM_LEVEL_INTROS_GAME_OVER;
                    cutsceneIndex = 0x11;
                    break;
                case 0x1E:
                    map = MAP_HELM_LEVEL_INTROS_GAME_OVER;
                    cutsceneIndex = 0x12;
                    break;
                case 0x30:
                    map = MAP_HELM_LEVEL_INTROS_GAME_OVER;
                    cutsceneIndex = 0x13;
                    break;
                case 0x48:
                    map = MAP_HELM_LEVEL_INTROS_GAME_OVER;
                    cutsceneIndex = 0x14;
                    break;
                case 0x57:
                    map = MAP_HELM_LEVEL_INTROS_GAME_OVER;
                    cutsceneIndex = 0x15;
                    exit = 0x15;
                    break;
                case 0x11:
                    map = MAP_HELM_LEVEL_INTROS_GAME_OVER;
                    cutsceneIndex = 0x16;
                    break;
            }
            if (cutsceneIndex != -1) {
                setFlag(PERMFLAG_LEVEL_ENTERED_JAPES + i, TRUE, FLAG_TYPE_PERMANENT);
                //@recomp: change story_skip to the function read
                if (recomp_get_story_skip() == 2) {
                    func_global_asm_805FF378(arg0, exit);
                    return TRUE;
                } else {
                    func_global_asm_80712524(map, cutsceneIndex);
                    return TRUE;
                }
            }
        }
    }
    return FALSE;
}

extern OSTime D_global_asm_807445B8;
extern u8 D_global_asm_80755350;
extern void func_global_asm_806C9AE0(void);
extern void func_global_asm_80731030(void);
extern void func_global_asm_8060DC3C(u8 fileIndex, s32 arg1);
extern u8 current_character_index[];
extern s32 func_global_asm_8060C6B8(s32 arg0, u8 arg1, u8 arg2, u8 fileIndex);
extern u8 current_file;
extern s32 D_global_asm_80755338;
extern s32 D_global_asm_8075533C;
extern void func_global_asm_805FF4D8(Maps map, s32 exit);

void fixHelmMedalsBug(void) {
    s32 i;
    
    if (isFlagSet(0x302, FLAG_TYPE_PERMANENT)) { // BoM Shut Down
        for (i = 0; i < 5; i++) {
            setFlag(0x4B + i, TRUE, FLAG_TYPE_TEMPORARY);
        }
    }
}

// @recomp: Intro Story Story Skip Read
RECOMP_PATCH void func_global_asm_807144B8(s8 arg0) {
    Maps map;
    s32 exit;

    D_global_asm_807445B8 = osGetTime();
    D_global_asm_80755350 = 0;
    func_global_asm_806C9AE0();
    func_global_asm_80731030(); // clearTemporaryFlags()
    func_global_asm_8060DC3C(arg0, 1);
    current_character_index[0] = 0; // DK
    //@recomp: change story_skip to the function read
    if (func_global_asm_8060C6B8(0xD, 0, 0, current_file) || (recomp_get_story_skip() > 0)) {
        // Flag: Isles: Escape Cutscene
        if (isFlagSet(PERMFLAG_CUTSCENE_ISLES_FTCS, FLAG_TYPE_PERMANENT)) {
            map = MAP_DK_ISLES_OVERWORLD;
            exit = 0;
        } else {
            map = MAP_TRAINING_GROUNDS;
            exit = 1;
        }
    } else {
        D_global_asm_80755338 = 1;
        map = MAP_DK_ISLES_DK_THEATRE;
        D_global_asm_8075533C = 0;
        exit = 0;
    }
    recomp_on_file_start();
    if (func_global_asm_8060C6B8(0xD, 0, 0, current_file)) {
        recomp_on_dirty_file_start();
        fixHelmMedalsBug();
    } else {
        recomp_on_file_start();
    }
    func_global_asm_805FF4D8(map, exit); // initMapChange()
    game_mode = GAME_MODE_ADVENTURE;
}

extern s32 D_global_asm_807FBB64;
extern int gameIsInDKTVMode(void);

// @recomp: Camera Type
RECOMP_PATCH u8 func_global_asm_80621174(s32 arg0, PlayerAdditionalActorData *arg1, Actor *arg2) {
    // Formatting is off here for better manual formatting. Whitespace doesn't affect the match here.
    // clang-format off
    return (arg2->control_state == 0x5B) ||
           ((D_global_asm_807FBB64 & 0x04000004) && (arg2->unkB8 != 0.0f)) ||
           (D_global_asm_807FBB64 & 0x08000000) ||
           (arg1->unkAC & 4) ||
           ((arg1->unkAC & 8) && (arg2->unkB8 != 0.0f)) ||
           ((recomp_get_camera_type() == 1) && (arg2->unkB8 != 0.0f) && (gameIsInDKTVMode() == 0)) ||
           (arg1->unkF0_u8[3] == 0xA) ||
           (arg1->unkF0_u8[3] == 0xD) ||
           (arg1->unkF0_u8[3] == 5) ||
           (arg1->unkF0_u8[3] == 0xC) ||
           (arg1->unkFC && (arg2->unkB8 != 0.0f)) ||
           (arg2->control_state == 0x6E) ||
           (arg2->unk58 == 8);
    // clang-format on
}

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
extern PlayerAdditionalActorData *extra_player_info_pointer;
extern u8 cc_player_index;

// @recomp: Orbit Camera
RECOMP_PATCH void func_global_asm_806EA200(void) {
    s8 change = 0x2D;
    s8 require_new_input = 1;
    s8 cooldown = 0xB;
    s32 invX = 0;
    s32 invY = 0;
    s32 cam_type;

    cam_type = recomp_get_camera_type();
    if (cam_type == 2) {
        cooldown = 5;
        change = 5 * (cooldown - 2);
        require_new_input = 0;
    }
    recomp_get_analog_inverted_axes(&invX, &invY);
    if (invX) {
        change = -change;
    }
    CameraPaad *CaaD = extra_player_info_pointer->unk104->AAD_as_array[0];
    if (((D_global_asm_807FD610[cc_player_index].unk2C & L_CBUTTONS) || (!require_new_input)) && (CaaD->unkF1 < 3)) {
        CaaD->unkB0 -= change;
        CaaD->unkF1 = cooldown;
    }
}

RECOMP_PATCH void func_global_asm_806EA26C(void) {
    s8 change = 0x2D;
    s8 require_new_input = 1;
    s8 cooldown = 0xB;
    s32 invX = 0;
    s32 invY = 0;
    s32 cam_type;

    cam_type = recomp_get_camera_type();
    if (cam_type == 2) {
        cooldown = 5;
        change = 5 * (cooldown - 2);
        require_new_input = 0;
    }
    recomp_get_analog_inverted_axes(&invX, &invY);
    if (invX) {
        change = -change;
    }
    CameraPaad *CaaD = extra_player_info_pointer->unk104->AAD_as_array[0];
    if (((D_global_asm_807FD610[cc_player_index].unk2C & R_CBUTTONS) || (!require_new_input)) && (CaaD->unkF1 < 3)) {
        CaaD->unkB0 += change;
        CaaD->unkF1 = cooldown;
    }
}

// extern Actor *gCurrentPlayer;

// u8 readMouseForCamera(void) {
//     if (recomp_get_camera_type() != 3) {
//         return FALSE;
//     }
//     if (gameIsInDKTVMode()) {
//         return FALSE;
//     }
//     if ((gCurrentPlayer->control_state > 1) && (gCurrentPlayer->control_state < 6)) {
//         return FALSE;
//     }
//     return TRUE;
// }

// void func_global_asm_8062217C(Actor*, s16);
// RECOMP_PATCH void func_global_asm_8061D1FC(Actor* arg0) {
//     CameraPaad* CaaD;
//     f32 temp_f2;
//     f32 var_f0;
//     f32 var_f14;
//     f32 dMouseX, dMouseY;
//     f32 opp, adj, ang, dTheta, cosd, sind;

//     CaaD = arg0->CaaD;
//     temp_f2 = D_global_asm_807FD610[CaaD->unkFB].unk2F * 0.5;
//     if (!(CaaD->unkAC & 0x2000) && !(D_global_asm_807FBB64 & 0x40)) {
//         if ((CaaD->unk8) && ((s32) CaaD->unk8 < 0x9B) && (CaaD->unkEF)) {
//             if (( CaaD->unkA == 0) || ((s32)  CaaD->unkA >= 0x9B)) {
//                 CaaD->unkF0 = CaaD->unkEE;
//             }
//             func_global_asm_8062217C(arg0, 1);
//             CaaD->unkAC |= 8;
//         } else {
//             if ((CaaD->unkA) && ((s32) CaaD->unkA < 0x9B)) {
//                 if (CaaD->unkEF) {
//                     if ((CaaD->unkF0) && (CaaD->unkEF != CaaD->unkF0)) {
//                         func_global_asm_8062217C(arg0, CaaD->unkF0);
//                     }
//                     CaaD->unkAC &= ~8;
//                 }
//             }
//         }
//     }
//     if (temp_f2 > 20.0f) {
//         var_f14 = temp_f2;
//     } else {
//         if (temp_f2 < -20.0f) {
//             var_f0 = temp_f2;
//         } else {
//             var_f0 = 0.0f;
//         }
//         var_f14 = var_f0;
//     }
//     CaaD->unk98 = ((var_f14 - CaaD->unk98) * 0.05) + CaaD->unk98;
//     CaaD->unkA0 += ((CaaD->unkA4 - CaaD->unkA0) * 0.2);
//     if (CaaD->unkAC & 0x10) {
//         arg0->distance_from_floor += ((300.0f - arg0->distance_from_floor) * 0.2);
//     } else {
//         arg0->distance_from_floor += ((CaaD->unkB8 - arg0->distance_from_floor) * 0.2);
//     }
//     if (readMouseForCamera()) {
//         recomp_get_mouse_deltas(&dMouseX, &dMouseY);
//         if (CaaD->unkF3 != 2) {
//             if (dMouseX != 0.0f) {
//                 CaaD->unkB0 = 0;
//                 CaaD->unkB0 += (dMouseX / 12.0f) * (4096.0f / 360.0f);
//             }
//         }
//     }
//     if ((CaaD->unkB0) && (CaaD->unkF3 != 2)) {
//         if (CaaD->unkB0 > 0) {
//             CaaD->unkB0 -= 9;
//             CaaD->unkB2 += 0x66;
//             if (CaaD->unkB0 < 0) {
//                 CaaD->unkB0 = 0;
//             }
//         } else {
//             CaaD->unkB0 += 9;
//             CaaD->unkB2 -= 0x66;
//             if (CaaD->unkB0 > 0) {
//                 CaaD->unkB0 = 0;
//             }
//         }
//     } else if (CaaD->unkF3 == 2) {
//         CaaD->unkB0 = 0;
//         CaaD->unkF1 = 0;
//     }
// }

extern Actor *gCurrentActorPointer;
extern s16 D_global_asm_80753B34[];
extern s16 D_global_asm_80753B44[];
extern s16 D_global_asm_807FD584;
void func_global_asm_806DF494(s16 *arg0, s16 arg1, s16 arg2);
// @recomp: Swimming controls
RECOMP_PATCH void func_global_asm_806E6B98(void) {
    s16 phi_v0;
    s8 pad;
    s16 rot;
    s16 phi_a2;
    s8 stick_x, stick_y;
    s32 invX = 0;
    s32 invY = 0;

    phi_v0 = gCurrentActorPointer->y_rotation;
    phi_a2 = 0x10;
    stick_x = D_global_asm_807FD610[cc_player_index].unk2E;
    stick_y = D_global_asm_807FD610[cc_player_index].unk2F;
    recomp_get_swimming_inverted_axes(&invX, &invY);
    if (invX) stick_x = -stick_x;
    if (!invY) stick_y = -stick_y;
    if (D_global_asm_807FD610[cc_player_index].unk30 != 0) {
        if (current_character_index[cc_player_index] == 7) {
            phi_v0 -= (stick_x * 0.8);
        } else {
            phi_v0 -= (stick_x * 0.5);
        }
        gCurrentActorPointer->y_rotation = phi_v0 & 0xFFF;
        if (stick_x >= 0) {
            phi_v0 = 1;
        } else {
            phi_v0 = -1;
        }
        rot = phi_v0;
        rot *= D_global_asm_80753B34[D_global_asm_807FD584];
        rot &= 0xFFF;
    } else {
        rot = 0;
    }
    func_global_asm_806DF494(&gCurrentActorPointer->x_rotation, rot, phi_a2);
    rot = ((D_global_asm_80753B44[D_global_asm_807FD584] * stick_y) / 70);
    rot &= 0xFFF;
    if (current_character_index[cc_player_index] == 7) {
        phi_a2 = (ABS(0.5 * stick_y)) + 16.0;
    }
    func_global_asm_806DF494(&gCurrentActorPointer->z_rotation, rot, phi_a2);
}

f32 func_global_asm_806EA2D8(void);

// @recomp: First person controls
RECOMP_PATCH void func_global_asm_806EA628(void) {
    PlayerAdditionalActorData *temp_a0;
    s32 pad;
    f32 *temp_v0;
    s16 *temp_v1;
    s32 stick_x, stick_y;
    s32 invX = 0;
    s32 invY = 0;
    f32 dGyroX, dGyroY, dMouseX, dMouseY;

    if (!(extra_player_info_pointer->unk1F0 & 0x8000)) {
        stick_x = D_global_asm_807FD610[cc_player_index].unk2E;
        stick_y = D_global_asm_807FD610[cc_player_index].unk2F;
        recomp_get_first_person_inverted_axes(&invX, &invY);
        recomp_get_mouse_deltas(&dMouseX, &dMouseY);
        recomp_get_gyro_deltas(&dGyroY, &dGyroX);
        if (invX) stick_x = -stick_x;
        if (!invY) stick_y = -stick_y;
        if (stick_x == 0) {
            if (dGyroX != 0.0f) {
                stick_x = dGyroX;
            } else if (dMouseX != 0.0f) {
                stick_x = dMouseX;
            }
        }
        if (stick_y == 0) {
            if (dGyroY != 0.0f) {
                stick_y = dGyroY;
            } else if (dMouseY != 0.0f) {
                stick_y = dMouseY;
            }
        }
        temp_a0 = extra_player_info_pointer->unk104->additional_actor_data;
        temp_v1 = &temp_a0->unkB2;
        *temp_v1 -= (stick_x * 0.08 * func_global_asm_806EA2D8() * 4096.0) / 360.0;
        temp_v0 = &temp_a0->unkB8;
        *temp_v0 += stick_y * 0.04 * func_global_asm_806EA2D8();
        *temp_v1 &= 0xFFF;
        if (temp_a0->unkBC + 0x32 < *temp_v0) {
            *temp_v0 = temp_a0->unkBC + 0x32;
        } else if (*temp_v0 < temp_a0->unkBC - 0x50) {
            *temp_v0 = temp_a0->unkBC - 0x50;
        }
        extra_player_info_pointer->unk104->distance_from_floor = *temp_v0;
        gCurrentActorPointer->y_rotation = (temp_a0->unkB2 + 0x800) & 0xFFF;
    }
}


Gfx *func_menu_8002F980(Gfx *, MenuAdditionalActorData *, u8 **, s8, s32 *, s32, f32 *, f32, s32);
s32 func_menu_800317E8(MenuAdditionalActorData *arg0, f32 arg1, f32 arg2, f32 *arg3, f32 *arg4, s32 arg5, s8 arg6, f32 arg7);
Gfx* printStyledText(Gfx* dl, s16 style, s16 x, s16 y, u8* string, u32 extraBitfield);
s16 playSound(s16 arg0, s32 arg1, f32 arg2, f32 arg3, u8 arg4, u8 arg5);
void func_menu_8002FD38(MenuAdditionalActorData*, s32, s32);
void func_menu_8002FC1C(Actor *, MenuAdditionalActorData *, s32);
void func_menu_8002FE08(MenuAdditionalActorData*, s32);
void func_global_asm_8060DEA8(void);
extern s8 D_global_asm_8074583C;
extern s8 D_global_asm_80745840;
extern s8 D_global_asm_80745844;
extern s16 D_menu_80033674;
extern u32 global_properties_bitfield;
extern u8 **label_string_pointer_array;

// @recomp: Sound DL
RECOMP_PATCH Gfx *func_menu_8002D520(Actor *arg0, Gfx *dl) {
    MenuAdditionalActorData *MaaD = arg0->MaaD;
    s32 sp60;
    s8 pad[4];
    f32 sp58;
    f32 sp54;
    f32 sp50;
    s16 pad4;
    s16 temp_f10;
    s8 pad3[2];
    s8 var_v0;
    char sp40[4];

    global_properties_bitfield &= ~0x10;
    func_menu_800317E8(MaaD, 160.0f, 20.0f, &sp58, &sp54, 2, 0, 1.5f);
    dl = printStyledText(dl, 1, 0x280, sp54 * 4.0f, label_string_pointer_array[32], 0x81U);
    dl = func_menu_8002F980(dl, MaaD, &label_string_pointer_array[33], 4, &sp60, 1, &sp50, 80.0f, -1);
    D_menu_80033674 = 0x3E8;
    func_menu_800317E8(MaaD, 160.0f, 150.0f, &sp58, &sp54, 2, 0, 1.5f);
    temp_f10 = sp54 * 4.0f;
    sp58 = (sp58 - sp50) * 4.0f;
    switch (sp60) {
        case 0:
            dl = printStyledText(dl, 1, sp58, temp_f10, label_string_pointer_array[D_global_asm_80745844 + 37], 0x81U);
            break;
        // case 1:
        // case 2:
        //     dl = printStyledText(dl, 1, sp58, temp_f10, label_string_pointer_array[40], 0x81U);
        //     var_v0 = sp60 == 1 ? D_global_asm_8074583C : D_global_asm_80745840;
        //     _sprintf(sp40, "%d", (var_v0 + 1) >> 1);
        //     dl = printStyledText(dl, 1, sp58, temp_f10 + 0x78, &sp40, 0x81U);
        //     break;
        // case 3:
        //     D_menu_80033674 = 0;
        //     break;
    }
    return dl;
}

extern s16 D_menu_80033670;
extern s8 D_global_asm_8074583C;
extern s8 D_global_asm_80745840;
extern s8 D_global_asm_80745844;

void func_global_asm_8060A398(s32);
void func_menu_80027E10(void);
void func_menu_8002907C(void);
void func_global_asm_8060C648(s32 arg0, u8 arg1, u8 arg2, u8 fileIndex, u32 arg4);

// @recomp: Sound screen code
RECOMP_PATCH void func_menu_8002CFA4(Actor *arg0, s32 arg1) {
    MenuAdditionalActorData *MaaD = arg0->MaaD;
    s8 sp23 = FALSE;

    func_menu_8002907C();
    if (MaaD->unk0 == 0.0f) {
        if (MaaD->unk4 == 0.0f) {
            switch (MaaD->unk17) {
                case 0:
                    if (!(D_menu_80033670 & 0x10) && (arg1 & 0x10)) {
                        D_global_asm_80745844++;
                        if (D_global_asm_80745844 > 2) {
                            D_global_asm_80745844 = 0;
                        }
                        sp23 = TRUE;
                    } else if (!(D_menu_80033670 & 0x20) && (arg1 & 0x20)) {
                        D_global_asm_80745844--;
                        if (D_global_asm_80745844 < 0) {
                            D_global_asm_80745844 = 2;
                        }
                        sp23 = TRUE;
                    }
                    if (sp23) {
                        func_menu_80027E10();
                    }
                    break;
                // case 1:
                //     if (arg1 & 0x10) {
                //         D_global_asm_8074583C++;
                //         playSound(0x2A0, 0x7FFF, 63.0f, 1.0f, 0, 0);
                //         if (D_global_asm_8074583C > 0x28) {
                //             D_global_asm_8074583C = 0x28;
                //         }
                //     } else if (arg1 & 0x20) {
                //         D_global_asm_8074583C--;
                //         playSound(0x2A0, 0x7FFF, 63.0f, 1.0f, 0, 0);
                //         if (D_global_asm_8074583C < 0) {
                //             D_global_asm_8074583C = 0;
                //         }
                //     }
                //     func_global_asm_80737B58(0, ((D_global_asm_8074583C * 0x61A8) / 40));
                //     func_global_asm_80737B58(1, ((D_global_asm_8074583C * 0x61A8) / 40));
                //     func_global_asm_80737B58(2, ((D_global_asm_8074583C * 0x61A8) / 40));
                //     func_global_asm_80737B58(3, ((D_global_asm_8074583C * 0x61A8) / 40));
                //     break;
                // case 2:
                //     if (arg1 & 0x10) {
                //         D_global_asm_80745840++;
                //         if (D_global_asm_80745840 >= 0x29) {
                //             D_global_asm_80745840 = 0x28;
                //         }
                //     } else if (arg1 & 0x20) {
                //         D_global_asm_80745840--;
                //         if (D_global_asm_80745840 < 0) {
                //             D_global_asm_80745840 = 0;
                //         }
                //     }
                //     func_global_asm_8060A398(0);
                //     func_global_asm_8060A398(2);
                //     break;
                // case 3:
                //     if (arg1 & 0x100) {
                //         D_global_asm_80745844 = 0;
                //         D_global_asm_8074583C = 0x28;
                //         D_global_asm_80745840 = 0x28;
                //         func_global_asm_80737B58(0, ((D_global_asm_8074583C * 0x61A8) / 40));
                //         func_global_asm_80737B58(1, ((D_global_asm_8074583C * 0x61A8) / 40));
                //         func_global_asm_80737B58(2, ((D_global_asm_8074583C * 0x61A8) / 40));
                //         func_global_asm_80737B58(3, ((D_global_asm_8074583C * 0x61A8) / 40));
                //         func_global_asm_8060A398(0);
                //         func_global_asm_8060A398(2);
                //         sp23 = 1;
                //         func_menu_80027E10();
                //     }
                //     break;
            }
            if (arg1 & 2) {
                if (func_global_asm_8060C6B8(0x1E, 0, 0, 0) != D_global_asm_80745844) {
                    func_global_asm_8060C648(0x1E, 0, 0, 0, D_global_asm_80745844);
                    func_global_asm_8060DEA8();
                }
                playSound(0x2C9, 0x7FFF, 63.0f, 1.0f, 0, 0);
                MaaD->unk16 = 0;
                MaaD->unk13 = 1;
            } else {
                func_menu_8002FD38(MaaD, 1, arg1);  // @recomp: Change to 1 option
            }
        }
        func_menu_8002FE08(MaaD, 1);  // @recomp: Change to 1 option
    }
    func_menu_8002FC1C(arg0, MaaD, 1);
    if (sp23) {
        playSound(0x74, 0x7FFF, 63.0f, 1.0f, 0, 0);
    }
}

extern s16 D_menu_80033674;
void func_global_asm_8060C8AC(u8 arg0);

// @recomp: Options DL
RECOMP_PATCH Gfx *func_menu_8002DBDC(Actor *arg0, Gfx *dl) {
    MenuAdditionalActorData *aaD;
    u32 sp68;
    f32 x;
    f32 sp60;
    f32 sp5C;
    s32 pad6;
    s16 y;
    s32 pad;
    s32 pad2;
    s32 pad3;
    s32 pad4;
    s32 pad5;
    s32 pad7;

    aaD = arg0->additional_actor_data;
    global_properties_bitfield &= ~0x10;
    func_menu_800317E8(aaD, 160.0f, 20.0f, &x, &sp60, 1, 0, 1.5f);
    dl = printStyledText(dl, 1, 640, sp60 * 4.0f, label_string_pointer_array[41], 0x81);
    dl = func_menu_8002F980(dl, aaD, &label_string_pointer_array[44], 1, &sp68, 1, &sp5C, 80.0f, -1);
    D_menu_80033674 = 1000;
    func_menu_800317E8(aaD, 160.0f, 150.0f, &x, &sp60, 2, 0, 1.5f);
    y = sp60 * 4.0f;
    x = (x - sp5C) * 4.0f;
    switch (sp68) {
        // case 0:
        //     dl = printStyledText(dl, 1, x, y + 64, label_string_pointer_array[47 + widescreen_enabled], 0x81);
        //     break;
        // case 1:
        //     dl = printStyledText(dl, 1, x, y + 64, label_string_pointer_array[49 + story_skip], 0x81);
        //     break;
        case 0:  // @recomp: Isolate to just resetting highscores
            D_menu_80033674 = 0;
            break;
        // case 3:
        //     dl = printStyledText(dl, 1, x, y + 64, label_string_pointer_array[52 + D_global_asm_80744530], 0x81);
        //     break;
        // case 4:
        //     dl = printStyledText(dl, 1, x, y + 64, label_string_pointer_array[51], 0x81);
        //     break;
    }
    return dl;
}

// @recomp: Options Code
extern u8 D_global_asm_80744530;
extern s16 D_global_asm_80744544;
extern u8 D_global_asm_807550C8;

RECOMP_PATCH void func_menu_8002D8AC(Actor *arg0, s32 arg1) {
    MenuAdditionalActorData *aaD;
    s8 optionChanged;
    s32 i;

    aaD = arg0->MaaD;
    optionChanged = FALSE;
    if (aaD->unk0 == 0.0f) {
        if (aaD->unk4 == 0.0f) {
            switch (aaD->unk17) {
                // case 0:
                //     if ((arg1 & 0x30) && !(D_menu_80033670 & 0x30)) {
                //         widescreen_enabled = 1 - widescreen_enabled;
                //         optionChanged = TRUE;
                //     }
                //     break;
                // case 1:
                //     if ((arg1 & 0x30) && !(D_menu_80033670 & 0x30)) {
                //         story_skip = 1 - story_skip;
                //         optionChanged = TRUE;
                //     }
                //     break;
                case 0:
                    if (arg1 & 0x100) {
                        func_global_asm_8060C8AC(0xFF);
                        playSound(0x23C, 0x7FFF, 63.0f, 1.0f, 0, 0);
                    }
                    break;
                // case 3:
                //     if ((arg1 & 0x30) && !(D_menu_80033670 & 0x30)) {
                //         D_global_asm_80744530 = 1 - D_global_asm_80744530;
                //         optionChanged = TRUE;
                //     }
                //     break;
                // case 4:
                //     if ((arg1 & 0x10) && !(D_menu_80033670 & 0x10)) {
                //         D_global_asm_807550C8++;
                //         optionChanged = TRUE;
                //     }
                //     if ((arg1 & 0x20) && !(D_menu_80033670 & 0x20)) {
                //         D_global_asm_807550C8--;
                //         optionChanged = TRUE;
                //     }
                //     D_global_asm_807550C8 &= 3;
                //     if (optionChanged) {
                //         for (i = 0; i < 101; i++) {
                //             func_global_asm_8061134C((label_string_pointer_array[i]));
                //         }
                //         func_global_asm_8061134C(label_string_pointer_array);
                //         func_menu_800324CC();
                //     }
                //     break;
            }
            if (arg1 & 2) {
                func_global_asm_8060C648(0x1F, 0, 0, 0, D_global_asm_807550C8);
                func_global_asm_8060C648(0x20, 0, 0, 0, D_global_asm_80744530);
                func_global_asm_8060DEA8();
                playSound(0x2C9, 0x7FFF, 63.0f, 1.0f, 0, 0);
                aaD->unk16 = 0;
                aaD->unk13 = 1;
            } else {
                func_menu_8002FD38(aaD, 1, arg1);
            }
        }
        func_menu_8002FE08(aaD, 1);
    }
    func_menu_8002FC1C(arg0, aaD, 1);
    // if (newly_pressed_input_copy & (U_CBUTTONS | D_CBUTTONS)) {
    //     D_global_asm_80744544 ^= 0x100;
    // }
    if (optionChanged) {
        playSound(0x74, 0x7FFF, 63.0f, 1.0f, 0, 0);
    }
}

u8 temporary_flag_block[0xE];
u8 *getFlagBlockAddress(u8 flagType);
void func_global_asm_807311C4(s16 index);
RECOMP_PATCH u8 isFlagSet(s16 flagIndex, u8 flagType) { // TODO: Can we use the FlagTypes enum? Needs to be a u8 to match
    u8 *flagBlock;
    s16 flagByte;
    
    if ((flagIndex == 0x1C) && (flagType == FLAG_TYPE_GLOBAL)) {
        // Multiplayer enabled global flag
        if (!recomp_get_mp_enabled()) {
            return FALSE;
        }
    }
    recomp_on_flag_check(&flagIndex, &flagType);
    if (flagIndex == -1) {
        return 0;
    }
    switch (flagType) {
        case FLAG_TYPE_PERMANENT:
        case FLAG_TYPE_GLOBAL:
            flagBlock = getFlagBlockAddress(flagType);
            break;
        case FLAG_TYPE_TEMPORARY:
            flagBlock = temporary_flag_block;
    }
    flagByte = flagIndex >> 3;
    return flagBlock[flagByte] >> (s16)(flagIndex - flagByte * 8) & 1;
}

RECOMP_PATCH void setFlag(s16 flagIndex, u8 newValue, u8 flagType) {
    u8 *flagBlock;
    s16 flagByte;
    s32 sp2C; // This is load bearing, cannot remove

    recomp_on_flag_change(&flagIndex, &newValue, &flagType);
    if (flagIndex != -1) {
        switch (flagType) {
            case FLAG_TYPE_PERMANENT:
            case FLAG_TYPE_GLOBAL:
                flagBlock = getFlagBlockAddress(flagType);
                break;
            case FLAG_TYPE_TEMPORARY:
                flagBlock = temporary_flag_block;
                break;
        }
        flagByte = flagIndex >> 3;
        if (newValue) {
            flagBlock[flagByte] |= 1 << (s16)(flagIndex - (flagByte * 8));
        } else {
            flagBlock[flagByte] &= ~(1 << (s16)(flagIndex - (flagByte * 8)));
        }
        if (newValue && (flagType == FLAG_TYPE_PERMANENT)) {
            func_global_asm_807311C4(flagIndex);
        }
    }
}

RECOMP_PATCH void func_global_asm_80731030(void) { // clearTemporaryFlags()
    s32 flagIndex;
    
    for (flagIndex = 0; flagIndex != 0xE; flagIndex++) {
        temporary_flag_block[flagIndex] = 0;
    }
}