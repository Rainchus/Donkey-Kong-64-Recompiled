#include "common_structs.h"
#include "ui.h"

extern u8 cc_player_index;
extern f32 func_global_asm_8065CFB8(s16 arg0, f32 arg1);
extern f32 func_global_asm_8065D0FC(f32 arg0);
extern Maps current_map;
extern void func_global_asm_8065CE4C(f32 arg0, f32 arg1, f32 arg2, f32 arg3, s16 arg4, s16 *arg5);

#define ACTOR_RATIO 0.0f
f32 recomp_filter_draw(f32 value, f32 ratio) {
    f32 fl = recomp_get_draw_distance();
    fl *= ratio;
    if (value < fl) {
        return fl;
    }
    return value;
}

f32 recomp_filter_draw_sq(f32 value, f32 ratio) {
    f32 fl = recomp_get_draw_distance();
    fl *= ratio;
    if (value < SQ(fl)) {
        return SQ(fl);
    }
    return value;
}

Gfx* func_global_asm_80614B34(Gfx*, Actor*);
s32 func_global_asm_80658E8C(f32, f32, f32, u8, s32);
void* func_global_asm_80722294(void*, Actor*, u8);
void func_global_asm_80614A64(Actor *arg0);
u8 getBonePosition(Actor *actor, s32 boneIndex, f32 *x, f32 *y, f32 *z);
void func_global_asm_8065C334(f32 arg0, f32 arg1, f32 arg2, s16 arg3, u8 *arg4, u8 *arg5, u8 *arg6, s16 arg7);
Gfx *func_global_asm_8065D008(Gfx *dl, s16 arg1, u8 arg2);
extern u8 cc_number_of_players;
extern u8  D_global_asm_807444FC;
RECOMP_PATCH Gfx* func_global_asm_80630DCC(s32 arg0, Actor* arg1, Gfx* dl, s32 arg3) {
    f32 temp_f0_2;
    f32 temp_f0_3;
    f32 temp_f12_2;
    f32 temp_f2_3;
    f32 var_f16;
    u8 spCB;
    u8 spCA;
    u8 spC9;
    f32 var_f4;
    f32 var_f8;
    f32 var_f18;
    f32 spB8;
    f32 sp88;
    f32 sp84;
    f32 sp80;
    f64 temp_f2_4;
    f64 var_f0;
    f64 var_f0_3;
    f32 var_f0_2;
    f32 sp64;
    s32 var_v1;
    s32 dst;

    spB8 = 1.0f;
    if ((arg1->object_properties_bitfield & 4) == 0) {
        var_v1 = FALSE;
        if ((cc_number_of_players >= 2) && (arg1->interactable & 1) && (arg1->PaaD->unk1A4 != cc_player_index)) {
            var_v1 = TRUE;
        }
        if (!var_v1) {
            return dl;
        }
    }
    switch (arg1->unk58) {
        case ACTOR_CASTLE_BRIDGE:
        case ACTOR_LARGE_BRIDGE:
        case ACTOR_UNKNOWN_174:
            break;
        default:
            if ((arg1->object_properties_bitfield & 0x400) == 0) {
                sp64 = _sqrtf(
                    SQ(character_change_array[cc_player_index].unk21C - arg1->x_position) +
                    SQ(character_change_array[cc_player_index].unk220 - arg1->y_position) +
                    SQ(character_change_array[cc_player_index].unk224 - arg1->z_position)
                );
                if (func_global_asm_8065D0FC(recomp_filter_draw(arg1->draw_distance, ACTOR_RATIO)) < sp64) {
                    return dl;
                }
                if (func_global_asm_80658E8C(arg1->x_position, arg1->y_position, arg1->z_position, arg1->unk130, arg1->unk131)) {
                    return dl;
                }
            }
            break;
    }
    arg1->object_properties_bitfield |= 0x100;
    if ((arg1->object_properties_bitfield & 0x200) == 0) {
        func_global_asm_80614A64(arg1);
    }
    gDPPipeSync(dl++);
    gSPSetGeometryMode(dl++, G_ZBUFFER | G_SHADING_SMOOTH);
    gSPClearGeometryMode(dl++, G_CULL_BOTH | G_FOG);
    if (arg1->unk58 == ACTOR_INSTRUMENT_LOGO) {
        gSPClearGeometryMode(dl++, G_ZBUFFER);
    }
    gSPSegment(dl++, 0x0F, osVirtualToPhysical(arg1->unk148[D_global_asm_807444FC]));
    if (arg1->object_properties_bitfield & 0x1000) {
        dl = func_global_asm_80722294(dl, arg1, cc_player_index);
        gSPSetGeometryMode(dl++, G_LIGHTING);
        gDPSetPrimColor(dl++, 0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF);
    } else {
        gSPClearGeometryMode(dl++, G_LIGHTING);
        if (arg1->object_properties_bitfield & 0x800000) {
            spCB = arg1->unk16A;
            spCA = arg1->unk16B;
            spC9 = arg1->unk16C;
        } else {
            if (arg1->unkCD) {
                getBonePosition(arg1, arg1->unkCD, &sp88, &sp84, &sp80);
            } else {
                sp88 = arg1->position.f[0];
                sp84 = arg1->position.f[1];
                sp80 = arg1->position.f[2];
            }
            func_global_asm_8065C334(sp88, sp84, sp80, arg1->unkCE, &spCB, &spCA, &spC9, arg1->unk12C);
            if ((arg1->unk64 & 1) == 0) {
                var_f8 = arg1->unk16D;
                temp_f0_2 = var_f8 / 15.0f;
                spCB *= temp_f0_2;
                spCA *= temp_f0_2;
                spC9 *= temp_f0_2;
            }
        }
        gDPSetPrimColor(dl++, 0, 0xFF, spCB, spCA, spC9, 0xFF);
    }
    if (arg1->object_properties_bitfield & 0x8000) {
        func_global_asm_8065CE4C(arg1->position.f[0], arg1->position.f[1], arg1->position.f[2], recomp_filter_draw(arg1->draw_distance, ACTOR_RATIO), -1, &arg1->shadow_opacity);
    }
    if ((arg1->interactable != 0x80) && (arg1->interactable != 0x40) && (arg1->interactable != 8)) {
        if ((arg1->position.f[1] - 5.0f) < character_change_array[cc_player_index].unk220) {
            if ((character_change_array[cc_player_index].unk220 < (arg1->position.f[1] + (2.0 * arg1->unk15E)))) {
                goto block_47;
            } else {
                goto block_46;
            }
        }
block_46:
        if (arg1->unk6A & 0x2000) {
block_47:
            temp_f2_3 = arg1->position.f[2] - character_change_array[cc_player_index].unk224;
            temp_f12_2 = arg1->position.f[0] - character_change_array[cc_player_index].unk21C;
            dst = SQ(temp_f2_3) + SQ(temp_f12_2);
            var_f0_2 = arg1->animation_state->scale[1];
            if (var_f0_2 < 0.01) {
                var_f0_2 = 0.01f;
            }
            temp_f2_4 = (dst / (var_f0_2 / 0.15)) / 400.0;
            var_f0_3 = MAX(temp_f2_4 - 0.3, 0.0);
            if (var_f0_3 < 1.0) {
                spB8 = MAX(temp_f2_4 - 0.3, 0.0);
            } else {
                spB8 = 1.0;
            }
        }
    }
    temp_f0_3 = arg1->shadow_opacity * spB8;
    if (temp_f0_3 != 0.0f) {
        dl = func_global_asm_8065D008(dl, temp_f0_3, 1U);
        dl = func_global_asm_80614B34(dl, arg1);
    }
    gDPPipeSync(dl++);
    gDPSetTextureLUT(dl++, G_TT_NONE);
    gDPPipeSync(dl++);
    return dl;
}

