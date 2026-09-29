#include "common_structs.h"
#include "options.h"

void func_global_asm_805FF9AC(Maps, s32, s32, s16);
void func_global_asm_80608DA8(s32, u8, s32, s32, s32);
void func_global_asm_80659F7C(f32, f32, f32, f32, s32);
void func_global_asm_80711BD0(f32, s16, s16, u8);
void func_global_asm_80659620(f32 *arg0, f32 *arg1, f32 *arg2, s16 arg3);
void func_global_asm_80704AFC(f32 arg0, f32 arg1, f32 arg2);
void setIntroStoryPlaying(u8 arg0);
void func_global_asm_805FF158(u8 arg0);
s32 func_global_asm_8068ABE0(s16 arg0);
void func_global_asm_805FF378(Maps nextMap, s32 nextExit);
void func_global_asm_805FF4D8(Maps map, s32 exit);
void func_global_asm_805FF628(Maps map, s32 exit);
void func_global_asm_806F3BEC(Actor *arg0, s16 arg1, s16 arg2, u8 arg3);
void func_global_asm_806C92C4(s32 arg0);
void func_global_asm_805FF660(u8 arg0);
void func_global_asm_805FF898(void);
void func_global_asm_805FF8F8(void);
u8 getLevelIndex(u8 map, u8 arg1);
void func_global_asm_806F397C(Actor *arg0, Actor *arg1, s16 arg2, s16 arg3);
void playSoundAtActorPosition(Actor *arg0, s16 arg1, u8 arg2, s16 arg3, u8 arg4);
void playSong(MUSIC_E arg0, f32 arg1);
void func_global_asm_80602B60(s32 arg0, u8 arg1);
void func_global_asm_80721560(s16 arg0, s16 arg1, s16 arg2, u8 arg3, u8 arg4, u8 arg5);
void func_global_asm_807215AC(s8 arg0, s8 arg1, s8 arg2);
void func_global_asm_80600044(s32 arg0);
void func_global_asm_80653F68(s16 index);
void func_global_asm_80659DB0(f32 arg0, f32 arg1, f32 arg2, s16 arg3);
void func_global_asm_80711950(f32, s16, s16);
void func_global_asm_80711F90(f32 arg0, s16 arg1, f32 arg2, s16 arg3, f32 arg4);
void func_global_asm_80659670(f32 arg0, f32 arg1, f32 arg2, s16 arg3);
void func_global_asm_80711410(f32 arg0, s16 arg1, f32 arg2, s16 arg3, f32 arg4);
s32 func_global_asm_807122B4(void);
void func_global_asm_8066466C(void);
void func_global_asm_80664D20(void);
extern u16 D_global_asm_8076A0A6;
extern s16 D_global_asm_807FDCB8;
extern s16 D_global_asm_807FDCBC;
extern u16 D_global_asm_80744700[];
extern u8 D_global_asm_80750190;
extern u8 D_global_asm_807FBDC4;
extern Actor *gCurrentActorPointer;
extern Actor *gPlayerPointer;
extern Actor *gCurrentPlayer;
extern GlobalASMStruct35 D_global_asm_807FBB70;
extern Maps current_map;
extern s32 current_exit;
extern PlayerAdditionalActorData *extra_player_info_pointer;
extern u32 global_properties_bitfield;
extern u8 D_global_asm_807FC620;
extern u8 D_global_asm_807FC621;
extern u8 D_global_asm_807FC622;
extern u16 newly_pressed_input[];

typedef struct LZControllerAAD {
    u8 unk0;
    u8 pad1[0x4 - 0x01];
    f32 unk4;
    s16 unk8;
    u8 padA[2];
    f32 unkC;
    f32 unk10;
    f32 unk14;
    u8 unk18;
    u8 unk19;
    s16 unk1A;
    s16 unk1C;
    s16 unk1E;
    u8 unk20;
    u8 unk21;
} LZControllerAAD;

