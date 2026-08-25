#include "patches.h"
#include "PR/os_message.h"
#include "PR/os_exception.h"
#include "PR/rcp.h"
#include "PR/sched.h"
#include "misc_funcs.h"
#include "enums.h"

extern u8 queue_dk64_fb_store;

typedef struct tuple_f {
    union {
        struct {
            f32 x;
            f32 y;
            f32 z;
        };
        f32 f[3];
    };
} tuple_f;

typedef struct rgb {
    u8 red;
    u8 green;
    u8 blue;
} rgb;


typedef struct rgba {
    u8 red;
    u8 green;
    u8 blue;
    u8 alpha;
} rgba;

u32 func_global_asm_806119A0(void); // Having this here to prevent having to declare it for RANDNUM()
#define SQ(x) ((x) * (x))
#define ACTOR_UNINITIALIZED(actor) (!(actor->object_properties_bitfield & 0x10))
#define MATH_HALFPI_F 1.5707964f
#define MATH_PI_F 3.1415927f
#define MATH_2PI_F 6.283185482f
#define RANDNUM() (func_global_asm_806119A0() & 0x7FFFFFFF)
#define RandClamp(a) ((RANDNUM() >> 0xF) % a)
#define RandChance(a) (RandClamp(1000) > (1000 - ((s32)((a) * 1000))))
#define ABS_D(d) (((d) > 0.0) ? (d) : -(d))
#define ABS_F(d) (((d) > 0.0f) ? (d) : -(d))
#define ABS(d) (((d) > 0) ? (d) : -(d))
#define CLAMP(value, min, max) ((value) < (min) ? (min) : MIN(max, value))

#define ALIGN_ON_SCREEN 0x01
#define ALIGN_POSITION_LEFT 0x02
#define ALIGN_POSITION_RIGHT 0x04
#define ALIGN_HIDING_IN_OVERSCAN 0x08
#define ALIGN_NO_INTERP 0x10
#define ALIGN_LEFT (ALIGN_POSITION_LEFT | ALIGN_ON_SCREEN)
#define ALIGN_RIGHT (ALIGN_POSITION_RIGHT | ALIGN_ON_SCREEN)

typedef enum interpolationIDs {
    MTXTAG_DEFAULT,
    MTXTAG_GLOBAL,
    MTXTAG_FRAMEBUFFERTRANSITION,
    MTXTAG_TAG3,
    MTXTAG_SKYBOXBLEND,
    MTXTAG_CAMERAPROJECTION,
    MTXTAG_MAINMENU_BARREL = 0xF00,
    MTXTAG_ACTORS = 0x1000, // 0x100 allocated per actor
    MTXTAG_PROP = 0x101000,
    MTXTAG_PROPSPRITE = 0x101800,
    MTXTAG_SPRITE = 0x102000,
    MTXTAG_TEXT = 0x200000,
    MTXTAG_FLUIDS = 0x400000,
    MTXTAG_SOLAR_FLARE = 0x400100,
} interpolationIDs;

typedef struct AnimationStateUnk0_0 {
    f32 unk0;
    u16 unk4;
    u8 pad6[0x12 - 0x6];
    u8 unk12;
} AnimationStateUnk0_0;

// TODO: What is this actually?
typedef struct animation_state_unk0 {
    AnimationStateUnk0_0* unk0;
    f32 unk4; // Used
    f32 unk8;
    s16 unkC;
    s16 unkE;
    s16 unk10; // Used
    s16 unk12;
    f32 unk14;
    f32 unk18;
    s16 unk1C;
    s16 unk1E;
    s16 unk20;
    s16 unk22;
    f32 unk24; // Used
    f32 unk28;
    f32 unk2C;
    f32 unk30;
    f32 unk34;
} AnimationStateUnk0;

typedef struct {
    s8 unk0;
    s8 unk1;
    union {
        s16 unk2;
        u16 unk2_u16; // used in K Rool Diddy code
    };
    s16 unk4;
    union {
        s16 unk6;
        u16 unk6_u16;
    };
    // Note: This is correctly sized, if you get references to unk8, use unk1C[1].unk0 instead
} AnimationStateUnk1C;

// TODO: Fill this in properly
typedef struct {
    u8 unk0;
    u8 unk1;
    u8 unk2;
    u8 unk3;
    f32 unk4;
    f32 unk8;
    f32 unkC;
} AnimationStateUnk24;

// See boss func_dk64_boot_8002FB7C
typedef struct animation_state_unk20 {
    u8 unk0;
    u8 unk1;
    u8 unk2;
    u8 unk3;
    f32 unk4;
    f32 unk8;
    f32 unkC;
} AnimationStateUnk20;

typedef struct actor_collision ActorCollision;
typedef struct actor Actor;
typedef struct otherSpriteControl otherSpriteControl;

typedef struct {
    u8 pad0[0x8];
    f32 unk8;
} PaaD0;

// Bonus func_bonus_800253E4
typedef struct {
    Actor* unk0;
} BaaD2;

typedef struct TempAAD {
    s8 unk0;
    u8 pad0[0x14 - 0x1];
    s16 unk14;
    s16 unk16;
    s16 unk18;
    s16 unk1A;
    u8 pad1[0x24 - 0x1C];
    s8 unk24;
} TempAAD;

typedef struct struct806A57C0_2 Struct806A57C0_2;
typedef struct struct806A57C0_3 Struct806A57C0_3;

typedef struct struct806A57C0_3_sub10 {
    s16 unk0;
    u8 unk2;
    u8 unk3;
    void* unk4;
    Mtx unk8[2];
    f32 unk88;
} struct806A57C0_3_sub10;

struct struct806A57C0_3 {
    s16 unk0;
    u8 unk2; // Used
    u8 unk3;
    f32 unk4; // Used
    f32 unk8; // Used
    s32 unkC;
    struct806A57C0_3_sub10 unk10;
    Struct806A57C0_3* unkA0; // Used
    u16 interpolation_id;
    u8 padA6;
    u8 initialized;
};

struct struct806A57C0_2 {
    u8 unk0; // Used
    u8 unk1;
    u8 unk2;
    u8 unk3;
    s32 unk4;
    s32 unk8;
    Struct806A57C0_3* unkC;
    u16 unk10; // Used
    u16 unk12;
    s32 unk14;
    f32 unk18;
    Struct806A57C0_2* next; // 0x1C
};

typedef struct {
    u16 unk0; // Used
    s16 unk2;
    s32 unk4;
    s32 unk8;
    Struct806A57C0_2* unkC; // Used
    u8 unk10;
    u8 unk11;
    u8 unk12;
    u8 unk13;
    f32 unk14; // Used
    f32 unk18; // Used
    u8 unk1C;
    u8 unk1D; // Used
    u8 unk1E;
    u8 unk1F;
    f32 unk20; // Used
    s32 unk24;
    s32 unk28;
    s32 unk2C;
    s32 unk30;
    Mtx* unk34; // Used
    s16 unk38;
    s16 unk3A;
    s32 unk3C;
    f32 unk40; // Used
    f32 unk44; // Used
    f32 unk48; // Used
    f32 unk4C; // Used
    f32 unk50;
    u8 unk54;
    u8 unk55; // Used
} AAD_global_asm_806A4DDC;

// TODO: This appears to be a "texture renderer" according to ScriptHawk, needs a better name though. Linked list at Actor->unk158
typedef struct global_asm_struct_60 GlobalASMStruct60;