typedef struct chunk_14 Chunk14;
struct chunk_14 {
    PropModel *unk0;
    f32 unk4;
    f32 unk8;
    f32 unkC;
    f32 unk10;
    Chunk14 *next;
    Chunk14 *next2;
    s16 unk1C;
    s16 unk1E;
    s16 unk20;
    u8 unk22;
    u8 unk23;
    u8 unk24;
};


extern void *_malloc(s32);
extern Chunk14 *D_global_asm_807F5FF0;
extern Chunk14 *D_global_asm_807F5FF4;
extern Chunk14 *D_global_asm_807F5FF8;
extern Chunk14 *D_global_asm_807F5FFC;

RECOMP_PATCH Chunk14 *func_global_asm_806303C4(Chunk14 *arg0, u8 arg1, PropModel *arg2, f32 arg3, f32 arg4, f32 arg5, s16 arg6, s16 arg7, u8 arg8, s16 arg9, u8 argA) {
    Chunk14 *temp_v0;
    Chunk14 *phi_v1;
    s32 phi_v0;
    Chunk14 *phi_a1;

    phi_v0 = FALSE;
    phi_v1 = NULL;
    arg6 = recomp_filter_draw(arg6, 1.0f);
    if (argA == 1) {
        arg0 = D_global_asm_807F5FF0;
    }
    if (argA == 2) {
        arg0 = D_global_asm_807F5FF4;
    }
    if (argA == 3) {
        arg0 = D_global_asm_807F5FF8;
    }
    if (argA == 4) {
        arg0 = D_global_asm_807F5FFC;
    }
    phi_a1 = arg0;
    while (arg0 && !phi_v0) {
        if (arg2 == arg0->unk0) {
            phi_v0 = TRUE;
        } else {
            phi_v1 = arg0;
            arg0 = arg0->next;
        }
    }
    if (phi_v0) {
        arg0->unk4 = arg3;
        arg0->unk8 = arg4;
        arg0->unkC = arg5;
        arg0->unk1C = arg6;
        if (arg9 != -1) {
            arg0->unk20 = arg9;
        }
        arg0->unk23 = 1;
    } else {
        temp_v0 = _malloc(sizeof(Chunk14));
        temp_v0->unk24 = arg1;
        temp_v0->unk0 = arg2;
        temp_v0->unk4 = arg3;
        temp_v0->unk8 = arg4;
        temp_v0->unkC = arg5;
        temp_v0->unk1C = arg6;
        temp_v0->unk1E = arg7;
        temp_v0->unk22 = arg8;
        temp_v0->unk23 = 1;
        temp_v0->unk10 = 0.0f;
        temp_v0->unk20 = 0;
        temp_v0->next = NULL;
        if (phi_v1) {
            phi_v1->next = temp_v0;
            temp_v0->next2 = phi_v1;
        } else {
            phi_a1 = temp_v0;
            temp_v0->next2 = NULL;
        }
    }
    if (argA == 1) {
        D_global_asm_807F5FF0 = phi_a1;
    }
    if (argA == 2) {
        D_global_asm_807F5FF4 = phi_a1;
    }
    if (argA == 3) {
        D_global_asm_807F5FF8 = phi_a1;
    }
    if (argA == 4) {
        D_global_asm_807F5FFC = phi_a1;
    }
    return phi_a1;
}

