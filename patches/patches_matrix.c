#include "common_structs.h"

extern u8 skip_interpolation;

Mtx identity_fixed_mtx = {{
    {
        0x00010000, 0x00000000,
        0x00000001, 0x00000000, }, {
        0x00000000, 0x00010000,
        0x00000000, 0x00000001,
    },
    {
        0x00000000, 0x00000000,
        0x00000000, 0x00000000, }, {
        0x00000000, 0x00000000,
        0x00000000, 0x00000000,
    }
}};
s32 cur_drawn_model_transform_id = 0;
s32 cur_model_transform_id_offset = 0;
u8 cur_drawn_model_is_map = FALSE;
u8 cur_model_uses_ex_vertex = FALSE;
u8 cur_model_uses_vertex_interp = FALSE;
u8 cur_drawn_model_skip_interpolation = FALSE;

typedef struct Struct80614C38_0 Struct80614C38;
struct Struct80614C38_0 {
    void *unk0;
    void *unk4;
    u8 unk8;
    u8 pad9[0xC - 0x9];
    Struct80614C38 *unkC;
    u8 pad10[0x14 - 0x10];
    Struct80614C38 *next;
};

typedef struct ActorModelHeader {
    s32 unk0;
    union {
        s32 *unk4;
        s32 unk4_raw;
    };
    s32 unk8;
    s32 unkC;
    s32 unk10;
    Struct80614C38 * unk14;
    u8 pad18[0x20 - 0x18];
    u8 bone_count;
    u8 unk21;
    u8 pad22[0x28 - 0x22];
} ActorModelHeader;

Gfx *func_global_asm_80614C38(Gfx *, Actor *, ActorModelHeader *);
Gfx *func_global_asm_80687EE0(Gfx *dl, Actor *arg1);
extern Actor *actor_list[];
extern s16 actor_count;

Gfx *set_model_matrix_group(Gfx * dl, void *geo_list, u8 skip_rotation, u8 *pushed) {
    if (cur_drawn_model_transform_id != 0) {
        u32 group_id;
        // Pick a group ID based on whether this is a map or not.
        // if (cur_drawn_model_is_map) {
        //     // Map models use a group ID determined by the offset of the geo command to guarantee they're unique and consistent between frames.
        //     // group_id = cur_drawn_model_transform_id + (u32)geo_list - (u32)modelRenderModelBin - modelRenderModelBin->geo_list_offset_4;
        // }
        // else {
            // Other models use a group ID determined by the transform ID offset.
            group_id = cur_drawn_model_transform_id + cur_model_transform_id_offset;
        // }
        if (skip_interpolation || cur_drawn_model_skip_interpolation) {
            // Skip interpolation if all interpolation is currently skipped or the transform was specified to be skipped.
            gEXMatrixGroupSkipAll(dl++, group_id, G_EX_PUSH, G_MTX_MODELVIEW, G_EX_EDIT_NONE);
        }
        else {
            // Tag the matrix with simple matrix interpolation if the model uses bones.
            // Use decomposed matrix interpolation on any other model.
            // u8 interpolation_mode = cur_model_uses_bones ? G_EX_INTERPOLATE_SIMPLE : G_EX_INTERPOLATE_DECOMPOSE;
            u8 rotation_mode = skip_rotation ? G_EX_COMPONENT_SKIP : G_EX_COMPONENT_INTERPOLATE;
            u8 vertex_interpolation_mode = cur_model_uses_vertex_interp ? G_EX_COMPONENT_INTERPOLATE : G_EX_COMPONENT_SKIP;
            // u8 texcoord_interpolation_mode = cur_drawn_model_is_map ? G_EX_COMPONENT_INTERPOLATE : G_EX_COMPONENT_SKIP;
            u8 interpolation_mode = G_EX_INTERPOLATE_SIMPLE;
            // u8 rotation_mode = G_EX_COMPONENT_INTERPOLATE;
            // u8 vertex_interpolation_mode = G_EX_COMPONENT_SKIP;
            u8 texcoord_interpolation_mode = G_EX_COMPONENT_SKIP;
            gEXMatrixGroup(dl++, group_id, interpolation_mode, G_EX_PUSH, G_MTX_MODELVIEW, G_EX_COMPONENT_INTERPOLATE, rotation_mode,
                G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_INTERPOLATE, G_EX_COMPONENT_INTERPOLATE, vertex_interpolation_mode, G_EX_COMPONENT_INTERPOLATE,
                G_EX_ORDER_LINEAR, G_EX_EDIT_NONE, G_EX_ASPECT_AUTO, texcoord_interpolation_mode, G_EX_COMPONENT_AUTO);
        }
        *pushed = TRUE;
        return dl;
    }
    else if (skip_interpolation) {
        gEXMatrixGroupNoInterpolate(dl++, G_EX_PUSH, G_MTX_MODELVIEW, G_EX_EDIT_NONE);
        *pushed = TRUE;
        return dl;
    }
    else {
        *pushed = FALSE;
        return dl;
    }
}

Gfx *pop_model_matrix_group(Gfx *dl) {
    gEXPopMatrixGroup(dl++, G_MTX_MODELVIEW);
    return dl;
}

typedef struct actor_lockdown_struct {
    Actor * actor;
    u16 value;
    u8 used;
    u8 pad;
} actor_lockdown_struct;

#define ACTOR_LOCKDOWN_BUFFER_SIZE 16
static actor_lockdown_struct actor_lockdowns[ACTOR_LOCKDOWN_BUFFER_SIZE];

void set_actor_interpolation_lockdown(Actor * ac, u16 value) {
    s32 i;
    actor_lockdown_struct *ld;
    for (i = 0; i < ACTOR_LOCKDOWN_BUFFER_SIZE; i++) {
        ld = &actor_lockdowns[i];
        if ((ld->actor == ac) || (!ld->used)) {
            if (!ld->used) {
                ld->used = TRUE;
                ld->value = value;
                ld->actor = ac;
            } else if (ld->value < value) {
                ld->value = value;
            }
            // recomp_printf("Set interpolation lockdown for actor %d (%08X) for %d frames\n", ac->unk58, ac, value);
            return;
        }
    }
}

void clear_actor_interpolation_lockdowns(void) {
    s32 i;
    for (i = 0; i < ACTOR_LOCKDOWN_BUFFER_SIZE; i++) {
        actor_lockdowns[i].used = FALSE;
    }
}

s32 getActorMatrixTag(Actor *ac) {
    if (ac->unk58 == 333) {
        // Main Menu barrel. This respawns every frame because dk64 lol
        return MTXTAG_MAINMENU_BARREL;
    }
    return MTXTAG_ACTORS + (ac->unk54 * 0x100);
}

// @recomp: Actor matrix stuff
RECOMP_PATCH Gfx *func_global_asm_80614B34(Gfx *dl, Actor *arg1) {
    ActorModelHeader *var_s0;
    s32 var_v1;
    u8 pushed_matrix_group = FALSE;
    s32 i;
    actor_lockdown_struct *ld;

    cur_drawn_model_transform_id = getActorMatrixTag(arg1);
    var_s0 = (ActorModelHeader *)arg1->unk0;
    if (arg1->unk4C != NULL) {
        var_s0 = (ActorModelHeader *)arg1->unk4C;
    }
    if (arg1->unk50 != NULL) {
        var_s0 = (ActorModelHeader *)arg1->unk50;
    }
    dl = func_global_asm_80687EE0(dl, arg1);
    dl = func_global_asm_80614C38(dl, arg1, var_s0);
    gSPSegment(dl++, 0x04, osVirtualToPhysical(arg1->unk8));
    gSPSegment(dl++, 0x03, osVirtualToPhysical((ActorModelHeader *)var_s0->unk0));
    for (i = 0; i < ACTOR_LOCKDOWN_BUFFER_SIZE; i++) {
        ld = &actor_lockdowns[i];
        if (ld->used && ld->actor == arg1) {
            if (ld->value > 0) {
                cur_drawn_model_skip_interpolation = TRUE;
                ld->value--;
            }
            if (ld->value == 0) {
                ld->used = FALSE;
            }
        }
    }
    for (var_v1 = 0; var_v1 < var_s0->unk21; var_v1++) {
        cur_model_transform_id_offset = var_v1;
        gSPMatrix(dl++, &identity_fixed_mtx, G_MTX_PUSH | G_MTX_MUL | G_MTX_MODELVIEW);
        dl = set_model_matrix_group(dl, NULL, FALSE, &pushed_matrix_group);
        gSPDisplayList(dl++, var_s0->unk4[var_v1]);
        gSPPopMatrix(dl++, G_MTX_MODELVIEW);
        if (pushed_matrix_group) {
            dl = pop_model_matrix_group(dl);
        }
    }
    cur_drawn_model_skip_interpolation = FALSE;
    return dl;
}