struct global_asm_struct_60 {
    s32* unk0; // Used
    GlobalASMStruct60* unk4; // Used? Might be wrong datatype, see func_global_asm_80688638
    u8 unk8; // Used
    u8 unk9;
    s16 unkA; // Used
    s16 unkC; // Used - Texture index?
    s16 unkE;
    f32 unk10; // Used
    f32 unk14; // Used
    s16 unk18; // Used
    s16 unk1A; // Used
    s16 unk1C; // Used
    s16 unk1E; // Used
    u8 unk20; // Used
    u8 unk21; // Used
    u8 unk22; // Used
    u8 unk23;
    GlobalASMStruct60* next; // at 0x24
};

typedef struct {
    s32 unk0; // TODO: Proper members
    s32 unk4;
    s32 unk8;
    s32 unkC;
    s32 unk10;
    s32 unk14;
} FloorTriangle;

typedef struct ActorAnimationState_unk170 {
    u8 pad0[0x80];
} ActorAnimationState_unk170;

typedef struct actor_animation_state {
    AnimationStateUnk0* unk0;
    AnimationStateUnk0* unk4;
    AnimationStateUnk0* unk8;
    AnimationStateUnk0* unkC;
    f32 unk10;
    Mtx* bone_arrays[2]; // at 0x14, camera, update bone positions // TODO: Proper type
    AnimationStateUnk1C* unk1C;
    AnimationStateUnk20* unk20; // See boss func_dk64_boot_8002FB7C
    AnimationStateUnk24* unk24;
    s16 unk28;
    s16 unk2A;
    s16 unk2C;
    s16 unk2E;
    s16 unk30;
    s16 unk32;
    union {
        struct {
            f32 scale_x; // at 0x34
            f32 scale_y; // at 0x38
            f32 scale_z; // at 0x3C
        };
        f32 scale[3];
    };
    f32 unk40;
    f32 unk44;
    f32 unk48;
    f32 unk4C;
    f32 unk50;
    f32 unk54;
    f32 unk58;
    u16 unk5C;
    s16 unk5E;
    s32 unk60;
    u16 unk64; // Used
    u16 unk66;
    s32 unk68;
    s32 unk6C;
    s32(*unk70)(Actor*);
    s32 unk74;
    s32 unk78;
    s32 unk7C;
    f32 unk80;
    s32 unk84;
    s32 unk88;
    f32 unk8C;
    AnimationStateUnk0 unk90[4];
    ActorAnimationState_unk170 unk170[1];
} ActorAnimationState;

typedef struct SnideAaD180 {
    u8 control_byte;
    u8 screen;
    u8 turned_in_count;
    u8 pad[0x3];
    u8 unk6;
    u8 unk7;
    s32 unk8;
    f32 unkC;
    f32 unk10;
    f32 unk14;
    s8 previous_y;
    s8 minigame_menu_unlocked;
    u8 unturned_count;
    u8 pad2[0x5];
} SnideAaD180;

typedef struct player_additional_actor_data {
    s16 unk0; // Used
    s16 unk2;
    f32 unk4;
    s16 unk8; // Used
    s16 unkA; // Used, Actor->y_rotation copied here
    s16 unkC; // Used
    s16 unkE; // Used
    s16 unk10; // Used
    s16 unk12; // Used
    s16 unk14; // Used
    union {
        struct {
            s8  unk16_s8;
            s8  unk17; // Used, set to current file selection?
        };
        s16 unk16;
    };
    s16 unk18; // Used
    u16 unk1A;
    f32 unk1C; // Used
    f32 unk20; // Used
    s16 unk24; // Used
    s16 unk26;
    s16 unk28; // Used
    s16 unk2A;
    f32 unk2C; // Used
    f32 unk30; // Used
    f32 unk34; // Used
    f32 unk38; // Used
    s16 unk3C;
    s16 unk3E; // Used
    s16 unk40;
    u8 unk42; // Used, related to distance from floor
    u8 unk43;
    u8 unk44;
    u8 unk45; // Used, VehicleAdditionalActorData
    u8 unk46;
    u8 unk47;
    s16 unk48; // Used
    s16 unk4A;
    s16 unk4C; // Used
    s16 unk4E;
    s16 unk50; // Used
    s16 unk52;
    f32 unk54; // Used, y velocity?
    s32 unk58; // Used, related to D_global_asm_80767CC0
    s32 unk5C;
    s32 unk60;
    s16 unk64; // Used
    s16 unk66;
    f32 unk68;
    f32 unk6C; // Used
    s16 unk70; // Used, y rotation?
    s16 unk72;
    f32 unk74;
    f32 unk78; // used
    f32 unk7C; // used
    s16 unk80; // used
    s16 unk82;
    s32 unk84;
    Actor* vehicle_actor_pointer; // Used
    u8  unk8C; // Used
    u8  unk8D; // Used
    u8 unk8E; // Used
    u8 unk8F;
    u8 unk90;
    u8 unk91; // Used
    u8 unk92;
    u8 unk93;
    f32 unk94; // Used
    f32 unk98;
    f32 unk9C; // Used
    f32 unkA0; // Used
    f32 unkA4; // Used
    f32 unkA8; // Used
    union {
        s32 unkAC; // Used
        f32 unkAC_f32; // Used
    }; // TODO: Deconflict
    union {
        struct {
            s16 unkB0;
            s16 unkB2; // Used
        };
        f32 unkB0_f32;
    }; // TODO: Deconflict
    f32 unkB4; // Used
    f32 unkB8; // Used
    s16 unkBC; // Used
    s16 unkBE;
    s16 unkC0;
    s16 unkC2; // Used
    s32 unkC4;
    s16 unkC8;
    u8  unkCA; // Used
    s8  unkCB;
    s16 unkCC; // Used
    s16 unkCE;
    s32 unkD0;
    s16 unkD4; // Used
    s16 unkD6;
    u8 unkD8; // Used
    u8 unkD9;
    u8 unkDA;
    u8 unkDB;
    Actor* unkDC; // Used
    f32 unkE0; // Used
    f32 unkE4; // Used
    u8 unkE8; // Used
    u8 unkE9;
    s16 unkEA; // Used
    s16 unkEC;
    u8 unkEE;
    u8 unkEF; // Used
    union { // TODO: No idea what's going on here, both are used
        float unkF0_f32;
        u8 unkF0;
        u8 unkF0_u8[4];
    };
    union { // TODO: No idea what's going on here, both are used
        float unkF4_f32;
        u8 unkF4;
        u8 unkF4_u8[4];
    };
    u8 unkF8; // Used
    u8 unkF9;
    u8 unkFA;
    u8 unkFB;
    union { // TODO: Deconflict
        u8 unkFC; // Used
        s32 unkFC_s32; // Used
    };
    s32 unk100;
    Actor* unk104; // Used, Actor*?
    f32 unk108; // Used
    s8 unk10C; // Used
    u8 unk10D;
    s16 unk10E;
    s16 unk110;
    s16 unk112;
    s16 unk114;
    s16 unk116;
    s16 unk118;
    s16 unk11A;
    u8 unk11C; // Used, VehicleAdditionalActorData?
    u8 unk11D;
    s16 unk11E; // Used
    s32 unk120;
    f32 unk124; // Used
    s32 unk128;
    s32 unk12C;
    Actor* unk130;
    s8 unk134;
    s8 unk135;
    s8 unk136;
    s8 unk137;
    f32 unk138;
    f32 unk13C;
    f32 unk140;
    f32 unk144;
    f32 unk148;
    f32 unk14C; // Used
    f32 unk150; // Used
    s16 unk154; // Used
    s16 unk156;
    Actor* unk158;
    s32 unk15C;
    s32 unk160;
    s32 unk164;
    s32 unk168;
    s32 unk16C;
    s32 unk170;
    s32 unk174;
    s32 unk178;
    s32 unk17C;
    s32 unk180;
    s32 unk184;
    s32 unk188;
    s32 unk18C;
    s32 unk190;
    s32 unk194;
    s32 unk198;
    s16 unk19C; // Used
    s16 unk19E; // Used
    u16 unk1A0; // Used
    u8  unk1A2;
    u8  unk1A3; // Used
    u8  unk1A4; // Used, playerIndex?
    u8  unk1A5;
    u8  unk1A6;
    u8  unk1A7;
    Actor* unk1A8;
    Actor* unk1AC; // TODO: Is this correct?
    Actor* unk1B0; // Used
    s32 unk1B4;
    u16 unk1B8; // Used
    s16 unk1BA;
    f32 unk1BC; // Used
    union {
        struct {
            f32 unk1C0; // Used, x scale
            f32 unk1C4; // Used, y scale
            f32 unk1C8; // Used, z scale
        };
        f32 scale1C0[3];
    };
    union {
        struct {
            f32 unk1CC; // Used, x scale
            f32 unk1D0; // Used, y scale
            f32 unk1D4; // Used, z scale
        };
        f32 scale1CC[3];
    };
    f32 unk1D8; // Used
    f32 unk1DC; // Used
    f32 unk1E0; // Used
    s32 unk1E4;
    f32 unk1E8;
    u8 unk1EC;
    u8 unk1ED;
    s16 unk1EE; // Used
    s32 unk1F0; // Used, bitfield // TODO: Proper bitfield syntax
    s32 unk1F4;
    s32 unk1F8;
    u8 unk1FC; // Used
    u8 unk1FD;
    u16 unk1FE; // Used
    s16 unk200; // Used
    s16 unk202;
    f32 unk204;
    f32 unk208;
    f32 unk20C;
    f32 unk210; // Used, x
    f32 unk214; // Used, y
    f32 unk218; // Used, z
    s16 unk21C;
    u16 unk21E; // Used
    f32 unk220; // Used, x
    f32 unk224; // Used, y
    f32 unk228; // Used, z
    f32 unk22C; // Used, x
    f32 unk230; // Used, y
    f32 unk234; // Used, z
    f32 unk238; // Used
    u16 unk23C; // Used
    u8 unk23E;
    u8 unk23F; // Used
    u8 unk240; // Used
    u8 unk241;
    u16 unk242; // Used, map?
    u8 unk244;
    u8 unk245; // Used
    s8 unk246; // Used func_global_asm_806CA26C
    s8 unk247; // Used func_global_asm_806CA26C
    s8 unk248;
    s8 unk249;
    u8 unk24A;
    s8 unk24B; // Used
    u16 unk24C; // Used // TODO: s16? func_global_asm_806CA1B4 uses this as u16
    u16 unk24E; // Used // TODO: s16? func_global_asm_806CA1B4 uses this as u16
    s32 unk250;
    otherSpriteControl* unk254;
    f32 unk258; // Used
    f32 unk25C; // Used
    void* unk260; // Used (multiplayer)
    s32 unk264; // Used
    u8 unk268; // Used
} PlayerAdditionalActorData;