u8 func_global_asm_80652E58(s16 arg0);
f32 func_global_asm_80689DD4(f32 x, f32 y, f32 z);

RECOMP_PATCH u8 func_global_asm_80689F80(ActorSpawner *spawner) {
    return func_global_asm_80652E58(spawner->unk4A)
        && func_global_asm_80689DD4(spawner->unk10, spawner->unk14, spawner->unk18) < recomp_filter_draw_sq(spawner->unk54, ACTOR_RATIO);
}

RECOMP_PATCH u8 func_global_asm_80689FEC(ActorSpawner *spawner) {
    return (!func_global_asm_80652E58(spawner->tied_actor->unk12C)
        || !(func_global_asm_80689DD4(spawner->unk10, spawner->unk14, spawner->unk18) < recomp_filter_draw_sq(spawner->unk54, ACTOR_RATIO)))
    && spawner->tied_actor->unk114 == NULL;
}

RECOMP_PATCH u8 func_global_asm_8068A074(ActorSpawner *spawner) {
    return (!func_global_asm_80652E58(spawner->tied_actor->unk12C)
        || !(func_global_asm_80689DD4(spawner->unk10, spawner->unk14, spawner->unk18) < recomp_filter_draw_sq(spawner->unk54, ACTOR_RATIO)))
        && (spawner->tied_actor->unk114 == NULL
        && spawner->tied_actor->control_state == 0);
}