void func_global_asm_80636D38(Prop*, s32, s32);
void func_global_asm_80639968(void*);
void func_global_asm_8065EB10(u8, s16, f32, f32, f32, s16, f32, s16);
f32 func_global_asm_8065D0FC(f32 arg0);
s32 func_global_asm_80659470(s32 arg0);
s32 func_global_asm_806D0964(s32 arg0, u8 playerIndex);
void func_global_asm_8065C334(f32 arg0, f32 arg1, f32 arg2, s16 arg3, u8 *arg4, u8 *arg5, u8 *arg6, s16 arg7);
void func_global_asm_8063E6B4(Prop_ScriptData *arg0);
void func_global_asm_8065CE4C(f32 arg0, f32 arg1, f32 arg2, f32 arg3, s16 arg4, s16 *arg5);
Gfx *func_global_asm_8065D008(Gfx *dl, s16 arg1, u8 arg2);
extern u8 D_global_asm_80750AB4;
extern u8 cc_player_index;
extern u8  D_global_asm_807444FC;
extern Prop *D_global_asm_807F6000;

typedef struct Struct80636FFC {
    f32 unk0;
    f32 unk4;
    f32 unk8;
    f32 unkC;
    f32 unk10;
    f32 unk14;
    f32 unk18;
    u8 pad1C[0x20 - 0x1C];
    Mtx unk20[2];
    Gfx *unkA0[2];
    Gfx *unkA8[2];
    void *unkB0;
    s16 unkB4;
    s16 unkB6;
    s32 unkB8;
    u8 padBC[0xC0 - 0xBC];
    s16 unkC0;
    u8 unkC2;
    u8 unkC3;
    u8 unkC4;
    u8 unkC5;
} Struct80636FFC;

extern f32 recomp_filter_draw(f32 value, f32 ratio);

// @recomp: Model 2 Matrix stuff
RECOMP_PATCH Gfx* func_global_asm_80636FFC(Struct80636FFC* arg0, Gfx* dl, s32 arg2, s32 arg3, s32 arg4, u8 arg5, s16 arg6) {
    f32 spF8[4][4];
    f32 spB8[4][4];
    s32* temp_a2;
    Prop_ScriptData* temp_a0_4;
    s32 pad;
    u8 var_v0;
    void ** var_a2;
    f32 var_f6;
    f32 var_f8;
    f32 temp_f0_6;
    u8 sp97;
    u8 sp96;
    u8 sp95;
    s16 sp92;
    s16 temp_v0_3;
    f32 var_f0;
    f32 sp88;
    f32 var_f2;
    f32 dX;
    f32 dY;
    f32 dZ;
    f32 var_f4;
    u8 pushed_matrix_group = FALSE;

    if (arg5 != 0) {
        dX = SQ(character_change_array[cc_player_index].unk21C - arg0->unk0);
        dY = character_change_array[cc_player_index].unk220 - arg0->unk4;
        dZ = character_change_array[cc_player_index].unk224 - arg0->unk8;
        sp88 = _sqrtf(dX + SQ(dY) + SQ(dZ));
        var_f2 = func_global_asm_8065D0FC(recomp_filter_draw(arg0->unkC0, 1.0f));
        if (arg0->unkC5 & 1) {
            var_f2 = recomp_filter_draw(arg0->unkC0, 1.0f);
        }
        if (var_f2 < sp88) {
            return dl;
        }
    }
    if (arg0->unkC3 != 0) {
        func_global_asm_8065EB10(arg0->unkC3, arg0->unkB6, arg0->unk0, arg0->unk4, arg0->unk8, recomp_filter_draw(arg0->unkC0, 1.0f), sp88 / var_f2, (s32) arg6);
    }
    if ((arg0->unkB4 == 0) || (arg0->unkB4 == 0x241)) {
        return dl;
    }
    if ((arg0->unkB4 == 0x74) || (arg0->unkB4 == 0x288) || (arg0->unkB4 == 0x290)) {
        arg0->unk14 = (f32) ((f64) arg0->unk14 + 10.0);
    }
    guScaleF(spF8, arg0->unkC, arg0->unkC, arg0->unkC);
    if (arg0->unk10 != 0.0) {
        guRotateF(spB8, arg0->unk10, 1.0f, 0.0f, 0.0f);
        guMtxCatF(spF8, spB8, spF8);
    }
    if (arg0->unk14 != 0.0) {
        guRotateF(spB8, arg0->unk14, 0.0f, 1.0f, 0.0f);
        guMtxCatF(spF8, spB8, spF8);
    }
    if (arg0->unk18 != 0.0) {
        guRotateF(spB8, arg0->unk18, 0.0f, 0.0f, 1.0f);
        guMtxCatF(spF8, spB8, spF8);
    }
    guTranslateF(spB8, arg0->unk0, arg0->unk4, arg0->unk8);
    guMtxCatF(spF8, spB8, spF8);
    guMtxF2L(spF8, &arg0->unk20[D_global_asm_807444FC]);
    gDPPipeSync(dl++);
    gSPSegment(dl++, 0x08, osVirtualToPhysical(arg0->unkB0));
    var_a2 = NULL;
    if (arg0->unkB6 != -1) {
        temp_v0_3 = func_global_asm_80659470(arg0->unkB6);
        func_global_asm_80636D38(&D_global_asm_807F6000[temp_v0_3], 0, 1);
        var_a2 = (void **)D_global_asm_807F6000[temp_v0_3].unk78;
        if (func_global_asm_806D0964(D_global_asm_807F6000[temp_v0_3].unk24->unk0, cc_player_index) != 0) {
            return dl;
        }
    }
    if (var_a2 != NULL) {
        gSPSegment(dl++, 0x09, osVirtualToPhysical(var_a2[D_global_asm_807444FC]));
    }
    gSPMatrix(dl++, osVirtualToPhysical(&arg0->unk20[D_global_asm_807444FC]), G_MTX_PUSH | G_MTX_MUL | G_MTX_MODELVIEW);
    if (arg0->unkB8 != 0) {
        var_v0 = D_global_asm_807444FC;
        func_global_asm_80639968(arg0);
    } else {
        var_v0 = 0;
    }
    gSPSegment(dl++, 0x0A, osVirtualToPhysical(arg0->unkA0[var_v0]));
    gSPClearGeometryMode(dl++, G_CULL_BOTH | G_FOG);
    if ((arg5) && (arg0->unkC2 == 0)) {
        if (D_global_asm_807F6000[temp_v0_3].unk7C) {
            var_f0 = D_global_asm_807F6000[temp_v0_3].unk7C->unk98;
        } else {
            var_f0 = 0;
        }
        func_global_asm_8065C334(arg0->unk0, arg0->unk4 + var_f0, arg0->unk8, 0, &sp97, &sp96, &sp95, (s16) (s32) arg6);
        temp_f0_6 = arg0->unkC4 / 15.0f;
        sp97 *= temp_f0_6;
        sp96 *= temp_f0_6;
        sp95 *= temp_f0_6;
    } else {
        sp95 = 0xFF;
        sp96 = 0xFF;
        sp97 = 0xFF;
    }
    gDPSetPrimColor(dl++, 0, 0xFF, sp97, sp96, sp95, 0xFF);
    if (arg5) {
        if ((arg0->unkB6 != -1) && (temp_a0_4 = D_global_asm_807F6000[temp_v0_3].unk7C, (temp_a0_4 != NULL)) && (temp_a0_4->unk60 == 1)) {
            func_global_asm_8063E6B4(temp_a0_4);
            sp92 = D_global_asm_807F6000[temp_v0_3].unk7C->unk64;
        } else {
            func_global_asm_8065CE4C(arg0->unk0, arg0->unk4, arg0->unk8, recomp_filter_draw(arg0->unkC0, 1.0f), (s16) (s32) arg0->unkB4, &sp92);
            if (arg0->unkB6 != -1) {
                if ((D_global_asm_807F6000[temp_v0_3].unk7C != NULL) && (D_global_asm_80750AB4 == 1)) {
                    D_global_asm_807F6000[temp_v0_3].unk7C->unk62 = sp92;
                }
            }
            if (arg0->unkB6 != -1) {
                if (D_global_asm_807F6000[temp_v0_3].unk7C != NULL) {
                    func_global_asm_8063E6B4(D_global_asm_807F6000[temp_v0_3].unk7C);
                }
            }
            if (arg0->unkB6 != -1) {
                if ((D_global_asm_807F6000[temp_v0_3].unk7C != NULL) && (D_global_asm_80750AB4 == 1)) {
                    sp92 = D_global_asm_807F6000[temp_v0_3].unk7C->unk64;
                }
            }
        }
    } else {
        sp92 = 0xFF;
    }
    if (sp92 != 0) {
        if (arg0->unkB6 != -1) {
            if (D_global_asm_807F6000[temp_v0_3].unk8C != 0) {
                if ((D_global_asm_807F6000[temp_v0_3].unk8C & 8) && (character_change_array[cc_player_index].playerPointer->unk58 != ACTOR_DK)) {
                    sp92 = 0x64;
                }
                if ((D_global_asm_807F6000[temp_v0_3].unk8C & 2) && (character_change_array[cc_player_index].playerPointer->unk58 != ACTOR_DIDDY)) {
                    sp92 = 0x64;
                }
                if ((D_global_asm_807F6000[temp_v0_3].unk8C & 4) && (character_change_array[cc_player_index].playerPointer->unk58 != ACTOR_TINY)) {
                    sp92 = 0x64;
                }
                if ((D_global_asm_807F6000[temp_v0_3].unk8C & 1) && (character_change_array[cc_player_index].playerPointer->unk58 != ACTOR_CHUNKY)) {
                    sp92 = 0x64;
                }
                if ((D_global_asm_807F6000[temp_v0_3].unk8C & 0x10) && (character_change_array[cc_player_index].playerPointer->unk58 != ACTOR_LANKY)) {
                    sp92 = 0x64;
                }
            }
        }
        dl = func_global_asm_8065D008(dl, sp92, 0U);
        gDPPipeSync(dl++);
        // @recomp: mtx tag
        cur_drawn_model_transform_id = MTXTAG_PROP + D_global_asm_807F6000[temp_v0_3].unk8A;
        // if (D_global_asm_807F6000[temp_v0_3].object_type == 622) { // Disable interpolation for the ship in intro story. Propellers look kinda weird with interpolation still
        //     cur_drawn_model_skip_interpolation = TRUE;
        // }
        cur_model_transform_id_offset = 0;
        gSPMatrix(dl++, &identity_fixed_mtx, G_MTX_PUSH | G_MTX_MUL | G_MTX_MODELVIEW);
        dl = set_model_matrix_group(dl, NULL, FALSE, &pushed_matrix_group);
        // cur_drawn_model_skip_interpolation = FALSE;
        // 
        gSPDisplayList(dl++, osVirtualToPhysical(arg0->unkA0[var_v0]));
        gDPPipeSync(dl++);
        gSPDisplayList(dl++, osVirtualToPhysical(arg0->unkA8[var_v0]));
        // @recomp: Mtx untag
        gSPPopMatrix(dl++, G_MTX_MODELVIEW);
        if (pushed_matrix_group) {
            dl = pop_model_matrix_group(dl);
        }
        //
    }
    gDPPipeSync(dl++);
    gSPPopMatrix(dl++, G_MTX_MODELVIEW);
    return dl;
}