typedef struct {
    // Small header on top of DisplayList Pointer
    u8 unk0[0x1C - 0x0];
    s16 unk1C;
    u8 pad1E[2];
    u8 unk20;
    u8 unk21[0x38 - 0x21];
    f32 unk38;
} Actor_unk0;

typedef struct KRoolChunkyAAD178 {
    Actor* unk0;
    u8 pad4[0x14 - 0x4];
    u8 unk14;
    u8 unk15;
    u8 unk16;
    u8 unk17;
    u8 unk18;
    u8 unk19;
} KRoolChunkyAAD178;

typedef struct {
    Actor* unk0[4]; // Used
    u8 unk10; // Used, index into unk0
    s8 unk11;
    s8 unk12;
    s8 unk13;
} Actor114;

typedef struct {
    s32 unk0; // TODO: Proper members
    s32 unk4;
    f32 unk8;
    u8 unkC;
    s8 unkD;
    s16 unkE;
    s16 unk10;
    s16 unk12;
} Actor118;

typedef struct {
    union {
        f32 unk0; // Used
        s32 unk0_s32; // Used // TODO: We might have another aaD situation here...
    };
    union {
        f32 unk4; // Used
        s32 unk4_s32; // Used // TODO: We might have another aaD situation here...
    };
    union {
        f32 unk8; // Used
        s32 unk8_s32; // Used // TODO: We might have another aaD situation here...
    };
    union {
        f32 unkC; // Used
        s32 unkC_s32; // Used // TODO: We might have another aaD situation here...
    };
    f32 unk10; // Used
    union {
        f32 unk14; // Used
        s32 unk14_s32; // Used // TODO: We might have another aaD situation here...
    };
    s32 unk18;
    s32 unk1C;
    s32 unk20;
    s32 unk24;
    s32 unk28;
    s32 unk2C;
    s32 unk30;
    s32 unk34;
    s32 unk38;
    s32 unk3C;
    s32 unk40;
    s32 unk44;
    s32 unk48;
    s32 unk4C;
    s32 unk50;
    s32 unk54;
    s32 unk58;
    s32 unk5C;
    s32 unk60;
    s32 unk64;
    s32 unk68;
    s32 unk6C;
} Actor124;

typedef struct ledge_info_8c LedgeInfo8C;

struct ledge_info_8c {
    f32 unk0;
    f32 unk4;
    f32 unk8;
    f32 unkC; // Used
    s32 unk10;
    s32 unk14;
    s32 unk18;
    s32 unk1C;
    s32 unk20;
    s16 unk24;
    u8 unk26; // Used
    u8 unk27; // Used
    LedgeInfo8C* next;
};

typedef struct ledge_info_90 LedgeInfo90;

struct ledge_info_90 {
    f32 unk0;
    f32 unk4;
    f32 unk8;
    s32 unkC;
    s32 unk10;
    s32 unk14;
    s32 unk18;
    LedgeInfo90* next;
};

typedef struct {
    f32 unk0; // Used
    s32 unk4;
    s32 unk8;
    f32 unkC; // Used
    s32 unk10;
    s32 unk14;
    f32 unk18; // Used
    s32 unk1C;
    s32 unk20;
    s32 unk24;
    s32 unk28;
    s32 unk2C;
    s32 unk30;
    s32 unk34;
    s32 unk38;
    s32 unk3C;
    s32 unk40;
    s32 unk44;
    s32 unk48;
    s32 unk4C;
    s32 unk50;
    s16 unk54;
    s8 unk56;
    s8 unk57;
} LedgeInfo98;

typedef struct {
    s32 unk0;
    s32 unk4;
    s32 unk8;
    s32 unkC;
    s32 unk10;
    s32 unk14;
    s32 unk18;
    s32 unk1C;
    s32 unk20;
    s32 unk24;
    s16 unk28;
    s8 unk2A; // Used
    s8 unk2B;
} LedgeInfoA0;

