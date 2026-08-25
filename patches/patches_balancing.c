#include "common_structs.h"

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

typedef struct enemy_info {
    u8 enemy_type; // at 0x00
    u8 unk1;
    u16 y_rotation; // at 0x02
    s16 x_position; // at 0x04
    s16 y_position; // at 0x06
    s16 z_position; // at 0x08
    u8 cutscene_model_index; // at 0x0A
    u8 unkB;
    u32 unkC;
    u16 unk10;
    u8 unk12;
    u8 unk13;
    u32 unk14;
    Actor* unk18; // TODO: Is this accurate?
    u32 unk1C;
    Struct80027840* unk20;
    u16 unk24;
    u16 unk26;
    u16 unk28; // Used
    s16 unk2A; // Used
    u32 unk2C;
    f32 unk30; // Used
    f32 unk34; // Used
    f32 unk38; // Used
    f32 unk3C; // at 0x3C
    s16 unk40; // Used
    s16 unk42;
    u8 unk44; // Used
    u8 unk45;
    u16 unk46; // Used
} EnemyInfo;

u8 isFlagSet(s16 flagIndex, u8 flagType);
void initializeCharacterSpawnerActor(void);
void func_global_asm_80724CA4(s16 arg0, s16 arg1);
extern PlayerAdditionalActorData *extra_player_info_pointer;
extern Actor *gCurrentActorPointer;
extern EnemyInfo *D_global_asm_807FDC98;
void playActorAnimation(Actor *arg0, s32 arg1);
void func_global_asm_80728950(u8 arg0);
void func_global_asm_80684850(u8 arg0);
s32 func_global_asm_80629148(void);
void func_global_asm_8063DA40(s16 arg0, s16 arg1);
void func_global_asm_80672C30(Actor *arg0);
void func_global_asm_80726EE0(u8 arg0);
void func_global_asm_806BE674(u8 arg0);
void func_global_asm_807289B0(u8 arg0, u8 arg1);
void func_global_asm_80641874(s16 arg0, s16 arg1);
void loadText(Actor *arg0, u16 fileIndex, u8 textIndex);
void func_global_asm_8061CAD8(void);
void addActorToTextOverlayRenderArray(void *arg0, Actor *arg1, u8 arg2);
Gfx *func_global_asm_806BE6F0(Gfx *dl, Actor *arg1);
void playSong(MUSIC_E arg0, f32 arg1);
extern s32 D_global_asm_807FBB64;
void func_global_asm_8061CB08(void);
void func_global_asm_8062217C(Actor*, s16);
extern Actor *D_global_asm_807F5D10;
void func_global_asm_8072B324(Actor *arg0, s32 arg1);
extern CharacterSpawner *D_global_asm_807FDC9C;
void func_global_asm_8061F510(u8 arg0, u8 arg1);
void func_global_asm_80602B60(s32 arg0, u8 arg1);
s32 playCutscene(Actor *arg0, s16 arg1, u8 arg2);
s16 func_global_asm_80665DE0(f32 arg0, f32 arg1, f32 arg2, f32 arg3);
void func_global_asm_8070E8DC(u8 arg0);
extern Struct807FDC90 *D_global_asm_807FDC90;
void func_global_asm_806A5DF0(s16 arg0, f32 x, f32 y, f32 z, s16 arg4, u8 arg5, s16 arg6, s32 arg7);
void setFlag(s16 flagIndex, u8 newValue, u8 flagType);
void func_global_asm_8072DC7C(u8 arg0);
void func_global_asm_8072EC94(s32 arg0, u8 arg1);
void func_global_asm_806ACC00(u8 arg0);
Gfx *func_global_asm_8068E474(Gfx *dl, Actor *arg1);
extern GlobalASMStruct35 D_global_asm_807FBB70;
void func_global_asm_8072881C(s32, void*);
u8 func_global_asm_8072AB74(u8 arg0, f32 x, f32 z, u16 arg3, f32 arg4);
u8 func_global_asm_8072D13C(u8 arg0, s32 arg1);
void func_global_asm_80724E48(u8 arg0);
void func_global_asm_8072A450(void);
void renderActor(Actor *arg0, u8 arg1);
extern u32 D_global_asm_80744478;
extern Actor *gPlayerPointer;
extern u32 object_timer;
extern u8 D_global_asm_807506C0[];