// @recomp: Sprite Matrix tagging init
u16 current_sprite_id = 0;
Struct80717D84 *func_global_asm_80714D08(void *sprite, f32 scale, f32 x, f32 y, f32 z, Actor *actor, s32 arg6, s32 boneIndex, u8 arg8);
void func_global_asm_806335B0(s32 arg0, u8 arg1, s32 boneIndex, f32* x, f32* y, f32* z);
u8 getBonePosition(Actor *actor, s32 boneIndex, f32 *x, f32 *y, f32 *z);
void func_global_asm_80714A38(u8 arg0);
extern u16 D_global_asm_807FDB36;

RECOMP_PATCH Struct80717D84 *func_global_asm_80714B84(void *sprite, f32 scale, s32 arg2, s32 boneIndex, u8 arg4) {
    f32 x, y, z;
    Struct80717D84 *sp;

    func_global_asm_806335B0(arg2, 1, boneIndex, &x, &y, &z);
    sp = func_global_asm_80714D08(sprite, scale, x, y, z, NULL, arg2, boneIndex, arg4);
    sp->sprite_index = MTXTAG_SPRITE + current_sprite_id;
    current_sprite_id++;
    return sp;
}

RECOMP_PATCH Struct80717D84 *func_global_asm_80714C08(void *sprite, f32 scale, Actor *actor, s32 boneIndex, u8 arg4) {
    f32 x, y, z;
    Struct80717D84 *sp;

    getBonePosition(actor, boneIndex, &x, &y, &z);
    if (!(actor->object_properties_bitfield & 0x200) && (actor->animation_state != NULL) && (D_global_asm_807FDB36 & 0x80)) {
        func_global_asm_80714A38(0x40);
    }
    sp = func_global_asm_80714D08(sprite, scale, x, y, z, actor, 0, boneIndex, arg4);
    sp->sprite_index = MTXTAG_SPRITE + current_sprite_id;
    current_sprite_id++;
    return sp;
}

RECOMP_PATCH Struct80717D84 *drawSpriteAtPosition(void* sprite, f32 scale, f32 x, f32 y, f32 z) {
    Struct80717D84 *sp;
    sp = func_global_asm_80714D08(sprite, scale, x, y, z, NULL, 0, 0, 0);
    sp->sprite_index = MTXTAG_SPRITE + current_sprite_id;
    current_sprite_id++;
    return sp;
}

s16 func_global_asm_806FD7A8(s32, u8);
extern void *_malloc(s32);
u8 func_global_asm_806FD894(s16 arg0);
void func_global_asm_80611690(void *arg0);
void func_global_asm_8061134C(void *arg0);
extern Actor *gCurrentPlayer;
extern u16 D_global_asm_80750AC8;
s32 textbox_interpolation_id = 1;
s32 last_interp_id_frame = 0;
extern s32 D_global_asm_8076AF10;

const u16 D_global_asm_8075A740[] = {
    0, 0, 0, 0
};
f32 func_global_asm_80612D1C(f32 arg0);
Gfx* printStyledText(Gfx* dl, s16 style, s16 x, s16 y, u8* string, u32 extraBitfield);
extern Actor *gCurrentActorPointer;

s32 getTextInterpolationId(s32 id) {
    return MTXTAG_TEXT + (gCurrentActorPointer->unk54 * 1000) + id;
}