typedef struct ledge_info {
    s32 unk0;
    s32 unk4;
    s32 unk8;
    s32 unkC;
    s32 unk10;
    f32 unk14;
    f32 unk18;
    f32 unk1C;
    f32 unk20;
    u8 unk24;
    u8 unk25;
    u8 unk26;
    u8 unk27;
    u32 unk28;
    u32 unk2C;
    u32 unk30;
    u32 unk34;
    u32 unk38;
    u32 unk3C;
    u32 unk40;
    u32 unk44;
    u16 unk48;
    u16 unk4A;
    u32 unk4C;
    u32 unk50;
    u32 unk54;
    u32 unk58;
    u32 unk5C;
    u32 unk60;
    u16 unk64;
    u16 unk66; // Used
    u32 unk68;
    u8 unk6C;
    u8 unk6D;
    u8 unk6E;
    u8 unk6F;
    Mtx* unk70;
    Mtx* unk74[2];
    s32 unk7C;
    s32 unk80; // object_timer gets written here
    s32 unk84;
    s32 unk88; // object_timer gets written here
    LedgeInfo8C* unk8C; // Linked List
    LedgeInfo90* unk90;
    s32 unk94;
    LedgeInfo98* unk98; // Array
    s32 unk9C;
    LedgeInfoA0* unkA0; // Array
    s32 unkA4;
    s32 unkA8;
    u8 unkAC;
} LedgeInfo;

typedef struct other_additional_actor_data {
    s32 unk0;
    s32 unk4;
    s16 unk8;
    s16 unkA;
    f32 unkC;
    f32 unk10;
    f32 unk14;
    s16 unk18;
    s16 unk1A;
    s32 unk1C;
} OtherAdditionalActorData;

typedef struct another_additional_actor_data {
    s32 unk0;
    s32 unk4;
    f32 unk8;
    s32 unkC;
    s16 unk10;
    u8 unk12;
    u8 unk13; // Used in func_global_asm_806925AC
} AnotherAdditionalActorData;

typedef struct {
    u8 unk0[0x27 - 0x0];
    u8 unk27;
    u8 unk28[0x36 - 0x28];
    u8 unk36;
} RaaD_unk20_unk4;

typedef struct {
    Actor* unk0;
    RaaD_unk20_unk4* unk4;
} RaaD_unk20;

typedef struct race_additional_actor_data {
    s16 unk0;
    s16 unk2; // used
    f32 unk4;
    s16 unk8;
    s16 unkA;
    f32 unkC;
    f32 unk10;
    f32 unk14;
    s16 unk18;
    s16 unk1A;
    u16 unk1C;
    u8 unk1E;
    s8 unk1F;
    RaaD_unk20* unk20; // Array
    u8  unk24;
    u8  unk25;
    u8  unk26; // Used
    s8  unk27;
    u8 unk28; // Used
    s8 unk29; // Used
    s16 unk2A;
    s32 unk2C;
    Actor* unk30;
    u8 unk34;
    u8 unk35;
    u8 unk36;
    u8 unk37;
    s32 unk38;
    u8 unk3C;
    u8 unk3D;
    u8 unk3E;
    u8 unk3F;
    s16 unk40;
    u8 unk42; // Used
    u8 unk43;
    u8 unk44;
    u8 unk45; // Used
    u8 pad46[2];
    s16* unk48;
    u8 pad4C[4];
    Mtx unk50[2];
} RaceAdditionalActorData;

typedef struct race_additional_actor_data2 {
    u8  unk0; // Used
    u8  unk1;
    u8  unk2;
    u8  unk3;
    u8 pad2[0x24 - 0x4];
    u8 unk24;
    u8 pad25[0x36 - 0x25];
    u8 unk36; // Used
} RaceAdditionalActorData2;

typedef struct yaad5 {
    f32 unk0; // Used
    f32 unk4; // Used
    f32 unk8; // Used
    Actor* unkC; // Used
    Actor* unk10; // Used
    s16 unk14; // Used
    s16 unk16; // Used
    u16 unk18;
    s16 unk1A; // Used
    s16 unk1C; // Used
    s8 unk1E; // Used
} YetAnotherAdditionalActorData5;

typedef struct yet_another_additional_actor_data {
    Actor* unk0;
    u16 unk4;
    u16 unk6;
    s32 unk8;
    s32 unkC;
    s32 unk10;
    s32 unk14;
    s32 unk18;
    s32 unk1C;
    s16 unk20;
    s16 unk22;
    f32 unk24;
    s16 unk28;
    s16 unk2A;
    s32 unk2C;
    s32 unk30;
    s32 unk34;
    s16 unk38;
    s16 unk3A;
    s32 unk3C;
} YetAnotherAdditionalActorData;

typedef struct yaad4 {
    s32 unk0;
    s32 unk4;
    s32 unk8;
    s32 unkC;
    s16 unk10; // Used
    s16 unk12; // Used
    s16 unk14; // Used
} YetAnotherAdditionalActorData4;

typedef struct {
    u8 unk0;
    u8 unk1; // Used
} A178_806A18A8;

typedef struct actor_17C {
    union {
        s32 unk0; // TODO: Actor*? Boss overlay Might require a union?
        s16 unk0_s16[2]; // TODO: Actor*? Boss overlay Might require a union?
        u16 unk0_u16[2]; // TODO: Actor*? Boss overlay Might require a union?
    };
    u8 unk4;
    u8 unk5;
    s16 unk6;
    s16 unk8;
} Actor17C;

typedef struct {
    u8 unk0;
    u8 unk1;
} Struct80027840;

typedef struct race_actor_178 {
    s32 unk0;
    s32 unk4;
    s32 unk8;
    s32 unkC;
    s32 unk10;
    f32 unk14;
} RaceActor178;

typedef struct boss_actor_178 {
    s32 unk0; // Used
} BossActor178;

typedef struct actor_178 {
    s16 unk0; // Used
    s16 unk2;
    u8 unk4;
    u8 unk5;
    s16 unk6;
    s16 unk8;
    u8 unkA;
    u8 unkB;
    u8 unkC;
    u8 unkD;
    u8 unkE;
    u8 unkF;
    u16 unk10; // Used
    u16 unk12; // Used
    u8 unk14;
} Actor178;

typedef struct menu_additional_actor_data {
    //u8 pad0[0x17];
    f32 unk0;
    f32 unk4;
    f32 unk8;
    s16 unkC;
    s16 unkE;
    s16 unk10;
    s8  unk12;
    s8  unk13;
    s8 unk14;
    s8 unk15;
    s8  unk16;
    s8 unk17;
} MenuAdditionalActorData;

typedef struct {
    Actor* unk0;
    Actor* unk4;
    s16 unk8;
    s16 unkA;
    s16 unkC;
    s16 unkE;
    s16 unk10;
    s16 unk12;
    s16 unk14;
    s16 unk16;
    s16 unk18;
} CameraPaad_unk44;