extern f32 *D_global_asm_8076A0C0;
extern f32 *D_global_asm_8076A0C4;
extern f32 *D_global_asm_8076A0C8;
extern f32 *D_global_asm_8076A0B4;
extern f32 *D_global_asm_8076A0B8;
extern f32 *D_global_asm_8076A0BC;
extern s32 D_global_asm_807F6C28;

RECOMP_PATCH void func_global_asm_8068AD7C(void) {
    Struct807FBB70_unk278 *temp_s0;
    s32 i; // 60
    s32 var_s0;
    s32 var_v0;
    LZControllerAAD *TaaD;
    f32 temp;
    s32 frequency;

    TaaD = gCurrentActorPointer->AAD_as_array[0];
    if (ACTOR_UNINITIALIZED(gCurrentActorPointer)) {
        TaaD->unk0 = 0U;
        TaaD->unk1A = -1;
        TaaD->unk21 = 0U;
        func_global_asm_80659620(&TaaD->unkC, &TaaD->unk10, &TaaD->unk14, 0);
    }
    for (i = 0; i < D_global_asm_807FBDC4; i++) {
        // if (i);
        temp_s0 = D_global_asm_807FBB70.unk278[i];
        switch (D_global_asm_807FBB70.unk258[i]) {
        case 0xFF:
            TaaD->unk1E = 0;
            TaaD->unk0 = 0xFF;
            break;
        case 0xF:
            func_global_asm_80704AFC(
                temp_s0->unk0 / 255.0,
                temp_s0->unk2 / 255.0,
                temp_s0->unk4 / 255.0
            );
            break;
        case 0xE:
            TaaD->unkC = temp_s0->unk0 / 255.0;
            TaaD->unk10 = temp_s0->unk2 / 255.0;
            TaaD->unk14 = temp_s0->unk4 / 255.0;
            TaaD->unk1A = temp_s0->unk6;
            break;
        case 0x10:
            TaaD->unk1C = temp_s0->unk6;
            TaaD->unk1E = TaaD->unk1C;
            break;
        case 0x1:
            if ((current_map != MAP_FUNGI) || !(extra_player_info_pointer->unk1F0 & 0x200000)) {
                TaaD->unk0 = temp_s0->unk0;
                TaaD->unk1A = temp_s0->unk2;
                TaaD->unk4 = (f32)(temp_s0->unk4) / 100.0;
                TaaD->unk8 = temp_s0->unk6;
                break;
            }
            break;
        case 0x2:
            if ((current_map != MAP_FUNGI) || !(extra_player_info_pointer->unk1F0 & 0x200000)) {
                TaaD->unk0 = 0xFF;
                break;
            }
            break;
        case 0x3:
            if (!(global_properties_bitfield & 0x400)) {
                if (temp_s0->unk4) {
                    setIntroStoryPlaying(2U);
                    func_global_asm_805FF158(0U);
                }
                if (temp_s0->unk0 == MAP_HELM) {
                    if (func_global_asm_8068ABE0(temp_s0->unk0) == 0) {
                        func_global_asm_805FF378(temp_s0->unk0, temp_s0->unk2);
                    }
                } else {
                    func_global_asm_805FF378(temp_s0->unk0, temp_s0->unk2);
                }
                func_global_asm_806F3BEC(gPlayerPointer, D_global_asm_807FDCB8, D_global_asm_807FDCBC, 0x46U);
                break;
            }
            break;
        case 0x15:
            if (!(global_properties_bitfield & 0x400)) {
                func_global_asm_805FF4D8(temp_s0->unk0, temp_s0->unk2);
                break;
            }
            break;
        case 0x4:
            if (!(global_properties_bitfield & 0x400)) {
                switch (temp_s0->unk4) {
                    case 1:
                        setIntroStoryPlaying(2U);
                    default:
                        func_global_asm_805FF158(0U);
                        break;
                    case 0:
                        break;
                }
                func_global_asm_805FF9AC(temp_s0->unk0, temp_s0->unk2, 0, 0);
                func_global_asm_806F3BEC(gPlayerPointer, D_global_asm_807FDCB8, D_global_asm_807FDCBC, 0x46U);
                break;
            }
            break;
        case 0x5:
            if (!(global_properties_bitfield & 0x400)) {
                func_global_asm_805FF628(temp_s0->unk0, temp_s0->unk2);
                func_global_asm_806F3BEC(gPlayerPointer, D_global_asm_807FDCB8, D_global_asm_807FDCBC, 0x46U);
                break;
            }
            break;
        case 0x6:
            extra_player_info_pointer->unk100 = 1;
            func_global_asm_806C92C4(temp_s0->unk0);
            break;
        case 0x7:
            if (!(global_properties_bitfield & 0x400)) {
                func_global_asm_805FF9AC(temp_s0->unk0, temp_s0->unk2, temp_s0->unk4, 2);
                func_global_asm_806F3BEC(gPlayerPointer, D_global_asm_807FDCB8, D_global_asm_807FDCBC, 0x46U);
                break;
            }
            break;
        case 0x8:
            if (!(global_properties_bitfield & 0x400)) {
                func_global_asm_805FF898();
                func_global_asm_806F3BEC(gPlayerPointer, D_global_asm_807FDCB8, D_global_asm_807FDCBC, 0x46U);
                break;
            }
            break;
        case 0x19:
            func_global_asm_805FF8F8();
            break;
        case 0x9:
            func_global_asm_805FF660(temp_s0->unk0);
            break;
        case 0xA:
            character_change_array->playerPointer->PaaD->unk1F0 |= 0x40000800;
            break;
        case 0xB:
            func_global_asm_805FF378(D_global_asm_80744700[getLevelIndex(current_map, 0U)], current_exit);
            func_global_asm_806F3BEC(gPlayerPointer, D_global_asm_807FDCB8, D_global_asm_807FDCBC, 0x46U);
            break;
        case 0xC:
            var_v0 = temp_s0->unk2 ? 0 : 0x8000;
            func_global_asm_806F397C(gPlayerPointer, NULL, temp_s0->unk0 + var_v0, 0);
            break;
        case 0xD:
            if (gPlayerPointer->unkB8 > 250.0f) {
                playSoundAtActorPosition(gPlayerPointer, temp_s0->unk0, 0xFFU, 0x7F, 0x1EU);
                break;
            }
            break;
        case 0x16:
            playSong(temp_s0->unk0, 1.0f);
            break;
        case 0x17:
            func_global_asm_80602B60(temp_s0->unk0, 0U);
            break;
        case 0x11:
            gCurrentPlayer->y_acceleration = temp_s0->unk0 / 10.0;
            break;
        case 0x12:
            gCurrentPlayer->unk6A |= 0x200;
            break;
        case 0x13:
            gCurrentPlayer->unk6A &= ~0x200;
            break;
        case 0x14:
            D_global_asm_807FC621 = temp_s0->unk0;
            D_global_asm_807FC622 = temp_s0->unk2;
            if (D_global_asm_807FC622) {
                func_global_asm_807215AC(0x35, 0x18, 0x28);
                func_global_asm_80721560(0x320, 0x82, 0, 0x64, 0x64, 0x64);
                break;
            }
            break;
        case 0x18:
            func_global_asm_80600044(getLevelIndex(current_map, 0U));
            break;
        case 0x1C:
            D_global_asm_8076A0A6 = *newly_pressed_input;
            break;
        case 0x1A:
            character_change_array->playerPointer->PaaD->unk1F0 |= 0x80000000;
            break;
        case 0x1B:
            func_global_asm_80653F68(temp_s0->unk2);
            break;
        }
    }
    frequency = recomp_get_lightning_frequency();
    if (current_map == MAP_FUNGI_MINECART) {
        frequency = 1;
    }
    if (TaaD->unk1E) {
        TaaD->unk1E--;
        temp = (f32) TaaD->unk1E / (f32) TaaD->unk1C;
        func_global_asm_80659F7C(TaaD->unkC, TaaD->unk10, TaaD->unk14, temp, TaaD->unk1A);
        if (TaaD->unk1E == 0) {
            func_global_asm_80659DB0(TaaD->unkC, TaaD->unk10, TaaD->unk14, TaaD->unk1A);
        }
    }
    switch (TaaD->unk0) {
    case 0x6:
        func_global_asm_80711BD0(TaaD->unk4, TaaD->unk8, TaaD->unk1A, 4);
        break;
    case 0x7:
        func_global_asm_80711BD0(TaaD->unk4, TaaD->unk8, TaaD->unk1A, 5);
        break;
    case 0x5:
        func_global_asm_80711BD0(TaaD->unk4, TaaD->unk8, TaaD->unk1A, 3);
        break;
    case 0x1:
        func_global_asm_80711950(TaaD->unk4, TaaD->unk8, TaaD->unk1A);
        break;
    case 0x4:
        func_global_asm_80711F90(TaaD->unk4, TaaD->unk8, 1.0f, TaaD->unk1A, 4.0f);
        break;
    case 0x3:
        if (RandClamp(50) == 0xF) {
            D_global_asm_807FC620 = 1;
        }
    case 0x2:
        if (TaaD->unk21) {
            TaaD->unk21--;
        }
        if (D_global_asm_807FC620) {
            TaaD->unk18 = MIN(0x28U * frequency, 255);
            D_global_asm_807FC620 = 0;
        }
        if (TaaD->unk18) {
            if ((TaaD->unk18 == 0x28) || (RandClamp(10) == 5)) {
                if (frequency > 0) {
                    func_global_asm_80659670(1.0f, 1.0f, 1.0f, TaaD->unk1A);
                    if ((D_global_asm_80750190 == 0) && (TaaD->unk21 == 0)) {
                        var_s0 = 70;
                        if (current_map == MAP_CASTLE) {
                            var_s0 = 45;
                        }
                        func_global_asm_80608DA8(0x9C, var_s0, 0x7F, 0x1E, (RANDNUM() >> 0xF) % 3);
                        if (current_map == MAP_GALLEON_PUFFTOSS) {
                            TaaD->unk21 = MIN(80 * frequency, 255);
                        } else {
                            TaaD->unk21 = MIN(50 * frequency, 255);
                        }
                    }
                }
                TaaD->unk1E = MAX(TaaD->unk1E, 4);
                TaaD->unk1C = TaaD->unk1E;
            }
            TaaD->unk18--;
        }
        func_global_asm_80711410(TaaD->unk4, TaaD->unk8, 1.0f, TaaD->unk1A, 1.0f);
        break;
    case 0xFF:
        if (func_global_asm_807122B4()) {
            TaaD->unk0 = 0U;
        }
        break;
    }
    func_global_asm_8066466C();
    func_global_asm_80664D20();
    D_global_asm_807FC621 |= 0x80;
}