RECOMP_PATCH void func_global_asm_806A370C(Gfx **arg0, AAD_global_asm_806A4DDC *arg1, Struct806A57C0_2 *arg2, Struct806A57C0_3 *arg3) {
    Gfx *dl;
    // wtf
    union {
        u16 h;
        u8 b;
        u8 pad[4];
    } spE8;
    f32 spA8[4][4];
    f32 sp68[4][4];
    f32 one;
    f32 temp_f12;
    f32 temp_f14;
    f32 temp_f0;
    f32 var_f2;
    struct806A57C0_3_sub10 *temp_s1;
    f32 sp4C;
    u8 pushed_matrix_group = FALSE;

    dl = *arg0;
    one = 1.f;
    spE8.h = D_global_asm_8075A740[0];
    temp_s1 = &arg3->unk10;
    guMtxIdentF(spA8);
    if ((temp_s1->unk0 != 0) && (arg3->unk2 != 0)) {
        temp_f12 = (arg3->unk0 * 0.5f) * 4.0f;
        temp_f14 = (arg2->unk18 * 0.5f) * 4.0f;
        guTranslateF(sp68, -temp_f12, -temp_f14, 0.0f);
        guMtxCatF(spA8, sp68, spA8);
        temp_s1->unk88 += 0.41887903213500977;
        if (temp_s1->unk0 & 8) {
            temp_f0 = temp_s1->unk88 * 0.5;
            if (temp_f0 <= MATH_2PI_F) {
                guRotateF(sp68, temp_f0 * 57.295776f, 0.0f, 0.0f, 1.0f);
                guMtxCatF(spA8, sp68, spA8);
            }
        }
        if (temp_s1->unk0 & 4) {
            if (MATH_2PI_F >= temp_s1->unk88) {
                var_f2 = func_global_asm_80612D1C(temp_s1->unk88 - MATH_HALFPI_F) + 1.0f;
                var_f2 = (0.25f * var_f2) + 1.0f;
            } else {
                var_f2 = 1.0f;
            }
            guScaleF(sp68, var_f2, var_f2, 1.0f);
            guMtxCatF(spA8, sp68, spA8);
        }
        if (temp_s1->unk0 & 2) {
            sp4C = (4.0f * func_global_asm_80612D1C(temp_s1->unk88)) * ((20.0f - RandClamp(40)) / 20.0f);
            guTranslateF(sp68, sp4C, (4.0f * func_global_asm_80612D1C(temp_s1->unk88)) * ((20.0f - RandClamp(40)) / 20.0f), 0.0f);
            guMtxCatF(spA8, sp68, spA8);
        }
        guTranslateF(sp68, temp_f12, temp_f14, 0.0f);
        guMtxCatF(spA8, sp68, spA8);
    }
    guScaleF(sp68, one, one, one);
    guMtxCatF(spA8, sp68, spA8);
    guTranslateF(sp68, (((f64) arg1->unk44) + arg3->unk4) * 4.0, (((f64) arg1->unk48) + arg3->unk8) * 4.0, 0.0f);
    guMtxCatF(spA8, sp68, spA8);
    guMtxF2L(spA8, &temp_s1->unk8[D_global_asm_807444FC]);
    
    if (arg3->initialized < 5) {
        if (arg3->initialized == 0) {
            arg3->initialized = TRUE;
            arg3->interpolation_id = textbox_interpolation_id;
            if (textbox_interpolation_id > 998) {
                textbox_interpolation_id = 1;
            } else {
                textbox_interpolation_id++;
            }
        }
        arg3->initialized++;
        cur_drawn_model_skip_interpolation = TRUE;
    }
    // Push mtx group
    cur_drawn_model_transform_id = getTextInterpolationId(arg3->interpolation_id);
    cur_model_transform_id_offset = 0;
    gSPMatrix(dl++, &identity_fixed_mtx, G_MTX_PUSH | G_MTX_MUL | G_MTX_MODELVIEW);
    dl = set_model_matrix_group(dl, NULL, FALSE, &pushed_matrix_group);
    cur_drawn_model_skip_interpolation = FALSE;
    //
    gSPMatrix(dl++, &temp_s1->unk8[D_global_asm_807444FC], (G_MTX_NOPUSH | G_MTX_LOAD) | G_MTX_MODELVIEW);
    gDPPipeSync(dl++);
    gDPSetPrimColor(dl++, 0, 0, 0x28, 0x28, 0xFF, arg3->unk3);
    spE8.b = temp_s1->unk2;
    dl = printStyledText(dl, 6, 0, 0, (u8 *) (&spE8), 0);
    // pop mtx
    gSPPopMatrix(dl++, G_MTX_MODELVIEW);
    if (pushed_matrix_group) {
        dl = pop_model_matrix_group(dl);
    }
    //
    *arg0 = dl;
}

Gfx* func_global_asm_806A3E9C(Gfx*, void*, s16);
void func_global_asm_806A3B78(Gfx **arg0, AAD_global_asm_806A4DDC *arg1, Struct806A57C0_2 *arg2, u8 arg3, u8 *arg4);
Gfx *func_global_asm_806A3C6C(Gfx *dl, Mtx *arg1, u8 arg2, s32 arg3);
extern Gfx** D_1000118;
extern Mtx D_20000C0;