typedef struct {
    Actor *unk0;
    u8 pad4[0x8 - 0x4];
    u16 unk8;
    u16 unkA;
    u8 padC[0x10 - 0xC];
    s16 unk10;
    u8 pad12[0x20 - 0x12];
    f32 unk20;
    f32 unk24;
    f32 unk28;
    f32 unk2C;
    f32 unk30;
    f32 unk34;
    f32 unk38;
    f32 unk3C;
    f32 unk40;
    CameraPaad_unk44 * unk44;
    s32 unk48;
    u8 pad4C[0x6C - 0x4C];
    f32 unk6C;
    f32 unk70;
    u8 pad74[0x78 - 0x74];
    f32 unk78;
    f32 unk7C;
    f32 unk80;
    f32 unk84;
    f32 unk88;
    f32 unk8C;
    f32 unk90;
    f32 unk94;
    f32 unk98;
    u8 pad9C[0x9E - 0x9C];
    s16 unk9E;
    f32 unkA0;
    f32 unkA4;
    u8 padA8[0xAC - 0xA8];
    s32 unkAC;
    s16 unkB0;
    s16 unkB2;
    u8 padB4[0xB8 - 0xB4];
    f32 unkB8;
    u8 padBC[0xC0 - 0xBC];
    s32 unkC0;
    u8 padC4[0xCC - 0xC4];
    tuple_f unkCC;
    tuple_f unkD8;
    u8 padE4[0xEE - 0xE4];
    u8 unkEE;
    u8 unkEF;
    u8 unkF0;
    u8 unkF1;
    u8 unkF2;
    u8 unkF3;
    u8 unkF4;
    u8 unkF5;
    u8 unkF6;
    u8 unkF7;
    u8 padF8[0xFA - 0xF8];
    u8 unkFA;
    u8 unkFB;
    u8 padFC[0xFF - 0xFC];
    u8 unkFF;
} CameraPaad;

struct actor {
    Actor_unk0* unk0;
    ActorAnimationState* animation_state;
    u32* unk8; // Current bone array Pointer // TODO: Proper type
    s16 unkC;
    s16 unkE;
    u32 unk10;
    u32 unk14;
    u32 unk18;
    u32 unk1C;
    u32 unk20;
    f32 unk24;
    u32 unk28;
    f32 unk2C;
    u32 unk30;
    u32 unk34;
    u32 unk38;
    u32 unk3C;
    u32 unk40;
    u32 unk44;
    u32 unk48;
    u32 unk4C;
    void *unk50;
    u32 unk54;
    Actors unk58;
    u16 interactable; // Bitfield at 0x5C
    u16 unk5E;
    union {
        struct {
            u32 OPBBit0 : 1;
            u32 OPBBit1 : 1;
            u32 OPBBit2 : 1;
            u32 OPBBit3 : 1;
            u32 OPBBit4 : 1;
            u32 OPBBit5 : 1;
            u32 OPBBit6 : 1;
            u32 OPBBit7 : 1;
            u32 OPBBit8 : 1;
            u32 OPBBit9 : 1;
            u32 OPBBit10 : 1;
            u32 OPBBit11 : 1;
            u32 OPBBit12 : 1;
            u32 OPBBit13 : 1;
            u32 OPBBit14 : 1;
            u32 OPBBit15 : 1;
            u32 OPBBit16 : 1;
            u32 OPBBit17 : 1;
            u32 OPBBit18 : 1;
            u32 OPBBit19 : 1;
            u32 OPBBit20 : 1;
            u32 OPBBit21 : 1;
            u32 OPBBit22 : 1;
            u32 OPBBit23 : 1;
            u32 OPBBit24 : 1;
            u32 OPBBit25 : 1;
            u32 OPBBit26 : 1;
            u32 OPBBit27 : 1;
            u32 OPBBit28 : 1;
            u32 OPBBit29 : 1;
            u32 OPBBit30 : 1;
            u32 OPBBit31 : 1;
        };
        u32 object_properties_bitfield; // at 0x60
    };
    s32 unk64; // Another bitfield
    u16 unk68;
    u16 unk6A; // is_grounded?
    u16 unk6C;
    s16 unk6E[1]; // TODO: How many elements?
    s16 unk70;
    u16 unk72;
    f32 unk74;
    u8 unk78;
    u8 unk79;
    u8 unk7A;
    u8 unk7B;
    union {
        struct {
            f32 x_position; // at 0x7C
            f32 y_position; // at 0x80
            f32 z_position; // at 0x84
        };
        tuple_f position;
    };
    f32 unk88;
    f32 unk8C;
    f32 unk90;
    f32 unk94;
    f32 unk98;
    f32 unk9C;
    f32 unkA0;
    f32 floor; // at 0xA4
    f32 unkA8; // Used
    f32 unkAC;
    s8 unkB0;
    s8 padB1[3];
    f32 distance_from_floor; // at 0xB4
    f32 unkB8; // at 0xB8 // velocity?
    f32 unkBC; // at 0xBC
    f32 y_velocity; // at 0xC0
    f32 y_acceleration; // at 0xC4
    f32 terminal_velocity; // at 0xC8
    u8 unkCC; // Used, bone index?
    u8 unkCD;
    s16 unkCE;
    u8 unkD0;
    u8 unkD1; // Used
    u8 unkD2;
    u8 unkD3;
    u32 unkD4;
    s16 unkD8; // Used
    s16 unkDA; // Used
    s16 unkDC; // Used
    s16 unkDE;
    f32 unkE0;
    s16 x_rotation; // at 0xE4
    s16 y_rotation; // at 0xE6
    s16 z_rotation; // at 0xE8
    s16 unkEA;
    s16 unkEC; // Something to do with shadow opacity
    s16 unkEE; // TODO: Interacts with rotation, maybe a copy of it?
    s16 unkF0;
    s16 unkF2;
    s16 unkF4;
    s16 unkF6;
    s16 unkF8;
    s16 unkFA; // Used
    u8 unkFC;
    u8 unkFD;
    u8 unkFE;
    u8 unkFF;
    f32 unk100; // Used
    f32 unk104; // Used
    f32 unk108; // Used
    s16 unk10C;
    u8 unk10E; // Used
    u8 unk10F;
    u8 locked_to_pad; // at 0x110
    u8 unk111;
    u8 unk112;
    u8 unk113;
    Actor114* unk114;
    Actor118* unk118;
    Actor* unk11C;
    f32 unk120;
    Actor124* unk124;
    s16 shadow_opacity; // at 0x128, max value 0xFF
    s16 draw_distance; // at 0x12A
    s16 unk12C; // Used
    u8 unk12E;
    u8 unk12F; // Used
    u8 unk130; // Used
    u8 unk131; // Used
    union {
        s16 unk132;
        u16 unk132_u16;
    };
    s16 health;
    s16 unk136;
    u32 unk138;
    ActorCollision* collision_queue_pointer; // at 0x13C
    LedgeInfo* ledge_info_pointer; // at 0x140
    u8 noclip_byte; // at 0x144
    u8 unk145;
    union {
        u16 unk146; // used (0x147 hand state? 0x146 seems to be u16)
        s16 unk146_s16; // used func_global_asm_8068A764
    };
    void* unk148[2]; // Used
    FloorTriangle* unk150;
    u8 control_state; // at 0x154
    u8 control_state_progress; // at 0x155
    u8 unk156;
    u8 unk157;
    GlobalASMStruct60* unk158; // Texture renderer linked list
    s16 unk15C;
    u8 unk15E;
    u8 unk15F; // Used, set to 0x01, 0x06, 0x0B in some cases
    f64 unk160; // TODO: Probably not correct
    s16 unk168; // Used
    u8 unk16A;
    u8 unk16B;
    u8 unk16C;
    u8 unk16D;
    u8 unk16E;
    u8 unk16F;
    u8 unk170;
    u8 unk171;
    u8 unk172;
    u8 unk173;
    union {
        void* AAD_as_array[4];
        struct {
            union {
                void* additional_actor_data; // For when you don't know which one it's using
                OtherAdditionalActorData* OaaD;
                AnotherAdditionalActorData* AaaD;
                RaceAdditionalActorData* RaaD;
                RaceAdditionalActorData2* RaaD2;
                YetAnotherAdditionalActorData* YaaD;
                YetAnotherAdditionalActorData4* YaaD4;
                YetAnotherAdditionalActorData5* YaaD5;
                MenuAdditionalActorData* MaaD;
                PlayerAdditionalActorData* PaaD;
                CameraPaad* CaaD;
                PaaD0* PaaD0; // TODO: Figure this out
                BaaD2* BaaD2; // TODO: Figure this out
                TempAAD* TaaD; // TODO: Figure this out (race overlay)
                void* additional_data_pointer; //legacy
            };
            union {
                void* unk178;
                Actor178* a178;
                RaceActor178* race178;
                BossActor178* boss178;
                A178_806A18A8* a178_806A18A8;
            };
            union {
                Actor17C* unk17C;
            };
            union {
                void* unk180;
                SnideAaD180* snide_aad_180;
            };
        };
    };
    void* unk184;
};