RECOMP_PATCH void func_global_asm_806BE8BC(void) {
    u8 sp37;
    s16 j;
    s16 i;
    s16 var_v0;

    sp37 = isFlagSet(PERMFLAG_PROGRESS_RABBIT_RACE_1_COMPLETE, FLAG_TYPE_PERMANENT);
    initializeCharacterSpawnerActor();
    if (extra_player_info_pointer->unk1F0 & 0x100000) {
        gCurrentActorPointer->control_state = 0x40;
        return;
    }
    if (ACTOR_UNINITIALIZED(gCurrentActorPointer)) {
        func_global_asm_80724CA4(2, 1);
        D_global_asm_807FDC98->unk46 |= 0x20;
        playActorAnimation(gCurrentActorPointer, 0x306);
        func_global_asm_80728950(0);
        gCurrentActorPointer->control_state = 0x1E;
        gCurrentActorPointer->control_state_progress = 0;
        gCurrentActorPointer->unk15F = 0;
    }
    switch (gCurrentActorPointer->control_state) {
        case 0x26:
            playActorAnimation(gCurrentActorPointer, 0x307);
            gCurrentActorPointer->control_state = 0x27;
            gCurrentActorPointer->control_state_progress = 0;
            break;
        case 0x1E:
            func_global_asm_80684850(1);
            if (gCurrentActorPointer->shadow_opacity < 0xFF) {
                gCurrentActorPointer->shadow_opacity += 5;
            }
            if (func_global_asm_80629148()) {
                func_global_asm_8063DA40(0x1F, 3);
                func_global_asm_80672C30(gPlayerPointer);
                func_global_asm_80726EE0(1);
                D_global_asm_807FDC98->unk46 |= 4;
                func_global_asm_806BE674(1);
                func_global_asm_807289B0(0, 0);
                if (sp37) {
                    func_global_asm_80641874(0x17, 0x14);
                    loadText(gCurrentActorPointer, 0x14, 1);
                } else {
                    loadText(gCurrentActorPointer, 0x14, 0);
                }
                playActorAnimation(gCurrentActorPointer, 0x309);
                gCurrentActorPointer->control_state = 0x1F;
                gCurrentActorPointer->control_state_progress = 0;
            }
            break;
        case 0x1F:
            switch (gCurrentActorPointer->control_state_progress) {
                case 0:
                    if (func_global_asm_80629148()) {
                        func_global_asm_8061CAD8();
                        func_global_asm_8061CAD8();
                        gCurrentActorPointer->control_state_progress++;
                        gCurrentActorPointer->unk168 = 0x78;
                    }
                    break;
                case 1:
                    addActorToTextOverlayRenderArray(func_global_asm_806BE6F0, gCurrentActorPointer, 3);
                    break;
                case 2:
                    playSong(MUSIC_169_FUNGI_FOREST_RABBIT_RACE, 1.0f);
                    D_global_asm_807FBB64 |= 4;
                    func_global_asm_8061CB08();
                    func_global_asm_8062217C(D_global_asm_807F5D10, 3);
                    playActorAnimation(gCurrentActorPointer, 0x302);
                    func_global_asm_8072B324(gCurrentActorPointer, (sp37 ? 1.5 : 1.0) * D_global_asm_807FDC9C->unkD);
                    gCurrentActorPointer->control_state = 2;
                    gCurrentActorPointer->control_state_progress = 0;
                    func_global_asm_8061F510(1, 0xA);
                    extra_player_info_pointer->unk1F4 |= 0x40;
                    break;
            }
            break;
        case 0x27:
            if (gCurrentActorPointer->control_state_progress == 0) {
                func_global_asm_80602B60(0xA9, 0);
                gCurrentActorPointer->object_properties_bitfield &= ~0x8000;
                gCurrentActorPointer->shadow_opacity = 0xFF;
                func_global_asm_806BE674(0);
                playCutscene(gCurrentActorPointer, 0xF, 5);
                gCurrentActorPointer->y_rotation = func_global_asm_80665DE0(gPlayerPointer->x_position, gPlayerPointer->z_position, gCurrentActorPointer->x_position, gCurrentActorPointer->z_position);
                loadText(gCurrentActorPointer, 0x14, 4);
                gCurrentActorPointer->control_state = 0x37;
                gCurrentActorPointer->control_state_progress = 0;
                gCurrentActorPointer->y_position = gCurrentActorPointer->floor;
            }
            break;
        case 0x28:
            switch (gCurrentActorPointer->control_state_progress) {
                case 0:
                    func_global_asm_80602B60(0xA9, 0);
                    gCurrentActorPointer->object_properties_bitfield &= ~0x8000;
                    gCurrentActorPointer->shadow_opacity = 0xFF;
                    func_global_asm_806BE674(0);
                    playCutscene(gCurrentActorPointer, 0x1F, 1);
                    gCurrentActorPointer->y_rotation = func_global_asm_80665DE0(gPlayerPointer->x_position, gPlayerPointer->z_position, gCurrentActorPointer->x_position, gCurrentActorPointer->z_position);
                    playActorAnimation(gCurrentActorPointer, 0x308);
                    func_global_asm_8070E8DC(1);
                    if (sp37) {
                        loadText(gCurrentActorPointer, 0x14, 3);
                    } else {
                        loadText(gCurrentActorPointer, 0x14, 2);
                    }
                    gCurrentActorPointer->control_state_progress = 1;
                    gCurrentActorPointer->y_position = gCurrentActorPointer->floor;
                    break;
                case 1:
                    if (func_global_asm_80629148()) {
                        if (sp37) {
                            func_global_asm_8063DA40(0x57, 0xA);
                            D_global_asm_807FDC90->unk1A |= 0x8000;
                        } else {
                            var_v0 = gCurrentActorPointer->y_rotation - 0x12C;
                            playSong(MUSIC_47_MELON_SLICE_DROP, 1.0f);
                            for (i = 0; i < 3; i++, var_v0 += 0x12C) {
                                func_global_asm_806A5DF0(
                                    0x35,
                                    gCurrentActorPointer->x_position,
                                    gCurrentActorPointer->y_position,
                                    gCurrentActorPointer->z_position,
                                    var_v0,
                                    0x63,
                                    -1,
                                    0
                                );
                            }
                        }
                        setFlag(PERMFLAG_PROGRESS_RABBIT_RACE_1_COMPLETE, TRUE, FLAG_TYPE_PERMANENT);
                        gCurrentActorPointer->control_state = 0x37;
                        gCurrentActorPointer->control_state_progress = 0;
                        gCurrentActorPointer->y_position = gCurrentActorPointer->floor;
                    }
                    break;
            }
            break;
        case 0x37:
            if (!(gCurrentActorPointer->object_properties_bitfield & 0x02000000)) {
                func_global_asm_8072DC7C(2);
                if (gCurrentActorPointer->control_state_progress != 0) {
                    D_global_asm_807FBB64 &= ~4;
                    extra_player_info_pointer->unk23F = 2;
                    func_global_asm_8061F510(1, 0);
                    extra_player_info_pointer->unk1F4 &= ~0x40;
                    func_global_asm_8072EC94(0x16, 0);
                    func_global_asm_80726EE0(2);
                    if (D_global_asm_807FDC90->unk1A & 0x8000) {
                        gCurrentActorPointer->control_state = 0x40;
                    } else {
                        if (gCurrentActorPointer->animation_state->unk64 != 0x308) {
                            func_global_asm_806ACC00(2);
                        }
                        func_global_asm_8063DA40(0x1F, 1);
                        gCurrentActorPointer->control_state = 0x3C;
                    }
                }
            }
            break;
        case 0x13:
            playActorAnimation(gCurrentActorPointer, 0x305);
            gCurrentActorPointer->y_velocity = 200.0f;
            gCurrentActorPointer->control_state = 2;
            gCurrentActorPointer->control_state_progress = 0;
            // fallthrough
        default:
            if (gCurrentActorPointer->unk15F < 0x10U) {
                addActorToTextOverlayRenderArray(func_global_asm_8068E474, gCurrentActorPointer, 3);
                //@recomp: Change rabbit speed based on lag boost. Normal N64 Lag here is 2.1
                func_global_asm_8072B324(gCurrentActorPointer, (sp37 ? 1.5 : 1.0) * ((f32)D_global_asm_80744478 / 2.1f) * D_global_asm_807FDC9C->unkD);
            }
            if ((RandChance(0.01)) && (gCurrentActorPointer->animation_state->unk64 == 0x302)) {
                playActorAnimation(gCurrentActorPointer, (object_timer & 1) ? 0x303 : 0x304);
            }
            if (D_global_asm_807FBB70.unk0 != 0) {
                if ((D_global_asm_807FBB70.unk1 >= D_global_asm_807506C0[gCurrentActorPointer->unk15F]) && (gCurrentActorPointer->unk15F < 0x10U)) {
                    if (D_global_asm_807FBB70.unk1 != D_global_asm_807506C0[gCurrentActorPointer->unk15F]) {
                        playActorAnimation(gCurrentActorPointer, 0x301);
                        gCurrentActorPointer->control_state = 0x27;
                        gCurrentActorPointer->control_state_progress = 0;
                    } else {
                        gCurrentActorPointer->unk15F++;
                        if (gCurrentActorPointer->unk15F >= 0x10U) {
                            func_global_asm_8072EC94(0x16, 1);
                        }
                    }
                }
            }
            for (j = 0; j < D_global_asm_807FBB70.unk254; j++) {
                if (D_global_asm_807FBB70.unk258[j] == 3) {
                    gCurrentActorPointer->control_state = 0x28;
                    gCurrentActorPointer->control_state_progress = 0;
                }
            }
            func_global_asm_8072881C(0, &D_global_asm_807FDC90->unk28);
            func_global_asm_8072AB74(gCurrentActorPointer->control_state, D_global_asm_807FDC90->unkA, D_global_asm_807FDC90->unkE, 0x10, 0);
            func_global_asm_8072D13C(gCurrentActorPointer->control_state, 0);
            break;
    }
    if ((gCurrentActorPointer->animation_state->unk64 == 0x301) || (gCurrentActorPointer->animation_state->unk64 == 0x302)) {
        func_global_asm_80724E48(0);
    }
    func_global_asm_8072A450();
    renderActor(gCurrentActorPointer, 0);
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

typedef struct {
    union {
        f32 unk0;
        s32 unk0_s32;
        s32 *unk0_s32_ptr;
    };
    union {
        f32 unk4;
        s32 unk4_s32;
        s32 *unk4_s32_ptr;
    };
    union {
        f32 unk8;
        f32 *unk8_f32_ptr;
    };
} Struct807F5FD4_unk0;
typedef struct {
    Struct807F5FD4_unk0 *unk0[2];
    s32 unk8;
} Struct807F5FD4;

extern Struct807FD610 D_global_asm_807FD610[];
extern Struct807F5FD4 *D_global_asm_807F5FD4; 
void func_bonus_8002733C(void *arg0);
void func_global_asm_806A2A10(s32, s32, u8);
u8 func_global_asm_806FDB8C(s32, u8 *, s32, f32, f32, f32); 
extern void func_global_asm_806FDAB8(s16 arg0, f32 arg1);
extern u8 setAction(s16 actionIndex, Actor *actor, u8 playerIndex);
extern void func_global_asm_8061C464(Actor *arg0, Actor *arg1, u8 arg2, s16 arg3, s16 arg4, s16 arg5, s16 arg6, s16 arg7, s16 arg8, s16 arg9, f32 argA);
void func_bonus_80029E54(Actor *arg0);
extern void func_global_asm_80659670(f32 arg0, f32 arg1, f32 arg2, s16 arg3);
s16 playSound(s16 arg0, s32 arg1, f32 arg2, f32 arg3, u8 arg4, u8 arg5);
f32 func_global_asm_806119FC(void);
u8 func_bonus_80027548(f32 arg0, f32 arg1, f32 arg2);
void func_bonus_800265C0(u8 arg0, u8 textIndex);
void func_bonus_800264E0(u8 arg0, u8 textIndex);
void func_global_asm_806A2B08(Actor *arg0);
u8 *getTextString(u8 fileIndex, s32 stringIndex, s32 arg2);
extern u8 is_cutscene_active;
s16 D_bonus_8002D8EC[4] = {
    0x22C, 0x29B, 0xB9, 0xC2
};
s16 D_bonus_8002D8F4 = 50;  // @recomp: Change this from an s8 to a s16
s16 D_bonus_8002D8F8 = 0;
s16 D_bonus_8002D8FC = 0;
extern f32 D_bonus_8002DDAC;
extern f32 D_bonus_8002DDB0;
extern f32 D_bonus_8002DDB4;
extern Gfx *func_bonus_80029B9C(Gfx *dl, Actor *arg1);
extern Maps current_map;

typedef struct krazykk_aad_struct {
    s16 unk0;
    u8 unk2;
    u8 unk3;
    void *unk4[5];
    u8 unk18;
    u8 pad19[0x1E - 0x19];
    s16 unk1E;
    s16 unk20;
    u8 unk22;
    u8 unk23;
    u8 unk24;
    u8 unk25;
} krazykk_aad_struct;

typedef struct krazykk_gameinfo_struct {
    s16 unk0;
    u8 pad2[0x4-0x2];
    f32 unk4;
    s8 unk8[6];
    s16 unkE;
    u8 unk10;
    u8 unk11;
    u8 unk12;
    u8 unk13;
    s16 unk14;
    s16 unk16;
} krazykk_gameinfo_struct;

#define KRAZY_KK_SCALING 2.7f
// @recomp: Krazy KK Handler
RECOMP_PATCH void func_bonus_80029364(void) {
    krazykk_aad_struct* aad;
    krazykk_gameinfo_struct* gameinfo; // 50
    f32 var_f0_2;
    f32 var_f14;
    s32 var_a0;
    s16 temp;
    s32 i;
    s32 i_copy;
    s32 temp_v0_5;
    u8 *str;

    aad = (krazykk_aad_struct *)gCurrentActorPointer->AAD_as_array[0];
    gameinfo =(krazykk_gameinfo_struct *)gCurrentActorPointer->AAD_as_array[1];
    if ((gCurrentActorPointer->object_properties_bitfield & 0x10) == 0) {
        aad->unk23 = 5;
        aad->unk0 = -2;
        switch (current_map) {
            case MAP_KRAZY_KONG_KLAMOUR_EASY:
                gameinfo->unk16 = 10;
                D_bonus_8002D8F4 = 48 * KRAZY_KK_SCALING; // @recomp: Usually 48, changed to accompany how lag works
                break;
            case MAP_KRAZY_KONG_KLAMOUR_NORMAL:
                gameinfo->unk16 = 15;
                D_bonus_8002D8F4 = 48 * KRAZY_KK_SCALING; // @recomp: Usually 48, changed to accompany how lag works
                break;
            case MAP_KRAZY_KONG_KLAMOUR_HARD:
                gameinfo->unk16 = 5;
                D_bonus_8002D8F4 = 30 * KRAZY_KK_SCALING; // @recomp: Usually 30, changed to accompany how lag works
                break;
            case MAP_KRAZY_KONG_KLAMOUR_INSANE:
                gameinfo->unk16 = 10;
                D_bonus_8002D8F4 = 30 * KRAZY_KK_SCALING; // @recomp: Usually 30, changed to accompany how lag works
            default:
                break;
        }
        gameinfo->unk14 = gameinfo->unk16;
        gameinfo->unkE = 60;
        gameinfo->unk12 = 0;
        str = getTextString(0x1AU, 2, 1);
        gameinfo->unk11 = func_global_asm_806FDB8C(1, str, 8, 0.0f, 0.0f, 0.0f);
        func_global_asm_806FDAB8(gameinfo->unk11, 0.0f);
        gameinfo->unk0 = 60 * KRAZY_KK_SCALING; // @recomp: Usually 60, changed to accompany how lag works
        gameinfo->unk4 = 1.0f;
        setAction(0x48, NULL, 0U);
        func_global_asm_8061C464(extra_player_info_pointer->unk104, gCurrentActorPointer, 4U, 0x800, 0x1A, 0, 0x1F, 0x15, 0x10, 0, D_bonus_8002DDAC);
        playCutscene(NULL, 0, 1U);
        gCurrentActorPointer->z_rotation = 0x9A;
        gCurrentActorPointer->object_properties_bitfield |= 0x800000;
        gCurrentActorPointer->unk16A = 0xFF;
        gCurrentActorPointer->unk16B = 0xFF;
        gCurrentActorPointer->unk16C = 0xFF;
        for (i = 0; i < 6; i++) {
            gameinfo->unk8[i] = i | 0x80;
        }
        func_bonus_80029E54(gCurrentActorPointer);
    }
    if (aad->unk0 > 0) {
        if (gCurrentActorPointer->control_state == 0) {
            // @recomp: Reduce the timer by the amount of lag present
            gameinfo->unk0 -= D_global_asm_80744478;
        } else {
            gameinfo->unk0 = 0x3C;
        }
        if (gameinfo->unk0 > 0) {
            gameinfo->unk4 += D_bonus_8002DDB0;
        } else {
            gameinfo->unk4 -= D_bonus_8002DDB4;
        }
        if (gameinfo->unk4 > 1.0) {
            gameinfo->unk4 = 1.0f;
        }
        if (gameinfo->unk4 < 0.0) {
            gameinfo->unk4 = 0.0f;
        }
        func_global_asm_80659670(gameinfo->unk4, gameinfo->unk4, gameinfo->unk4, 0);
        if (gameinfo->unk4 == 0.0f) {
            temp = D_bonus_8002D8F8;
            temp--;
            D_bonus_8002D8F8 = temp;
            if (temp <= 0) {
                playSound(0x1A1, 0x7FFF, 63.0f, 1.0f, 0x50U, 0U);
                D_bonus_8002D8F8 = (func_global_asm_806119A0() & 3) + 2;
            }
            temp = D_bonus_8002D8FC;
            temp++;
            D_bonus_8002D8FC = temp;
            if (temp == 0xE) {
                playSound(D_bonus_8002D8EC[func_global_asm_806119A0() & 3], 0x7FFF, 63.0f, 1.0f, 0x14U, 0U);
            }
        } else {
            D_bonus_8002D8FC = func_global_asm_806119FC() < 0.5f ? 0 : -0x64;
            D_bonus_8002D8F8 = 1;
        }
        if (gameinfo->unk0 < -30 * KRAZY_KK_SCALING) { // @recomp: Usually -30, but changed to accompany how lag works
            gameinfo->unk0 = D_bonus_8002D8F4;
            i = 0;
            while (i < 6) {
                gameinfo->unk8[i] = -1;
                i++;
            }
            i_copy = 0;
            for (i = 0; i < 6; i++) {
                i_copy = i;
                var_a0 = func_global_asm_806119A0() & 7;
                while (var_a0 >= 0) {
                    if (gameinfo->unk8[i_copy] == -1) {
                        var_a0--;
                        if (var_a0 >= 0) {
                            i_copy++;
                        }
                    } else {
                        i_copy++;
                    }
                    if (i_copy >= 6) {
                        i_copy = 0;
                    }
                }
                gameinfo->unk8[i_copy] = i | 0x80;
            }
        }
        aad->unk22 = 0U;
        if ((gCurrentActorPointer->control_state == 0) && (((s32) D_global_asm_807FD610->unk30 >= 0x1F) || ((s32) D_global_asm_807FD610->unk31 >= 0x1F))) {
            if (D_global_asm_807FD610->unk2F > 0) {
                if (D_global_asm_807FD610->unk2E < -0x28) {
                    aad->unk22 = 1U;
                } else if (D_global_asm_807FD610->unk2E >= 0x29) {
                    aad->unk22 = 3U;
                } else {
                    aad->unk22 = 2U;
                }
            } else if (D_global_asm_807FD610->unk2E < -0x28) {
                aad->unk22 = 4U;
            } else if (D_global_asm_807FD610->unk2E >= 0x29) {
                aad->unk22 = 5U;
            } else {
                aad->unk22 = 6U;
            }
        }
        var_f14 = D_global_asm_807F5FD4->unk0[0][aad->unk22].unk4;
        var_f0_2 = D_global_asm_807F5FD4->unk0[0][aad->unk22].unk8;
        if (aad->unk22) {
            var_f14 += 27.0f;
            var_f0_2 -= 8.0f;
        }
        temp_v0_5 = func_bonus_80027548(D_global_asm_807F5FD4->unk0[0][aad->unk22].unk0, var_f14, var_f0_2);
        if ((temp_v0_5 != 1) && (temp_v0_5 == 2)) {
            aad->unk25 = 0xFF;
            playSound(0x3BC, 0x7FFF, 63.0f, 1.0f, 0U, 0U);
        }
        switch (gCurrentActorPointer->control_state) {
        case 0:
            if (gameinfo->unk14 == 0) {
                gCurrentActorPointer->control_state = 2;
            }
            if (gCurrentActorPointer->unk11C->control_state == 5) {
                gCurrentActorPointer->control_state = 1;
            }
            break;
        case 1:
            func_bonus_800265C0(0U, 1U);
            playCutscene(NULL, 1, 0x11U);
            gCurrentActorPointer->control_state = 3;
            break;
        case 2:
            func_bonus_800264E0(0U, 0U);
            playCutscene(NULL, 1, 0x11U);
            gCurrentActorPointer->control_state = 3;
            break;
        }
    } else {
        switch (aad->unk0) {
            case -2:
                if (is_cutscene_active != 1) {
                    aad->unk0++;
                    func_global_asm_806A2A10(0xDC, 0x2A, gameinfo->unkE);
                    loadText(gCurrentActorPointer, 0U, 9U);
                }
                break;
            case -1:
                if ((gCurrentActorPointer->object_properties_bitfield & 0x02000000) == 0) {
                    aad->unk0 = 1;
                    func_global_asm_806A2B08(gCurrentActorPointer->unk11C);
                    func_bonus_8002733C(aad);
                    playSong(MUSIC_8_BONUS_MINIGAMES, 1.0f);
                }
                break;
        }
    }
    addActorToTextOverlayRenderArray(func_bonus_80029B9C, gCurrentActorPointer, 3U);
    renderActor(gCurrentActorPointer, 0U);
}