// @recomp: Text Bubble Renderer
RECOMP_PATCH Gfx* func_global_asm_806A4284(Gfx* dl, Actor* arg1) {
    AAD_global_asm_806A4DDC* temp_s2;
    Struct806A57C0_2* var_s1;
    s32 i;
    u8 sp13B;
    s32 pad2;
    s16 sp132;
    s32 pad[3];
    u8 res;
    f32 spE0[4][4];
    f32 spA0[4][4];
    f32 temp_f20;
    Gfx* dl_0; // 98
    Gfx* dl_0_start; // 94
    f32 var_f2;
    f32 temp_f0;

    temp_s2 = arg1->AAD_as_array[0];
    var_s1 = temp_s2->unkC;
    sp13B = 0;
    dl_0 = _malloc(0x5000); // @recomp: Boost to 0x5000 (from 0x4000) to account for mtx tagging
    func_global_asm_8061134C(dl_0);
    dl_0_start = dl_0;
    gSPDisplayList(dl_0++, &D_1000118);
    gSPMatrix(dl_0++, &D_20000C0, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_PROJECTION);
    gDPPipeSync(dl_0++);
    gDPSetRenderMode(dl_0++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
    gDPSetCycleType(dl_0++, G_CYC_1CYCLE);
    gDPSetCombineMode(dl_0++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
    gSPLoadGeometryMode(dl_0++, 0);
    gSPSetGeometryMode(dl_0++, G_SHADE | G_SHADING_SMOOTH);
    sp132 = temp_s2->unk1C / 2;
    temp_f20 = (f32) ((1.0 - ((f64) temp_s2->unk40 * 0.5)) * (f64) temp_s2->unk20 * 1.899999976158142 * 4.0);
    guTranslateF(spE0, -48.0f, -32.0f, 0.0f);
    guScaleF(spA0, temp_f20, temp_f20, 1.0f);
    guMtxCatF(spE0, spA0, spE0);
    guTranslateF(spA0, arg1->position.f[0] * 4.0f, arg1->position.f[1] * 4.0f, 0.0f);
    guMtxCatF(spE0, spA0, spE0);
    guMtxF2L(spE0, &temp_s2->unk34[D_global_asm_807444FC]);
    gDPPipeSync(dl_0++);
    
    gDPSetScissor(dl_0++, G_SC_NON_INTERLACE,
        character_change_array[0].unk270[0],
        character_change_array[0].unk270[1],
        character_change_array[0].unk270[2],
        character_change_array[0].unk270[3]);
    dl_0 = func_global_asm_806A3E9C(dl_0, temp_s2, sp132);
    temp_f0 = temp_s2->unk20 * 150.0f * 2;
    var_f2 = MIN(150.0f, temp_f0);
    dl_0 = func_global_asm_806A3C6C(dl_0, &temp_s2->unk34[D_global_asm_807444FC], var_f2, sp132);
    if (temp_s2->unk1D != 0) {
        gDPPipeSync(dl_0++);
        gDPSetScissor(dl_0++, G_SC_NON_INTERLACE,
            temp_s2->unk44,
            temp_s2->unk48,
            temp_s2->unk4C,
            temp_s2->unk50);
    }
    for (i = 0; (i <= temp_s2->unk10) && (var_s1) && (!sp13B); i++) {
        gDPPipeSync(dl_0++);
        res = i == temp_s2->unk10;
        func_global_asm_806A3B78(&dl_0, temp_s2, var_s1, res, &sp13B);
        if (!sp13B) {
            var_s1 = var_s1->next;
        }
    }
    gDPPipeSync(dl_0++);
    if (temp_s2->unk1D != 0) {
        gDPSetScissor(dl_0++, G_SC_NON_INTERLACE,
            character_change_array[cc_player_index].unk270[0],
            character_change_array[cc_player_index].unk270[1],
            character_change_array[cc_player_index].unk270[2],
            character_change_array[cc_player_index].unk270[3]);
    }
    temp_s2->unk1C++;
    temp_s2->unk1C %= 16;
    gSPEndDisplayList(dl_0++);
    gSPDisplayList(dl++, dl_0_start);
    return dl;
}

typedef struct {
    /* 0x00 */ f32 unk0;
    /* 0x04 */ f32 unk4;
    /* 0x08 */ f32 unk8;
    /* 0x0C */ f32 unkC;
    /* 0x10 */ char pad10[4];
    /* 0x14 */ Mtx* unk14;
    /* 0x18 */ Mtx* unk18;
    /* 0x1C */ Vtx* unk1C;
    /* 0x20 */ u8 unk20;
    /* 0x21 */ char pad21[3];
    /* 0x24 */ void *unk24[4];
    /* 0x34 */ void *unk34[4];
    /* 0x44 */ u8 unk44;
    /* 0x45 */ u8 unk45;
    /* 0x46 */ u8 unk46;
    /* 0x47 */ u8 unk47;
    /* 0x48 */ s16 unk48;
    /* 0x4A */ s16 unk4A;
    /* 0x4C */ s16 unk4C;
    /* 0x4E */ u8 unk4E;
    /* 0x4F */ u8 unk4F;
    /* 0x50 */ char pad50[8];
    /* 0x58 */ s16 unk58;
    /* 0x5A */ u8 unk5A;
    /* 0x5B */ u8 unk5B;
    /* 0x5C */ u8 unk5C;
    /* 0x5D */ char pad5D[3];
} Struct80635468_arg1;

s32 func_global_asm_80626F8C(f32 arg0, f32 arg1, f32 arg2, f32 *arg3, f32 *arg4, s32 arg5, f32 arg6, s32 arg7);
void func_global_asm_80658C10(Struct80635468_arg1 *arg0, s16 arg1, s16 arg2, s16 arg3);
u8 func_global_asm_80658DAC(Struct80635468_arg1 *arg0);
s32 func_global_asm_80658E8C(f32, f32, f32, s32, s32);
void func_global_asm_80635468(Prop_unk24 *arg0, Struct80635468_arg1 *arg1);
void func_global_asm_806392BC(s16, void *, void *);
void func_global_asm_80639C04(Struct80635468_arg1 *, s32, void *, void *, void *, void *);
void func_global_asm_80639FC0(f32 arg0, f32 arg1, f32 arg2, f32 arg3, f32 arg4, f32 arg5, f32* arg6, f32* arg7, f32* arg8, u8 arg9);
extern s32 D_global_asm_807F5FD8;
extern u8 D_global_asm_807F6009;
extern s32 D_global_asm_807F600C;

RECOMP_PATCH Gfx *func_global_asm_80637B6C(Struct80635468_arg1 *arg0, Gfx *dl, f32 arg2, f32 arg3, f32 arg4, u8 arg5, s16 arg6) {
    f32 sp238[4][4];
    f32 sp1F8[4][4];
    f32 sp1F4;
    f32 sp1F0;
    f32 sp1EC;
    s32 sp1E8;
    s32 sp1E4;
    u8 sp1E3;
    s16 sp1E0;
    Mtx *var_s3;
    f32 sp1D8;
    f32 sp1D4;
    f32 sp1D0;
    f32 temp;
    s32 var_v0;
    f32 temp_f0_3;
    s32 i;
    s16 sp1BE;
    s32 pad;
    s32 sp1B4;
    s16 pad2;
    s16 sp1B0;
    u8 sp1AF;
    u8 sp1AE;
    u8 sp1AD;
    u8 sp1AC;
    u8 sp1AB;
    Mtx *sp1A4;
    u8 pushed_matrix_group = FALSE;

    sp1B4 = 0;
    sp1AF = 0;
    if (arg5 != 0) {
        sp1D0 = _sqrtf(SQ(arg2 - arg0->unk0) + SQ(arg3 - arg0->unk4) + SQ(arg4 - arg0->unk8));
        // get max distance
        sp1BE = func_global_asm_8065D0FC(recomp_filter_draw(arg0->unk58, 1.0f));
        if (sp1BE < sp1D0) {
            // cull if too far away
            return dl;
        }
    }
    if (arg0->unk5B != 0) {
        func_global_asm_8065EB10(arg0->unk5B, arg0->unk4C, arg0->unk0, arg0->unk4, arg0->unk8, recomp_filter_draw(arg0->unk58, 1.0f), sp1D0 / sp1BE, arg6);
    }
    if (arg5) {
        if (func_global_asm_80658E8C(arg0->unk0, arg0->unk4, arg0->unk8, 0, 0) != 0) {
            return dl;
        }
    } else {
        sp1BE = -0x8000;
        sp1D0 = 1.0f;
    }
    if (arg0->unk4E & 0x10) {
        return dl;
    }
    if (arg0->unk4E & 8) {
        func_global_asm_80626F8C(arg0->unk0, arg0->unk4, arg0->unk8, &sp1D8, &sp1D4, 0, 1.0f, cc_player_index);
        func_global_asm_80658C10(arg0, sp1D8, sp1D4, sp1D0);
        if (func_global_asm_80658DAC(arg0) != 0) {
            return dl;
        }
    }
    D_global_asm_807F5FD8 += 1;
    if (arg0->unk24[0] == NULL) {
        func_global_asm_80635468(D_global_asm_807F6000[(s16)func_global_asm_80659470(arg0->unk4C)].unk24, arg0);
    }

    func_global_asm_80639FC0(arg0->unk0, arg0->unk4, arg0->unk8, arg2, arg3, arg4, &sp1F4, &sp1F0, &sp1EC, arg0->unk4E & 4);
    guTranslateF(sp238, 0.0f, -arg0->unk48, 0.0f);
    if (sp1F4 != 0.0) {
        guRotateF(sp1F8, sp1F4, 1.0f, 0.0f, 0.0f);
        guMtxCatF(sp238, sp1F8, sp238);
    }
    if (sp1F0 != 0.0) {
        guRotateF(sp1F8, sp1F0, 0.0f, 1.0f, 0.0f);
        guMtxCatF(sp238, sp1F8, sp238);
    }
    if (arg0->unkC != 1.0) {
        guScaleF(sp1F8, arg0->unkC, arg0->unkC, arg0->unkC);
        guMtxCatF(sp238, sp1F8, sp238);
    }
    guTranslateF(sp1F8, arg0->unk0, arg0->unk4 + (arg0->unk48 * arg0->unkC), arg0->unk8);
    guMtxCatF(sp238, sp1F8, sp238);
    if (D_global_asm_807444FC != 0) {
        if (D_global_asm_807444FC != 1) {
            var_s3 = sp1A4;
        } else {
            var_s3 = &arg0->unk18[cc_player_index];
        }
    } else {
        var_s3 = &arg0->unk14[cc_player_index];
    }
    guMtxF2L(sp238, var_s3);

    gDPPipeSync(dl++);
    gSPClearGeometryMode(dl++, 0xFFFFFF);
    gSPMatrix(dl++, osVirtualToPhysical(var_s3), G_MTX_PUSH);

    temp_f0_3 = sp1BE * 3 / 4;
    if (sp1D0 < temp_f0_3) {
        sp1AE = 1;
        sp1B0 = 0xFF;
    } else {
        sp1AE = 0;
        var_v0 = (((sp1D0 - temp_f0_3) / ((f32) sp1BE - temp_f0_3)) * 255.0f);
        if ((s16)var_v0 >= 0x100) {
            var_v0 = 0xFF;
        }
        sp1B0 = 0xFF - var_v0;
    }
    func_global_asm_806392BC(arg0->unk4A, &sp1AE, &sp1B0);
    if ((arg5) && (arg0->unk5A == 0)) {
        func_global_asm_8065C334(arg0->unk0, arg0->unk4, arg0->unk8, 0, &sp1AD, &sp1AC, &sp1AB, arg6);
        temp = arg0->unk5C / 15.0f;
        sp1AD = sp1AD * temp;
        sp1AC = sp1AC * temp;
        sp1AB = sp1AB * temp;
    } else {
        sp1AC = -1;
        sp1AB = -1;
        sp1AD = -1;
    }
    cur_drawn_model_transform_id = MTXTAG_PROPSPRITE + arg0->unk4C;
    cur_model_transform_id_offset = 0;
    gSPMatrix(dl++, &identity_fixed_mtx, G_MTX_PUSH | G_MTX_MUL | G_MTX_MODELVIEW);
    dl = set_model_matrix_group(dl, NULL, FALSE, &pushed_matrix_group);
    cur_drawn_model_skip_interpolation = FALSE;
    for (i = 0; i < arg0->unk20; i++) {
        func_global_asm_80639C04(arg0, i, &sp1E8, &sp1E4, &sp1E3, &sp1E0);
        gDPPipeSync(dl++);
        if (arg0->unk4E & 2) {
            if (sp1AE) {
                gDPSetRenderMode(dl++, G_RM_PASS, G_RM_AA_ZB_TEX_EDGE2);
            } else {
                gDPSetRenderMode(dl++, G_RM_PASS, G_RM_ZB_XLU_SURF2);
            }
        } else if (arg0->unk4E & 1) {
            gDPSetRenderMode(dl++, G_RM_PASS, G_RM_ZB_XLU_SURF2);
        } else {
            gDPSetRenderMode(dl++, G_RM_PASS, G_RM_XLU_SURF2);
        }
        gDPSetCycleType(dl++, G_CYC_2CYCLE);
        if ((arg0->unk4E & 1) || (arg0->unk4E & 2)) {
            gSPSetGeometryMode(dl++, G_ZBUFFER | G_SHADE | G_FOG | G_SHADING_SMOOTH);
        } else {
            gSPSetGeometryMode(dl++, G_SHADE | G_FOG | G_SHADING_SMOOTH);
        }
        gDPSetPrimColor(dl++, 0, sp1E0, sp1AD, sp1AC, sp1AB, sp1B0);
        switch (sp1E3) {
            case 0:
                gDPSetCombine(dl++, 0x001197FF, 0xFFFFFE38);
                gSPTexture(dl++, 0xFFFF, 0xFFFF, 0, 0, G_ON);
                if ((D_global_asm_807F6009 != 2) || (sp1E8 != D_global_asm_807F600C)) {
                    switch (arg0->unk46) {
                        case 0:
                            gDPLoadTextureBlock_4b(dl++,
                                OS_PHYSICAL_TO_K0(sp1E8), arg0->unk47,
                                arg0->unk44, arg0->unk45, 0,
                                G_TX_CLAMP, G_TX_CLAMP, 0, 0, 0, 0);
                            if (arg0->unk47 == 2) {
                                gDPLoadTLUT_pal16(dl++, 0, arg0->unk34[i]);
                                sp1AF = 1;
                                gDPSetTextureLUT(dl++, G_TT_RGBA16);
                            }
                            break;
                        case 1:
                            gDPLoadTextureBlock(dl++,
                                OS_PHYSICAL_TO_K0(sp1E8), arg0->unk47, G_IM_SIZ_8b,
                                arg0->unk44, arg0->unk45, 0,
                                G_TX_CLAMP, G_TX_CLAMP, 0, 0, 0, 0);
                            if (arg0->unk47 == 2) {
                                gDPLoadTLUT(dl++, 256, 256, arg0->unk34[i]);
                                sp1AF = 1;
                                gDPSetTextureLUT(dl++, G_TT_RGBA16);
                            }
                            break;
                        case 2:
                            gDPLoadTextureBlock(dl++,
                                OS_PHYSICAL_TO_K0(sp1E8), arg0->unk47, G_IM_SIZ_16b,
                                arg0->unk44, arg0->unk45, 0,
                                G_TX_CLAMP, G_TX_CLAMP, 0, 0, 0, 0);
                            break;
                        case 3:
                            gDPLoadTextureBlock(dl++,
                                OS_PHYSICAL_TO_K0(sp1E8), arg0->unk47, G_IM_SIZ_32b,
                                arg0->unk44, arg0->unk45, 0,
                                G_TX_CLAMP, G_TX_CLAMP, 0, 0, 0, 0);
                            break;
                    }
                }
                D_global_asm_807F600C = sp1E8;
                break;
            case 1:
                gDPSetCombineLERP(dl++, TEXEL1, TEXEL0, PRIM_LOD_FRAC, TEXEL0, TEXEL1, TEXEL0, PRIM_LOD_FRAC, TEXEL0, COMBINED, 0, PRIMITIVE, 0, COMBINED, 0, 0, COMBINED);
                gSPTexture(dl++, 0xFFFF, 0xFFFF, 0, 0, G_ON);
                gDPSetTextureLOD(dl++, G_TL_TILE);
                gDPSetTextureImage(dl++, G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, sp1E8);
                gDPSetTile(dl++, G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, 0, 0, 0, 0, 0, 0);
                gDPLoadSync(dl++);
                gDPLoadBlock(dl++, G_TX_LOADTILE, 0, 0, ((arg0->unk44 * arg0->unk45 * 2) >> 1) - 1,
                             CALC_DXT(arg0->unk44, G_IM_SIZ_16b_BYTES));
                gDPSetTextureImage(dl++, G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, sp1E4);
                gDPTileSync(dl++);
                gDPSetTile(dl++, G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0x100, G_TX_LOADTILE, 0, 0, 0, 0, 0, 0, 0);
                gDPLoadSync(dl++);
                gDPLoadBlock(dl++, G_TX_LOADTILE, 0, 0, ((arg0->unk44 * arg0->unk45 * 2) >> 1) - 1,
                             CALC_DXT(arg0->unk44, G_IM_SIZ_16b_BYTES));
                gDPSetTile(dl++, G_IM_FMT_RGBA, G_IM_SIZ_16b, arg0->unk44 / 4, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 0,
                           0, G_TX_CLAMP, 0, 0);
                gDPSetTileSize(dl++, G_TX_RENDERTILE, 2, 2, ((arg0->unk44 - 1) * 4) + 2, ((arg0->unk45 - 1) * 4) + 2);
                gDPSetTile(dl++, G_IM_FMT_RGBA, G_IM_SIZ_16b, arg0->unk44 / 4, 0x100, 1, 0, G_TX_CLAMP, 0, 0,
                           G_TX_CLAMP, 0, 0);
                gDPSetTileSize(dl++, 1, 2, 2, ((arg0->unk44 - 1) * 4) + 2, ((arg0->unk45 - 1) * 4) + 2);
                D_global_asm_807F600C = 0;
                break;
        }
        gSPVertex(dl++, osVirtualToPhysical(&arg0->unk1C[sp1B4]), 4, 0);
        gSP2Triangles(dl++, 0, 1, 2, 0, 0, 2, 3, 0);
        sp1B4 += 4;
        D_global_asm_807F6009 = 2;
    }
    gSPPopMatrix(dl++, G_MTX_MODELVIEW);
    if (pushed_matrix_group) {
        dl = pop_model_matrix_group(dl);
    }

    gSPPopMatrix(dl++, G_MTX_MODELVIEW);
    gDPPipeSync(dl++);
    gDPSetCycleType(dl++, G_CYC_1CYCLE);
    gDPSetTextureLOD(dl++, G_TL_LOD);
    if (sp1AF) {
        gDPSetTextureLUT(dl++, G_TT_NONE);
    }
    gDPSetDepthSource(dl++, G_ZS_PIXEL);
    return dl;
}

extern Gfx *displayImage(Gfx *dl, u16 textureIndex, s32 arg3, u32 codec, s32 width, s32 height, s16 x, s16 y, f32 xScale, f32 yScale, s32 arg11, f32 arg12);
extern Gfx *printText(Gfx *dl, s16 x, s16 y, f32 scale, u8 *string);
extern Gfx *func_global_asm_8070068C(Gfx *dl);
extern s32 getCenterOfString(s16 renderStyle, u8* string);
extern u8 *D_global_asm_80750338[];

// @recomp: Text overlay draw code (1)
RECOMP_PATCH Gfx *func_global_asm_8069F904(Gfx *dl, Actor *arg1) {
    f32 temp_f0;
    u8 pushed_matrix_group = FALSE;

    temp_f0 = arg1->unkE0;
    gSPDisplayList(dl++, &D_1000118);
    gDPSetCombineMode(dl++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
    gDPSetRenderMode(dl++, G_RM_CLD_SURF, G_RM_CLD_SURF2);
    gSPMatrix(dl++, &D_20000C0, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_PROJECTION);
    gDPPipeSync(dl++);
    gDPSetPrimColor(dl++, 0, 0, 0xFF, 0xFF, 0xFF, arg1->y_rotation);
    // push mtx
    cur_drawn_model_transform_id = getActorMatrixTag(arg1);
    cur_model_transform_id_offset = 0x10;
    gSPMatrix(dl++, &identity_fixed_mtx, G_MTX_PUSH | G_MTX_MUL | G_MTX_MODELVIEW);
    dl = set_model_matrix_group(dl, NULL, FALSE, &pushed_matrix_group);
    //
    dl = displayImage(dl, (arg1->unk15F + 0x61), 0, 2, 0x28, 0x33, arg1->x_position, arg1->y_position, temp_f0, temp_f0, 0, 0.0f);
    // pop mtx
    gSPPopMatrix(dl++, G_MTX_MODELVIEW);
    if (pushed_matrix_group) {
        dl = pop_model_matrix_group(dl);
    }
    //
    return dl;
}

// @recomp: Text overlay draw code (2)
RECOMP_PATCH Gfx *func_global_asm_8069FA40(Gfx *dl, Actor *arg1) {
    f32 spFC;
    f32 spBC[4][4];
    f32 sp7C[4][4];
    Mtx *sp78;
    char sp68[0x10];
    s16 center;
    u8 style;
    u8 sp64;
    u8 pushed_matrix_group = FALSE;

    spFC = 0.3 * MAX(0.01, arg1->unkE0 - 2.0);
    sp64 = FALSE;
    gSPDisplayList(dl++, &D_1000118);
    gDPSetCombineMode(dl++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
    gDPSetRenderMode(dl++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
    gDPPipeSync(dl++);
    gDPSetPrimColor(dl++, 0, 0, 0xFF, 0xFF, 0xFF, arg1->y_rotation);
    dl = func_global_asm_8070068C(dl);
    sp78 = _malloc(sizeof(Mtx));
    func_global_asm_8061134C(sp78);
    style = 1;
    switch (arg1->unk15F) {
        case 0:
            _sprintf(sp68, "ROUND");
            break;
        case 30:
        case 31:
        case 32:
        case 33:
        case 34:
        case 35:
        case 36:
        case 37:
        case 38:
            sp64 = TRUE;
            // TODO: WTF
            _sprintf(sp68, D_global_asm_80750338[arg1->unk15F << 0]);
            break;
        default:
            if (arg1->unk15F & 0x80) {
                spFC = 4.0 - spFC;
                style = 3;
                _sprintf(sp68, "%d", 0xA - (arg1->unk15F & 0x3F));
            } else if (arg1->unk15F & 0x40) {
                spFC = 4.0 - spFC;
                style = 3;
                _sprintf(sp68, "%d", 8 - (arg1->unk15F & 0x3F));
            } else {
                _sprintf(sp68, "%d", arg1->unk15F);
            }
            break;
    }
    center = getCenterOfString(style, sp68);
    if (sp64) {
        arg1->x_position = (320 - center) << 1;
    }
    guScaleF(spBC, spFC, spFC, 1);
    guTranslateF(sp7C, arg1->x_position + (2.0 * center), arg1->y_position * 4.0f, 0);
    guMtxCatF(spBC, sp7C, spBC);
    guMtxF2L(spBC, sp78);
    gSPMatrix(dl++, sp78, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    // push mtx
    cur_drawn_model_transform_id = getActorMatrixTag(arg1);
    cur_model_transform_id_offset = 0x10;
    gSPMatrix(dl++, &identity_fixed_mtx, G_MTX_PUSH | G_MTX_MUL | G_MTX_MODELVIEW);
    dl = set_model_matrix_group(dl, NULL, FALSE, &pushed_matrix_group);
    //
    dl = printStyledText(dl, style, center * -2, 0, sp68, 0);
    // pop mtx
    gSPPopMatrix(dl++, G_MTX_MODELVIEW);
    if (pushed_matrix_group) {
        dl = pop_model_matrix_group(dl);
    }
    //
    return dl;
}

typedef struct global_asm_struct_58 GlobalASMStruct58;
typedef struct {
    f32 unk0;
    f32 unk4;
    f32 unk8;
    f32 unkC;
    f32 unk10;
    u8 unk14[0x46 - 0x14];
    s16 unk46;
    s16 unk48;
    s16 unk4A;
    s16 unk4C;
    s16 unk4E;
    s16 unk50[2];
    u8 unk54[0x60 - 0x54];
    u8 unk60;
    u8 unk61;
    u8 unk62;
    u8 unk63;
    u8 unk64;
    u8 unk65;
    u8 unk66;
    u8 unk67;
    u8 unk68;
} GlobalASMStruct58_unk0;
struct global_asm_struct_58 {
    GlobalASMStruct58_unk0 *unk0;
    f32 unk4;
    f32 unk8;
    f32 unkC;
    f32 unk10;
    f32 unk14;
    f32 unk18;
    s32 unk1C[1];
    s32 unk20;
    Gfx *unk24[2];
    s32 unk2C;
    s32 unk30;
    s32 unk34;
    s32 unk38;
    s32 unk3C;
    s16 unk40;
    s16 unk42;
    s16 unk44;
    s16 unk46;
    s16 unk48;
    s8 unk4A;
    s8 unk4B;
    u8 unk4C;
    s8 unk4D;
    s16 unk4E;
    GlobalASMStruct58 *next;
};
extern GlobalASMStruct58 *D_global_asm_807F93C0;
void* func_global_asm_8065FEB8(void*, GlobalASMStruct58*);
void func_global_asm_80660070(Gfx *, s32, GlobalASMStruct58*);
typedef struct Struct80748A90 {
    void *unk0;
    void *unk4;
    void *unk8;
    void *unkC[2];
    u8 unk14[4];
} Struct80748A90;

extern Struct80748A90 D_global_asm_80748A90[];

// @recomp: Fluids
RECOMP_PATCH Gfx* func_global_asm_8065FD88(Gfx* dl, u8 arg1, u8 arg2) {
    GlobalASMStruct58* var_s0;
    void* temp_v0;
    s32 i;
    u8 pushed_matrix_group;

    var_s0 = D_global_asm_807F93C0;
    i = 0;
    while (var_s0) {
        if ((var_s0->unk4C & 1) && (arg2 == D_global_asm_80748A90[var_s0->unk0->unk66].unk14[2])) {
            dl = func_global_asm_8065FEB8(dl, var_s0);
            // Tag Matrix
            cur_drawn_model_transform_id = MTXTAG_FLUIDS + i;
            i++;
            pushed_matrix_group = FALSE;
            cur_model_transform_id_offset = 0;
            cur_model_uses_vertex_interp = TRUE;
            gSPMatrix(dl++, &identity_fixed_mtx, G_MTX_PUSH | G_MTX_MUL | G_MTX_MODELVIEW);
            dl = set_model_matrix_group(dl, NULL, FALSE, &pushed_matrix_group);
            //
            gSPDisplayList(dl++, osVirtualToPhysical(var_s0->unk24[D_global_asm_807444FC]));
            if (!(var_s0->unk4C & 4)) {
                func_global_asm_80660070(var_s0->unk24[D_global_asm_807444FC], var_s0->unk1C[D_global_asm_807444FC], var_s0);
                var_s0->unk4C |= 4;
            }
            // Pop Matrix
            gSPPopMatrix(dl++, G_MTX_MODELVIEW);
            cur_model_uses_vertex_interp = FALSE;
            if (pushed_matrix_group) {
                dl = pop_model_matrix_group(dl);
            }
            //
        }
        var_s0 = var_s0->next;
    }
    return dl;
}

void getAnimationArg16(s16 *arg0);

void setHandLockdown(Actor *ac) {
    if ((ac->unk58 == 98) || (ac->unk58 == 136) || (ac->unk58 == 137)) {
        // @recomp: Tag Barrels set their hand state every frame. We can just ditch this
        return;
    }
    set_actor_interpolation_lockdown(ac, 1);
}

RECOMP_PATCH void func_global_asm_806187E8(Actor *arg0) {
    s16 sp1E;

    getAnimationArg16(&sp1E);
    arg0->unk146_s16 |= sp1E;
    setHandLockdown(arg0);
}

RECOMP_PATCH void func_global_asm_80618820(Actor *arg0) {
    s16 sp1E;

    getAnimationArg16(&sp1E);
    arg0->unk146_s16 &= ~sp1E;
    setHandLockdown(arg0);
}

RECOMP_PATCH void func_global_asm_8068A764(Actor *arg0, u8 arg1) {
    arg0->unk146_s16 |= 1 << arg1;
    setHandLockdown(arg0);
}

RECOMP_PATCH void func_global_asm_8068A784(Actor *arg0, u8 arg1) {
    arg0->unk146_s16 &= ~(1 << arg1);
    setHandLockdown(arg0);
}

tuple_f *func_global_asm_80612840(tuple_f *output, tuple_f a0, tuple_f a1);
tuple_f *func_global_asm_806128A8(tuple_f *output, tuple_f arg1, tuple_f arg4);
tuple_f *func_global_asm_80612910(tuple_f *output, tuple_f input, f32 scale);
tuple_f *func_global_asm_80612970(tuple_f *output, tuple_f arg1);     
void func_global_asm_80612AD8(tuple_f *arg0, f32 arg1, f32 arg2, f32 arg3);
f32 func_global_asm_80612A14(tuple_f a0, tuple_f a1);
Gfx *func_global_asm_80703374(Gfx *dl, u8 r, u8 g, u8 b, u8 a);
typedef struct Struct80754AF0 {
    f32 unk0;
    f32 unk4;
    rgba unk8;
    s8 unkC;
    u8 unkD;
    u8 unkE;
    u8 unkF;
} Struct80754AF0;

extern Struct80754AF0 D_global_asm_80754AF0[];
extern Mtx D_2000180;
typedef struct Struct807FD808 {
    tuple_f position;
    rgba unkC;
} Struct807FD808;

// @recomp: Solar Flare
RECOMP_PATCH Gfx* func_global_asm_80701098(Gfx* dl, Struct807FD808 arg1, u8 arg5) {
    tuple_f sp1B4;
    tuple_f sp1A8;
    tuple_f sp19C;
    tuple_f sp190;
    tuple_f sp184;
    tuple_f sp178;
    tuple_f sp16C;
    tuple_f sp160;
    f64 var_f0;
    f32 var_f20;
    s16 i;
    s32 pad2[17];
    f32 sp108;
    s32 pad3[4];
    f32 spF4;
    f32 spF0;
    f32 var_f24;
    f32 var_f26;
    s32 pad4[2];
    tuple_f spD0;
    tuple_f spC4;
    u8 pushed_matrix_group = FALSE;

    spF0 = arg5 / 5.0;
    func_global_asm_80612AD8(&sp160, arg1.position.x, arg1.position.y, arg1.position.z);
    func_global_asm_80612AD8(&sp178, character_change_array->unk21C, character_change_array->unk220, character_change_array->unk224);
    func_global_asm_80612AD8(&sp16C, character_change_array->unk234, character_change_array->unk238, character_change_array->unk23C);
    func_global_asm_80612840(&spD0, sp16C, sp178);
    func_global_asm_80612970(&sp19C, spD0);
    func_global_asm_80612910(&spD0, sp19C, 5000.0f);
    func_global_asm_806128A8(&sp1B4, sp178, spD0);
    func_global_asm_80612840(&spD0, sp160, sp178);
    func_global_asm_80612970(&sp190, spD0);
    var_f20 = MAX(0.00001, func_global_asm_80612A14(sp190, sp19C));
    func_global_asm_80612910(&spC4, sp190, 5000.0f);
    func_global_asm_80612910(&spD0, spC4, 1.0f / var_f20);
    func_global_asm_806128A8(&sp160, sp178, spD0);
    func_global_asm_80612840(&sp1A8, sp160, sp1B4);
    if (var_f20 < 0.0f) {
        return dl;
    } else {
        for (i = 0; i < 12; i++) {
            // Mtx Tag
            cur_drawn_model_transform_id = MTXTAG_SOLAR_FLARE;
            pushed_matrix_group = FALSE;
            cur_model_transform_id_offset = i;
            gSPMatrix(dl++, &identity_fixed_mtx, G_MTX_PUSH | G_MTX_MUL | G_MTX_MODELVIEW);
            dl = set_model_matrix_group(dl, NULL, FALSE, &pushed_matrix_group);
            //
            var_f0 = MAX(0.0, var_f20 - 0.75);
            var_f26 = var_f0 * 4.0;
            var_f24 = MAX(0.85, var_f26);
            switch (i) {
                case 11:
                    sp108 = 640.0f;
                    spF4 = 480.0f;
                    var_f0 = MAX(0.0, var_f20 - 0.9);
                    var_f26 = var_f0 * 10.0;
                    var_f24 = MAX(0.85, var_f26);
                    break;
                case 0:
                case 12:
                    var_f0 = MAX(0.0, var_f20 - 0.25);
                    var_f26 = var_f0 * 1.333;
                    var_f24 = MAX(0.85, var_f26);
                    func_global_asm_80626F8C(sp160.x, sp160.y, sp160.z, &sp108, &spF4, 0, 4.0f, 0);
                    break;
                default:
                    func_global_asm_80612910(&spD0, sp1A8, D_global_asm_80754AF0[i].unk4);
                    func_global_asm_806128A8(&sp184, sp1B4, spD0);
                    func_global_asm_80626F8C(sp184.x, sp184.y, sp184.z, &sp108, &spF4, 0, 4.0f, 0);
                    break;
            }
            gDPPipeSync(dl++);
            gDPSetPrimColor(dl++, 0, 0,
                (arg1.unkC.red / 255.0) * D_global_asm_80754AF0[i].unk8.red * var_f24,
                (arg1.unkC.green / 255.0) * D_global_asm_80754AF0[i].unk8.green * var_f24,
                (arg1.unkC.blue / 255.0) * D_global_asm_80754AF0[i].unk8.blue * var_f24,
                D_global_asm_80754AF0[i].unk8.alpha * var_f26 * spF0);
            dl = displayImage(dl,
                D_global_asm_80754AF0[i].unkC + 0x4D,
                G_IM_FMT_IA, G_IM_SIZ_8b,
                0x40, 0x40,
                sp108, spF4,
                D_global_asm_80754AF0[i].unk0 * 4 * spF0, D_global_asm_80754AF0[i].unk0 * 4 * spF0,
                0, 0);
            // Mtx pop
            gSPPopMatrix(dl++, G_MTX_MODELVIEW);
            if (pushed_matrix_group) {
                dl = pop_model_matrix_group(dl);
            }
            //
        }
        gDPPipeSync(dl++);
        if (var_f20 > 0.97) {
            dl = func_global_asm_80703374(dl, arg1.unkC.red, arg1.unkC.green, arg1.unkC.blue, (var_f20 - 0.97) * 33.0 * 128.0);
            gSPTexture(dl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
            gDPSetCombineMode(dl++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);

        }
    }
    return dl;
}

extern u8 D_global_asm_80754BC0;

extern Struct807FD808 D_global_asm_807FD808[] ;
extern u8 D_global_asm_807FD838[][3];

RECOMP_PATCH Gfx* func_global_asm_807007B8(Gfx* dl) {
    s16 i;
    u8* temp_v1;
    u8 temp_v0;
    Struct807FD808 *temp_t2;

    if (D_global_asm_80754BC0 != 0) {
        gDPPipeSync(dl++);
        dl = func_global_asm_8070068C(dl);
        gSPMatrix(dl++, &D_2000180, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gDPPipeSync(dl++);
        gSPLoadGeometryMode(dl++, 0);
        gSPSetGeometryMode(dl++, G_SHADE | G_SHADING_SMOOTH);
        gDPSetCycleType(dl++, G_CYC_1CYCLE);
        gDPSetTexturePersp(dl++, G_TP_PERSP);
        gDPSetTextureFilter(dl++, G_TF_BILERP);
        gSPTexture(dl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
        gDPSetCombineMode(dl++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
        gDPSetRenderMode(dl++, G_RM_CLD_SURF, G_RM_CLD_SURF2);
        gDPSetColorDither(dl++, G_CD_DISABLE);
        gDPSetAlphaDither(dl++, G_AD_DISABLE);
        for (i = 0; i < D_global_asm_80754BC0; i++) {
            if (D_global_asm_807FD838[D_global_asm_807444FC ^ 1][i]) {
                cur_drawn_model_transform_id = MTXTAG_SOLAR_FLARE + (0xC * i);
                dl = func_global_asm_80701098(dl, D_global_asm_807FD808[i], D_global_asm_807FD838[D_global_asm_807444FC ^ 1][i]--);
            }
            
        }
        gDPSetColorDither(dl++, G_CD_MAGICSQ);
        gDPSetAlphaDither(dl++, G_AD_PATTERN);
    }
    return dl;
}