typedef struct {
    s32 unk0;
    s32 unk4;
    s32 unk8;
    s32 unkC;
    s32 unk10;
    s32 unk14;
    s32 unk18;
    s32 unk1C;
    s32 unk20;
    s32 unk24;
    s32 unk28;
    s32 unk2C;
    s32 unk30;
    s32 unk34;
    s32 unk38;
    s32 unk3C;
} CharacterChange8Array;

typedef struct {
    s16 unk0;
    s16 unk2;
    s16 unk4;
    s16 unk6;
    s16 unk8;
    s16 unkA;
    s16 unkC;
    s16 unkE;
} CharacterChange250;

typedef struct {
    f32 unk0;
    u16 unk4;
    u16 unk6;
    u8 unk8;
    u8 unk9;
    u8 unkA;
    u8 unkB;
} CharacterChange2DC;

typedef struct {
    u8      does_player_exist; // bitfield? 0x00
    u8      unk1;
    u8      unk2;
    u8      unk3;
    Actor * playerPointer;    // 0x04
    Mtx     unk8[2];
    Mtx     unk88[2];
    u8      pad108[0x188 - 0x108];
    u16     unk188;
    u8      pad18A[0x190 - 0x18A];
    LookAt  unk190[2];
    Hilite  unk1D0[2];
    Hilite  unk1F0[2]; // Unsure on struct. Is a 0x10-sized struct
    union {
        struct {
            f32     look_at_eye_x; // 0x210 maybe an array?
            f32     look_at_eye_y; // 0x214
            f32     look_at_eye_z; // 0x218
        };
        f32 look_at_eye[3];
    };
    f32     unk21C; // Used
    f32     unk220; // Used
    f32     unk224; // Used
    f32     look_at_at_x; // 0x228 maybe an array?
    f32     look_at_at_y; // 0x22C
    f32     look_at_at_z; // 0x230
    f32     unk234; // Used
    f32     unk238; // Used
    f32     unk23C; // Used
    f32     look_at_up_x; // 0x240 maybe an array?
    f32     look_at_up_y; // 0x244
    f32     look_at_up_z; // 0x248;
    f32     unk24C;
    CharacterChange250 unk250[2];
    s16     unk270[4];
    union {
        struct {
            s16     unk278;
            s16     unk27A;
        };
        s16 unk278_arr[2];
    };
    f32     fov_y; // 0x27C
    f32     unk280;
    f32     near; // 0x284
    f32     far; // 0x288
    f32     unk28C;
    s16     chunk; // 0x290
    s16     unk292;
    OSContPad* unk294; // Used
    OSContPad* new_controller_inputs; // bitfield 0x298
    s16     action_initiated; // 0x29C
    s16     unk29E;
    void* unk2A0;
    s32     unk2A4;
    s32     unk2A8; // Used
    s32     unk2AC;
    s32     unk2B0; // Used
    tuple_f unk2B4;
    u8      unk2C0; // Used
    u8      unk2C1; // Used
    u8      unk2C2; // Used
    u8      unk2C3; // Used
    f32     unk2C4; // Used
    s16     unk2C8; // Used
    s16     unk2CA;
    s16     unk2CC;
    s16     unk2CE;
    f32     unk2D0;
    f32     unk2D4;
    f32     unk2D8;
    CharacterChange2DC      unk2DC;
    u8      unk2E8;
    u8      unk2E9;
    u8      unk2EA;
    u8      unk2EB;
    u8      unk2EC;
    u8      unk2ED;
    u8      unk2EE;
    u8      unk2EF;
} CharacterChange;


extern CharacterChange* character_change_array;

typedef struct {
    s16 unk0;
    s16 unk2;
    s16 unk4;
    s16 unk6;
} Struct807FBB70_unk278;
typedef struct {
    u8 unk0;
    u8 unk1;
    u8 unk2;
    u8 unk3;
    f32 unk4;
    f32 unk8;
    f32 unkC;
    s16 unk10;
    s16 unk12;
    u8 unk14;
    u8 unk15;
    u8 unk16;
    u8 unk17;
    u8 unk18;
    u8 unk19;
    u8 unk1A;
    u8 unk1B;
    u8 unk1C;
    u8 pad1D[0x24 - 0x1D];
    u8 unk24;
    u8 unk25;
    u8 pad26[0x2C - 0x26];
    u8 unk2C;
    u8 pad2D[0x38 - 0x2D];
    u8 unk38;
    u8 unk39;
    u16 unk3A;
    f32 unk3C;
    f32 unk40;
    f32 unk44;
    s32 unk48;
    s32 unk4C;
    s32 unk50;
    s32 unk54;
    s32 unk58;
    s32 unk5C;
    s32 unk60;
    s32 unk64;
    s32 unk68;
    s32 unk6C;
    s32 unk70;
    s32 unk74;
    s32 unk78;
    Actor *unk7C[4];
    s32 unk8C;
    u8 unk90;
    u8 unk91;
    s16 unk92;
    s16 unk94;
    u8 unk96;
    u8 unk97;
    s32 unk98;
    s32 unk9C;
    s32 unkA0;
    s32 unkA4;
    s32 unkA8;
    s32 unkAC;
    s32 unkB0;
    s32 unkB4;
    s32 unkB8;
    s32 unkBC;
    s32 unkC0;
    s32 unkC4;
    s32 unkC8;
    s32 unkCC;
    s32 unkD0;
    s32 unkD4;
    f32 unkD8;
    f32 unkDC;
    f32 unkE0;
    u8 unkE4;
    u8 unkE5;
    u8 unkE6;
    u8 unkE7;
    f32 unkE8;
    s32 unkEC;
    f32 unkF0;
    f32 unkF4;
    u8 padF8[0x1F8 - 0xF8];
    s32 unk1F8;
    Actor *unk1FC;
    u8 unk200;
    u8 unk201;
    s16 unk202;
    Actor *unk204[1];
    u8 pad208[0x243 - 0x208];
    u8 unk243[1];
    u8 unk244[1];
    s8 unk245;
    s8 unk246;
    s8 unk247;
    u8 pad248[0x254 - 0x248];
    u8 unk254;
    u8 unk255;
    u8 unk256;
    u8 unk257;
    s32 unk258[1];
    s32 unk25C;
    s32 unk260;
    s32 unk264;
    s32 unk268;
    s32 unk26C;
    s32 unk270;
    s32 unk274;
    Struct807FBB70_unk278 *unk278[1];
} GlobalASMStruct35;