RECOMP_PATCH u8 func_global_asm_8068A118(ActorSpawner *arg0) {
    // TODO: idk why this temporary variable is needed but... yeah
    u8 temp = func_global_asm_80689DD4(arg0->unk10, arg0->unk14, arg0->unk18) < recomp_filter_draw_sq(arg0->unk54, ACTOR_RATIO);
    return temp;
}

RECOMP_PATCH u8 func_global_asm_8068A164(ActorSpawner *spawner) {
    return !(func_global_asm_80689DD4(spawner->tied_actor->x_position, spawner->tied_actor->y_position, spawner->tied_actor->z_position) < recomp_filter_draw_sq(spawner->unk54, ACTOR_RATIO));
}

typedef struct Struct807FDC90 Struct807FDC90;

// Use this for D_global_asm_807FDC90
struct Struct807FDC90 {
    Struct807FDC90 *unk0; // TODO: This type may not be correct
    Actor *unk4;
    s16 unk8;
    s16 unkA; // Used: X Position
    s16 unkC; // Used
    s16 unkE; // Used: Z Position
    s16 unk10; // Used
    s16 unk12;
    s16 unk14; // Used
    s16 unk16;
    u16 unk18;
    u16 unk1A; // Used
    union {
        struct {
            u16 unk1C; // Used
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
    s16 unk26; // Used
    s32 unk28;
    s16 unk2C;
    s16 unk2E; // TODO: This might not be correct
    f32 unk30; // Used
    u8 unk34;
    u8 unk35; // Used
    u8 unk36;
    u8 unk37;
    u8 unk38;
};

typedef struct {
    s16 count;
    s16 unk2;
    EnemySpawner *firstSpawner;
} EnemySpawnerLocator;

u8 func_global_asm_8072818C(EnemySpawner *, s32);
void func_global_asm_8061CFCC(Actor *arg0);
s32 deleteActor(Actor*);
s16 func_global_asm_806531B8(f32 arg0, f32 arg1, f32 arg2, s16 arg3);
void func_global_asm_807278C0(EnemySpawner *arg0);
s32 spawnActor(Actors actorIndex, s32 modelIndex);
void func_global_asm_80726744(Actor *, EnemySpawner *);
u8 func_global_asm_80727F20(EnemySpawner *arg0, s32 arg1);
s32 func_global_asm_807317FC(s16 arg0, s16 arg1);
u8 func_global_asm_80728004(EnemySpawner *arg0, s32 arg1);
void func_global_asm_80678428(Actor *arg0);
u8 func_global_asm_807280C8(EnemySpawner *arg0, s32 arg1);
void func_global_asm_80602B60(s32 arg0, u8 arg1);
void func_global_asm_80605314(Actor *arg0, u8 arg1);
void func_global_asm_80678458(Actor *arg0);
extern EnemySpawnerLocator* D_global_asm_80755694;
extern Struct807FDC90 *D_global_asm_807FDC90;
extern s32 D_global_asm_807FBB64;
extern Actor *gLastSpawnedActor;
extern Actor *gCurrentActorPointer;

RECOMP_PATCH void func_global_asm_80727958(void) {
    Actor *temp_a0;
    Actor *temp_a0_2;
    Actor *temp_a0_3;
    Actor *temp_a0_4;
    Actor *temp_a0_5;
    Actor *temp_a0_6;
    Actor *temp_v0_5;
    EnemyMovementBox *temp_v0_4;
    EnemySpawner *var_s0;
    Struct8075EB80 *temp_v0_2;
    Struct8075EB80 *temp_v0_3;
    f32 dz;
    f32 dx;
    s16 temp_v0;
    s16 j;
    s16 i;
    u8 temp_a1;
    s32 temp_f16;
    s32 temp_lo;
    s32 temp_t4;
    s32 var_s1;
    s32 min_dist;
    s32 var_v0;
    u8 temp_v0_6;
    u8 temp_v0_7;
    PlayerAdditionalActorData *PaaD;

    var_s0 = D_global_asm_80755694->firstSpawner;
    for (i = 0; i < D_global_asm_80755694->count; i++) {
        temp_a1 = (var_s0->spawn_state > 4) && (var_s0->spawn_state < 7);
        if (temp_a1 != 0) {
            D_global_asm_807FDC90 = var_s0->tied_actor->additional_actor_data;
        }
        var_s1 = D_global_asm_8075EB80[var_s0->alternative_enemy_spawn].unkE * 100;
        var_s1 = var_s1 * var_s1;
        if (temp_a1 != 0) {
            var_s0->chunk = var_s0->tied_actor->unk12C;
        }
        if ((var_s1 == 0) || (var_s0->properties_bitfield & 4) || (D_global_asm_807FBB64 & 0x100)) {
            var_s1 = 0x7FFFFFFF;
        } else if (var_s0->alternative_enemy_spawn != 0x44) { // Not a fairy
            var_s1 = recomp_filter_draw_sq(var_s1, ACTOR_RATIO);
        }
        if ((temp_a1 != 0) && var_s0->tied_actor->control_state == 0x3B) {
            func_global_asm_8061CFCC(var_s0->tied_actor);
            deleteActor(var_s0->tied_actor);
            var_s0->spawn_state = 0;
        } else if ((temp_a1 != 0) && var_s0->tied_actor->control_state == 0x3C) {
            func_global_asm_8061CFCC(var_s0->tied_actor);
            deleteActor(var_s0->tied_actor);
            var_s0->spawn_state = var_s0->init.something_spawn_state;
            var_s0->properties_bitfield &= 0xFFFB;
        } else {
            if ((var_s0->spawn_state == 7) && (var_s0->respawn_time != 0)) {
                if (!(var_s0->properties_bitfield & 2)) {
                    var_s0->respawn_time--;
                    if (var_s0->respawn_time == 0) {
                        var_s0->counter += 1;
                        var_s0->spawn_state = var_s0->init.something_spawn_state;
                        var_s0->chunk = func_global_asm_806531B8(var_s0->init.x_pos, var_s0->init.y_pos, var_s0->init.z_pos, 0);
                        func_global_asm_807278C0(var_s0);
                        temp_v0_2 = &D_global_asm_8075EB80[var_s0->alternative_enemy_spawn];
                        if (spawnActor(temp_v0_2->unk0, temp_v0_2->unk2)) {
                            func_global_asm_80726744(gLastSpawnedActor, var_s0);
                            gLastSpawnedActor->control_state = 0x36;
                            var_s0->properties_bitfield |= 1;
                        }
                    }
                }
            } else if (var_s0->spawn_state == 2) {
                if ((func_global_asm_80727F20(var_s0, var_s1)) && (func_global_asm_807317FC(current_map, var_s0->init.spawn_trigger))) {
                    func_global_asm_807278C0(var_s0);
                    temp_v0_3 = &D_global_asm_8075EB80[var_s0->alternative_enemy_spawn];
                    if (spawnActor(temp_v0_3->unk0, temp_v0_3->unk2)) {
                        func_global_asm_80726744(gLastSpawnedActor, var_s0);
                    }
                }
            } else if ((temp_a1 != 0) && var_s0->tied_actor->control_state == 0x40) {
                temp_v0_4 = var_s0->movement_box_pointer;
                var_s0->spawn_state = 7;
                var_s0->respawn_time = var_s0->init.respawn_timer_init * 30;
                if (temp_v0_4->unk1C == var_s0->tied_actor) {
                    temp_v0_4->unk1C = 0;
                }
                func_global_asm_8061CFCC(var_s0->tied_actor);
                deleteActor(var_s0->tied_actor);
            } else if (temp_a1 != 0) {
                min_dist = 999999;
                temp_v0_5 = D_global_asm_807FDC90->unk4;
                if ((temp_v0_5->interactable & 1) && (temp_v0_5->control_state == 0x67)) {
                    temp_a0_4 = var_s0->tied_actor;
                    if ((temp_a0_4->interactable & 2) && (var_s0->spawn_state == 5)) {
                        temp_v0_6 = temp_a0_4->control_state;
                        if ((temp_v0_6 != 0x16) && (temp_v0_6 != 0x37) && !(var_s0->properties_bitfield & 0x40)) {
                            temp_a0_4->control_state = 0x16;
                            var_s0->tied_actor->control_state_progress = 0;
                        }
                    }
                }
                if (cc_number_of_players > 1) {
                    for (j = 0; j < cc_number_of_players; j++) {
                        PaaD = character_change_array->playerPointer->PaaD;
                        dz = gCurrentActorPointer->z_position - character_change_array[j].playerPointer->z_position;
                        dx = gCurrentActorPointer->x_position - character_change_array[j].playerPointer->x_position;
                        temp_f16 = SQ(dz) + SQ(dx);
                        if ((temp_f16 < min_dist) || (PaaD->unkD4 != 0)) {
                            min_dist = temp_f16;
                            D_global_asm_807FDC90->unk4 = character_change_array[j].playerPointer;
                        }

                    }
                }
                if (var_s0->spawn_state == 6) {
                    if (func_global_asm_8072818C(var_s0, var_s1) != 0) {
                        deleteActor(var_s0->tied_actor);
                        var_s0->spawn_state = var_s0->init.something_spawn_state;
                    } else if (func_global_asm_80728004(var_s0, var_s1) != 0) {
                        func_global_asm_80678428(var_s0->tied_actor);
                        var_s0->spawn_state = 5;
                        var_s0->properties_bitfield |= 0x8000;
                    }
                } else if ((var_s0->spawn_state == 5) && (func_global_asm_807280C8(var_s0, var_s1) != 0)) {
                    if ((var_s0->tied_actor->control_state != 0x16) && (var_s0->tied_actor->control_state != 0x37)) {
                        if (var_s0->tied_actor->unk58 == ACTOR_FAIRY) {
                            func_global_asm_80602B60(0x42, 0U);
                        }
                        func_global_asm_80605314(var_s0->tied_actor, 0U);
                        func_global_asm_80605314(var_s0->tied_actor, 1U);
                        func_global_asm_80678458(var_s0->tied_actor);
                        var_s0->spawn_state = 6;
                        D_global_asm_807FDC90->unk18 = 0;
                    }
                }
            }
        }
        var_s0++;
    }
}

typedef struct {
    void *unk0; // Used
    void *unk4;
    u8 unk8; // Used
    u8 unk9;
    u8 unkA;
    u8 unkB;
} Struct807FA8A0;

s32 delayed_decompression_count = 0;
Struct807FA8A0 delayed_decompression_array[DECOMPRESSION_BUFFER_SIZE] = {};
OSIoMesg unk_decompression_array[DECOMPRESSION_BUFFER_SIZE] = {};
OSMesg decompression_mq[DECOMPRESSION_BUFFER_SIZE];

void func_global_asm_8066AEE4(void *arg0, void *arg1);
void _free(void *ptr);
extern void *D_global_asm_80748E14;
extern s8 D_global_asm_80746834;
extern OSMesgQueue D_global_asm_807656D0;

RECOMP_PATCH void func_global_asm_8066AF40(void) {
    s32 i;

    for (i = 0; i < delayed_decompression_count; i++) {
        D_global_asm_80746834 = 7;
        osRecvMesg(&D_global_asm_807656D0, NULL, 1);
        D_global_asm_80746834 = 0;
        if (delayed_decompression_array[i].unk8 != 0) {
            func_global_asm_8066AEE4(delayed_decompression_array[i].unk0, delayed_decompression_array[i].unk4);
        }
    }
    if (D_global_asm_80748E14 != NULL) {
        _free(D_global_asm_80748E14);
        D_global_asm_80748E14 = NULL;
    }
    delayed_decompression_count = 0;
}

s32 func_global_asm_8066B4D4(s32 arg0, s32 arg1, u32 *arg2, s32 *arg3);
void *func_global_asm_8066B5C8(s32 pointerTableIndex, s32 fileIndex);
void func_global_asm_8066B5F4(s32 pointerTableIndex);
void func_global_asm_8066B8C8(void *arg0, s32 pointerTableIndex, s32 arg2);
void *func_global_asm_806111BC(s32 arg0, s32 arg1);
void func_global_asm_8060B140(u32 arg0, u8 *arg1, s32 *arg2, u8 arg3, u8 arg4, u8 arg5, u8 *arg6);
void raiseException(u8 arg0, s32 arg1, s32 arg2, s32 arg3);
void func_global_asm_8066B4AC(s32 arg0, s32 arg1, void *arg2);
extern u8 D_global_asm_80748E18[];
extern s32 *D_global_asm_807FB1A0[];
extern s32 D_global_asm_807F9678;
extern u8 D_global_asm_807F967C;

RECOMP_PATCH void *getPointerTableFile(enum pointertable_e pointerTableIndex, u32 fileIndex, u8 arg2, u8 arg3) {
    s32 temp;
    u32 sp50;
    s32 sp4C;
    void *var_v0;
    s32 var_a1;
    void *sp40;

    func_global_asm_8066B5F4(pointerTableIndex);
    if (!arg3) {
        if ((fileIndex >= 0x80000000) && (fileIndex < 0xA0000000)) {
            func_global_asm_8066B8C8((void*)fileIndex, pointerTableIndex, 0);
            D_global_asm_807F967C = 0;
            D_global_asm_807F9678 = 0;
            return (void*)fileIndex;
        }
        var_v0 = func_global_asm_8066B5C8(pointerTableIndex, fileIndex);
        if (var_v0 != NULL) {
            func_global_asm_8066B8C8(var_v0, pointerTableIndex, fileIndex);
            D_global_asm_807F967C = 0;
            D_global_asm_807F9678 = 0;
            return var_v0;
        }
    }
    func_global_asm_8066B4D4(pointerTableIndex, fileIndex, &sp50, &sp4C);
    if (sp4C == 0) {
        D_global_asm_807F967C = 0;
        D_global_asm_807F9678 = 0;
        return NULL;
    }
    if (D_global_asm_80748E18[pointerTableIndex] != 0) {
        var_a1 = D_global_asm_807FB1A0[pointerTableIndex][fileIndex];
    } else {
        var_a1 = sp4C;
    }
    if (D_global_asm_807F9678 == 0) {
        var_v0 = _malloc(var_a1);
    } else {
        var_v0 = func_global_asm_806111BC(D_global_asm_807F9678, var_a1);
    }
    if (arg2 != 0) {
        if (D_global_asm_80748E18[pointerTableIndex] != 0) {
            sp40 = _malloc(sp4C);
            func_global_asm_8060B140(sp50, sp40, &sp4C, 0, 0, 0, 0);
            func_global_asm_8066AEE4(sp40, var_v0);
        } else {
            func_global_asm_8060B140(sp50, var_v0, &sp4C, 0, 0, 0, 0);
        }
    } else {
        if (delayed_decompression_count == DECOMPRESSION_BUFFER_SIZE) {
            raiseException(6, 0, 0, 0);
        }
        delayed_decompression_array[delayed_decompression_count].unk8 = D_global_asm_80748E18[pointerTableIndex];
        if (D_global_asm_80748E18[pointerTableIndex] != 0) {
            delayed_decompression_array[delayed_decompression_count].unk0 = _malloc(sp4C);
            delayed_decompression_array[delayed_decompression_count].unk4 = var_v0;
        } else {
            delayed_decompression_array[delayed_decompression_count].unk4 = var_v0;
            delayed_decompression_array[delayed_decompression_count].unk0 = var_v0;
        }
        osInvalDCache(delayed_decompression_array[delayed_decompression_count].unk0, sp4C);
        osPiStartDma(&unk_decompression_array[delayed_decompression_count], 0, 0, sp50, delayed_decompression_array[delayed_decompression_count].unk0, sp4C, &D_global_asm_807656D0);
        delayed_decompression_count++;
    }
    func_global_asm_8066B4AC(pointerTableIndex, fileIndex, var_v0);
    func_global_asm_8066B8C8(var_v0, pointerTableIndex, fileIndex);
    D_global_asm_807F967C = 0;
    D_global_asm_807F9678 = 0;
    return var_v0;
}