s16 playSound(s16 arg0, s32 arg1, f32 arg2, f32 arg3, u8 arg4, u8 arg5);
void func_global_asm_80608DA8(s32, u8, s32, s32, s32);
extern void *_malloc(s32);
extern f32 D_global_asm_80770DCC;
extern f32 D_global_asm_80770DD0;
extern f32 D_global_asm_80770DD4;
extern f32 D_global_asm_807480DC;
extern s8 D_global_asm_8077058C;
extern u8 D_global_asm_80770DC9;
extern f32 D_global_asm_807F621C;
extern f32 D_global_asm_807F6220;
extern f32 D_global_asm_807F6224;
extern u8 is_cutscene_active;

typedef struct InstanceData806443E4 {
    s32 unk0;
    f32 unk4;
    f32 unk8;
    f32 unkC;
} InstanceData806443E4;

// @recomp: Japes stormy area lightning
RECOMP_PATCH void func_global_asm_806443E4(Prop_ScriptData *arg0, s16 arg1, s16 arg2, s16 arg3) {
    InstanceData806443E4 *var_v1;
    f32 var_f16;
    f32 var_f14;
    f32 dx, dy, dz;
    s32 frequency;

    // fake match
    if (gPlayerPointer->PaaD) {
    }

    if (gPlayerPointer->PaaD->unk1F0 & 0x20000000) {
        return;
    }
    if (arg0->unk0 == NULL) {
        var_v1 = _malloc(0x10);
        arg0->unk0 = var_v1;
        var_v1->unk0 = 0;
        var_v1->unk4 = 0.0f;
        var_v1->unk8 = 0.0f;
        var_v1->unkC = 0.0f;
    }
    frequency = recomp_get_lightning_frequency();
    if (frequency == 0) {
        func_global_asm_80659670(0.4f, 0.3f, 0.3f, 0xE); // @recomp: Lock lighting
        return;
    }
    var_v1 = arg0->unk0;
    dz = character_change_array->look_at_eye_z - D_global_asm_807F6224;
    dx = character_change_array->look_at_eye_x - D_global_asm_807F621C;
    dy = character_change_array->look_at_eye_y - D_global_asm_807F6220;
    var_f16 = _sqrtf(SQ(dz) + (SQ(dx) + SQ(dy)));
    
    if ((character_change_array->chunk == 0xE) && (is_cutscene_active != 1)) {
        var_f14 = 1.0f;
    } else if (character_change_array->chunk == 7) {
        var_f14 = 0.0f;
    } else {
        var_f16 -= _sqrtf(SQ(D_global_asm_807F621C - 1714.0f) + SQ(D_global_asm_807F6220 - 226.0f) + SQ(D_global_asm_807F6224 - 3410.0f));
        if (var_f16 < 0.0) {
            var_f16 = 0.0f;
        }
        if (var_f16 > 600.0f) {
            var_f14 = 0.0f;
        } else {
            var_f14 = 1.0 - (var_f16 / 600.0f);
        }
    }
    if ((character_change_array->chunk == 0xE) || ((character_change_array->chunk == 0xB) && (var_f16 < D_global_asm_807480DC))) {
        func_global_asm_80711410(2.9f, -0x1E, var_f14, 0xE, MAX(0.05, var_f14));
    } else {
        D_global_asm_8077058C = 0;
    }
    if (RandClamp(50) == 0xF) {
        var_v1->unk0 = 0x28;
    }
    if (var_v1->unk0) {
        var_v1->unk0--;
        if (RandClamp(10 * frequency) == 5) {
            if (var_f16 < 2200.0f) {
                if (D_global_asm_80770DC9 != 0) {
                    if (D_global_asm_80770DD4 < 600.0f) {
                        playSound(0x9C,
                            (s32) ((((var_f14 * 32767.0f * 60.0f) / 255.0f) * (600.0 - D_global_asm_80770DD4)) / 1000.0),
                                  D_global_asm_80770DCC,
                                  0.8f,
                                  0x1E,
                                  (u32) D_global_asm_80770DD0);
                    }
                } else {
                    func_global_asm_80608DA8(0x9C, var_f14 * 60.0f, 0x7F, 0x1E, (RANDNUM() >> 0xF) % 3);
                }
            }
            var_v1->unk4 = 1.0f;
            var_v1->unk8 = 1.0f;
            var_v1->unkC = 1.0f;
        }
    }
    var_v1->unk4 = ((0.4 - var_v1->unk4) * 0.2) + var_v1->unk4;
    var_v1->unk8 = ((0.3 - var_v1->unk8) * 0.2) + var_v1->unk8;
    var_v1->unkC = ((0.3 - var_v1->unkC) * 0.2) + var_v1->unkC;
    func_global_asm_80659670(var_v1->unk4, var_v1->unk8, var_v1->unkC, 0xE);
}