typedef struct global_asm_struct_83 GlobalASMStruct83;
struct global_asm_struct_83 {
    Actor* unk0;
    u32 unk4;
    Actor* unk8;
    GlobalASMStruct83 *next;
};
typedef struct prop_model_50_b8 PropModel50_B8;
struct prop_model_50_b8 {
    s32 unk0[4];
    s32 unk10[4];
    s32 unk20[4];
    s32 unk30;
    s32 unk34;
    s16 unk38;
    s16 unk3A;
    s32 unk3C;
    s32 unk40;
    s32 unk44;
    u8 unk48;
    u8 unk49;
    u8 unk4A;
    u8 unk4B;
    s32 unk4C;
    s32 unk50;
    s32 unk54;
    s32 unk58;
    s32 unk5C;
    s32 unk60[4];
    s32 unk70[4];
    s32 unk80[4];
    s32 unk90[4];
    s32 unkA0[4];
    s32 unkB0[4];
    s32 unkC0[4];
    u8 unkD0;
    u8 unkD1;
    u8 unkD2;
    u8 unkD3;
    PropModel50_B8 *next;
};

typedef struct {
    void *unk0;
    f32 unk4;
    u8 unk8;
    u8 unk9;
    u8 unkA;
    u8 unkB;
    s16 unkC;
    s16 unkE;
    s32 unk10;
    s32 unk14;
    s32 unk18;
    s32 unk1C;
    s16 unk20;
    s16 unk22;
    s16 unk24;
    s16 unk26;
    s16 unk28;
    s16 unk2A;
    s16 unk2C;
    u8 unk2E;
    u8 unk2F;
    u8 unk30;
    u8 pad31[0x34 - 0x31];
    s32 unk34;
} PropModel54_BC;
typedef struct {
    f32 unk0[4];
    f32 unk10[4];
    u8 unk20;
    u8 unk21;
    u8 unk22;
    u8 unk23;
    void *unk24[1];
    s32 unk28;
    s32 unk2C;
    s32 unk30;
    void *unk34[1];
    s32 unk38;
    s32 unk3C;
    s32 unk40;
    s32 unk44;
    s16 unk48;
    s16 unk4A;
    s16 unk4C;
    u8 unk4E;
    u8 unk4F;
    PropModel50_B8 *unk50;
    PropModel54_BC *unk54;
    s16 unk58;
    u8 unk5A;
    u8 unk5B;
    u8 unk5C;
    u8 pad5D[3];
    s32 unk60[4];
    s32 unk70[4];
    s32 unk80[4];
    s32 unk90[4];
    s32 unkA0[4];
    s32 unkB0;
    s16 unkB4;
    s16 unkB6;
    PropModel50_B8 *unkB8;
    PropModel54_BC *unkBC;
    s16 unkC0;
    u8 unkC2;
    u8 unkC3;
    u8 unkC4;
    u8 unkC5;
} PropModel;
typedef struct PropScriptBlock PropScriptBlock;
typedef struct PropScriptInstruction {
    s16 instruction;
    s16 args[3];
} PropScriptInstruction;
struct PropScriptBlock {
    u8 condition_count;
    u8 pad1;
    PropScriptInstruction conditions[5];
    u8 execution_count;
    u8 pad2B;
    PropScriptInstruction executions[4];
    PropScriptBlock *next;
};
typedef struct {
    void* unk0;
    u32 unk4;
    f32 unk8;
    f32 unkC;
    s16 unk10[2];
    f32 unk14[8];
    f32 unk34;
    s32 unk38;
    f32 unk3C;
    s16 unk40;
    s16 unk42;
    s16 unk44[2];
    u8 unk48[3];
    u8 next_state[3];
    s8 unk4E;
    s8 unk4F;
    u8 unk50;
    s8 unk51;
    u8 unk52;
    s8 unk53;
    s8 unk54;
    s8 unk55;
    s8 unk56;
    s8 unk57;
    s32 unk58;
    u8 unk5C;
    u8 unk5D;
    u16 unk5E;
    u8 unk60;
    u8 unk61;
    s16 unk62;
    s16 unk64;
    u8 unk66;
    u8 unk67;
    s16 unk68;
    s16 unk6A;
    s16 unk6C;
    u8 unk6E;
    s8 unk6F;
    union {
        u8 unk70;
        s8 unk70_s8;
    };
    u8 unk71;
    u8 unk72;
    u8 unk73;
    f32 unk74;
    f32 unk78;
    f32 unk7C;
    s16 unk80;
    s16 unk82;
    s16 unk84;
    s16 unk86;
    s16 unk88;
    u16 unk8A;
    f32 unk8C;
    f32 unk90;
    Actor *unk94;
    s16 unk98;
    u8 unk9A;
    s8 unk9B;
    s32 unk9C;
    PropScriptBlock *unkA0;
} Prop_ScriptData;
typedef struct {
    s32 unk0;
    s32 unk4;
    s32 unk8;
    s32 unkC;
    s32 unk10;
    s32 unk14;
    s32 unk18;
    u8 unk1C;
    u8 unk1D;
    s16 unk1E;
    s32 unk20;
    s32 unk24;
    s16 unk28;
    s16 unk2A;
    s16 unk2C;
    s16 unk2E;
    s16 unk30;
    u8 unk32;
    u8 unk33;
    u8 unk34;
    u8 unk35;
    u8 unk36;
    u8 unk37;
    u8 unk38;
    u8 unk39;
    s16 unk3A;
    s16 unk3C;
    s16 unk3E;
    s32 unk40;
    s32 unk44;
    s32 unk48;
    s32 unk4C;
    s32 unk50;
    s32 unk54;
    s32 unk58;
    s32 unk5C;
    s32 unk60;
    s32 unk64;
    s32 unk68;
} Prop_unk24;
typedef struct Prop_unk74 Prop_unk74;
struct Prop_unk74 {
    f32 unk0;
    f32 unk4;
    f32 unk8;
    Prop_unk74 *next;
};
typedef struct {
    s16 unk0;
    u8 unk2;
    u8 unk3;
    u8 unk4;
    union {
        s8 unk5;
        u8 unk5_u8;
    };
    u8 unk6;
    u8 unk7;
    s32 unk8;
    f32 unkC;
    s16 unk10;
    s16 unk12;
} Prop_unk78;
typedef struct {
    u8 unk0[0x10 - 0x0];
    s32 unk10;
    u8 unk14;
    u8 unk15;
    u8 unk16;
    u8 unk17;
    void *unk18;
} Prop_unk28;
typedef struct {
    void *unk0;
    s32 unk4;
    s32 unk8;
    u8 unkC[0x24 - 0xC];
    u8 unk24;
    u8 unk25[2];
} Prop_unk48;
typedef struct {
    Prop_unk28 *unk0;
    s32 unk4;
    s32 unk8;
    s32 unkC;
    s32 unk10;
    u8 unk14;
    u8 unk15;
    u8 pad16[2];
    s32 unk18;
    s32 unk1C;
} Prop_seg28;
typedef struct prop {
    f32 x_position;
    f32 y_position;
    f32 z_position;
    f32 hitbox_scale;
    f32 x_rotation;
    f32 y_rotation;
    f32 z_rotation;
    f32 unk1C;
    PropModel *model_pointer;
    Prop_unk24 *unk24;
    Prop_seg28 unk28;
    Prop_unk48 *unk48;
    s32 unk4C;
    s32 unk50;
    s32 unk54;
    s32 unk58;
    s32 unk5C;
    s32 unk60;
    s32 unk64;
    s32 unk68;
    u8 unk6C;
    u8 unk6D[1];
    u8 unk6E;
    u8 unk6F;
    GlobalASMStruct83 *unk70;
    Prop_unk74 *unk74;
    Prop_unk78 *unk78;
    Prop_ScriptData *unk7C;
    s32 unk80;
    s16 object_type;
    s16 unk86;
    s16 unk88;
    s16 unk8A;
    u8 unk8C;
    u8 unk8D;
    u8 unk8E;
    u8 unk8F;
} Prop;

typedef struct Struct80717D84 Struct80717D84;

typedef struct {
    Gfx *unk0[2];
} Struct80717D84_unk0;

typedef struct {
    s32 unk0;
    u8 unk4[0xA - 0x4];
    u8 unkA;
    u8 unkB;
    s16 unkC;
    s16 unkE;
    u8 pad10[0x14 - 0x10];
    u8 unk14;
    u8 unk15;
    s16 unk16;
} Struct80717D84_unk330;

struct Struct80717D84 {
    Struct80717D84_unk0 unk0[4];
    u8 unk20;
    u8 unk21;
    u8 unk22[0x26 - 0x22];
    u16 sprite_index; // @recomp: Custom index for mtx tagging
    Vtx unk28[4 * 4];
    float unk128[4][2][4][4];
    void *unk328;
    u8 unk32C;
    u8 unk32D[0x330 - 0x32D];
    Struct80717D84_unk330 *unk330;
    s8 unk334;
    s8 unk335;
    s8 unk336;
    s8 unk337;
    void *unk338;
    s16 unk33C;
    s8 unk33E;
    s8 unk33F;
    f32 unk340;
    f32 unk344;
    f32 unk348;
    s16 unk34C;
    s16 unk34E;
    u8 unk350;
    u8 unk351;
    u8 unk352;
    u8 unk353;
    s32 unk354;
    s32 unk358;
    s32 unk35C;
    f32 unk360;
    f32 unk364;
    s16 unk368;
    u8 unk36A;
    u8 unk36B;
    u8 unk36C;
    s8 unk36D;
    u8 unk36E;
    u8 unk36F;
    u32 unk370[4];
    s32 unk380;
    void *unk384;
    s16 unk388;
    s8 unk38A;
    s8 unk38B;
    u16 unk38C;
    s16 unk38E;
    s16 unk390;
    s16 unk392;
    s16 unk394;
    Struct80717D84 *unk398;
    Struct80717D84 *unk39C;
};


typedef struct ActorSpawner ActorSpawner;

struct ActorSpawner {
    s16 actor_type; // At 0x0
    u16 unk2;
    f32 x_position; // At 0x4
    f32 y_position; // At 0x8
    f32 z_position; // At 0xC
    f32 unk10;
    f32 unk14;
    f32 unk18;
    s16 y_rotation; // At 0x1C
    u16 unk1E;
    f32 unk20; // At 0x20, Used
    u8 pad24[0x32 - 0x24];
    s16 unk32;
    u8 pad34[0x44 - 0x34];
    Actor* tied_actor; // At 0x44
    u8 unk48; // Used
    u8 unk49;
    s16 unk4A;
    u8 unk4C; // Used
    u8 unk4D;
    Actor *unk50;
    f32 unk54;
    s16 unk58;
    s16 id; // At 0x5A
    s32 (*unk5C)(s32); // At 0x5C
    s32 (*drawing_code)(s32); // At 0x60
    ActorSpawner* previous_spawner; // At 0x64
    ActorSpawner* next_spawner; // At 0x68
    u8 pad6C[0x80 - 0x6C];
};

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

extern Struct8075EB80 D_global_asm_8075EB80[];

// TODO: Use that clever tuple thing from BKomp
typedef struct {
    s16 coords_0[3]; // 0x0
    s16 coords_1[3]; // 0x6
    s16 coords_2[3]; // 0xC
    s16 coords_3[3]; // 0x12
    s16 coords_4[3]; // 0x18
} EnemyAggressionBox;

typedef struct EnemyMovementBox EnemyMovementBox;

struct EnemyMovementBox {
    s16 x_pos_0; // 0x0
    s16 z_pos_0; // 0x2
    s16 x_pos_1; // 0x4
    s16 z_pos_1; // 0x6
    u8 pad[4];
    EnemyAggressionBox *aggression_box_pointer; // = 0xC, -- u32
    u8 pad10[0xC];
    Actor *unk1C;
};

typedef struct SpawnerFileData {
    u8 enemy_value; // 0x0
    u8 unk1;
	u16 y_rot; // 0x2
	s16 x_pos; // 0x4
	s16 y_pos; // 0x6
	s16 z_pos; // 0x8
	u8 cs_model; // 0xA
    u8 unkB;
	u8 max_idle_speed; // 0xC
	u8 max_aggro_speed; // 0xD
    u8 unkE;
	u8 scale; // 0xF
	u8 aggro; // 0x10
    u8 unk11;
	u8 something_spawn_state; // 0x12
	u8 spawn_trigger; // 0x13
	u8 respawn_timer_init; // 0x14 - Result is multiplied by 30 to get actual respawn timer
	u8 unk15;
} SpawnerFileData;

typedef struct SpawnerData_unk20 {
    u8 unk0;
    u8 unk1;
} SpawnerData_unk20;

typedef struct {
	SpawnerFileData init;
    u8 unk16;
    u8 unk17;
    Actor *tied_actor; // 0x18
	EnemyMovementBox *movement_box_pointer; // 0x1C
	SpawnerData_unk20 *unk20; // 0x20
	s16 respawn_time; // 0x24
    s16 unk26;
    s32 unk28;
	f32 unk2C; // 0x2C initially written to 0.01
	f32 unk30; // 0x30 initially written to 1
	f32 animation_speed; // 0x34
	u32 unk38; // 0x38 TODO: maybe float, based on alt enemy type
	u32 unk3C;
    s16 chunk; // 0x40
	u8 spawn_state; // 0x42
	u8 counter; // 0x43
	u8 alternative_enemy_spawn; // 0x44

	// 1000 0000 0000 0000 - ?
	// 0100 0000 0000 0000 - ? Resets on Respawn
	// 0010 0000 0000 0000 - ? Resets on Respawn
	// 0001 0000 0000 0000 - ?

	// 0000 1000 0000 0000 - ?
	// 0000 0100 0000 0000 - ?
	// 0000 0010 0000 0000 - ?
	// 0000 0001 0000 0000 - ?

	// 0000 0000 1000 0000 - ?
	// 0000 0000 0100 0000 - Ignores instrument plays
	// 0000 0000 0010 0000 - Ignores movement boundaries
	// 0000 0000 0001 0000 - ?

	// 0000 0000 0000 1000 - ? Reset on respawn
	// 0000 0000 0000 0100 - ?
	// 0000 0000 0000 0010 - Won't Respawn
	// 0000 0000 0000 0001 - Spawned from respawn pending

    // TODO: proper bitfield syntax
	u16 properties_bitfield; // = 0x46 bitfield -- TODO: Document this, find where this comes from so we can display stuff pre-load
} EnemySpawner; // 807FDC8C pointer to array of structs, count at 807FDC88

typedef struct {
    u8 pad0[0x2 - 0x0];
    s16 unk2;
    s16 unk4;
    s16 unk6;
    s16 unk8;
    union {
        u8 unkA_u8[2];
        u16 unkA_u16;
        s16 unkA_s16;
    };
    u8 unkC;
    u8 unkD;
    u8 unkE;
    u8 unkF;
    u8 unk10;
    u8 unk11;
    u8 unk12;
    u8 unk13;
} CharacterSpawner;
