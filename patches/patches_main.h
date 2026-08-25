#ifndef MAX
#define MAX(a,b) (((a)>(b))?(a):(b))
#endif
#ifndef MIN
#define MIN(a,b) (((a)<(b))?(a):(b))
#endif

#define	OS_TV_PAL		0
#define PI_STATUS_DMA_BUSY (1 << 0)
#define OS_READ 0
#define UNK_ADDR 0x800002E8
#define	OS_CLOCK_RATE		62500000LL
#define	OS_CPU_COUNTER		(OS_CLOCK_RATE*3/4)
#define OS_NSEC_TO_CYCLES(n)	(((u64)(n)*(OS_CPU_COUNTER/15625000LL))/(1000000000LL/15625000LL))
#define BUFFER_TIME OS_NSEC_TO_CYCLES(48484843)

extern void osWritebackDCache(void*, s32);
extern u32  osPiGetStatus(void);
extern void osInvalDCache(void*, s32);

typedef u32 OSIntMask;

typedef struct OverlayInfoStruct {
    s32 rom_code_start;
    s32 rom_data_end;
    void* rdram_start;
    void* overlay_end;
    void* rdram_code_end;
    void* rdram_data_end;
} OverlayInfoStruct;
extern OverlayInfoStruct gOverlayTable[];

typedef struct {
    /* 0x0 */ s32 unk0;   // ID or handle; freed via func_global_asm_80611408
    /* 0x4 */ u8  unk4;   // countdown timer; evict when decremented to 0
    /* 0x5 */ u8  pad5;
    /* 0x6 */ u16 pad6;
    /* 0x8 */             // sizeof == 0x8
} Unk807F0A58Entry;

typedef struct {
    s32 unk0;
    s32 unk4;
    s32 unk8;
} Struct80744464;

typedef struct UnkMQStruct {
    OSMesgQueue mq;
    OSMesg msgs[1];
} UnkMQStruct;

extern Struct80744464 D_global_asm_80744464;
typedef u64	OSTime;
extern OSThread D_global_asm_80761430;
extern OSTime osGetTime(void);
extern Unk807F0A58Entry D_global_asm_807F0A58[];
extern s32 D_global_asm_807F5A58;  // active entry count
extern s32 D_global_asm_8076A070;
extern s32 osTvType;
extern s8 D_global_asm_80744460;
extern u16 D_global_asm_8074682C;
extern u8 D_global_asm_80744504;
extern u8 D_global_asm_807F059C[];
extern u64 gStackCanary;
extern s32 D_global_asm_807655E0;
extern u8 D_global_asm_807444F0;
extern s32 D_global_asm_80767CC0;
extern Maps current_map;
extern s32 current_exit;
extern Maps next_map;
extern s32 next_exit;
extern u8 is_cutscene_active;
extern f32 D_global_asm_807444BC;
extern f32 D_global_asm_807FD888;
extern u32 global_properties_bitfield;
extern u8 D_global_asm_807444F8;
extern OSMesgQueue D_global_asm_8076A110;
extern u8  D_global_asm_8076A0B1;
extern u8  D_global_asm_8076A0B2;
extern u16* D_global_asm_80744470[2];
extern Gfx* D_global_asm_8076A050[];
extern u8 D_global_asm_807444FC;

typedef struct {
    OSMesgQueue queue;
    u8 unk18[0x260 - 0x18];
    s32 unk260;
    u8 unk264[0x280 - 0x264];
    s32 unk280;
    s32 frame_count;
} Struct80767A40;

extern Struct80767A40 D_global_asm_80767A40; // TODO: Might be an array, see func_global_asm_805FBC5C
extern s32 D_global_asm_8076AF10;
extern u8 D_global_asm_8076AF14;
extern s16 D_global_asm_8076AF00[];
extern u8 D_global_asm_80745290;
extern u32 D_global_asm_80744478;
extern u32 object_timer;
extern s32 D_global_asm_807FBB64; // shadow_display_bitfield, shockwave_bitfield, long_distance_actor_spawn


extern OSViMode osViModeTable[];
extern s16 D_global_asm_80744494;
extern s16 D_global_asm_80744498;
extern s16 D_global_asm_8074449C;
extern s16 D_global_asm_807444A0;
extern s16 D_global_asm_807444A4;
extern s16 D_global_asm_807444A8;
extern s16 D_global_asm_807444AC;
extern s16 D_global_asm_807444B0;
extern s16 D_global_asm_807444B4;
extern u8 D_global_asm_8074450C;
extern u8 D_global_asm_80744510;
extern s32 D_global_asm_80744588[];
extern s8 D_global_asm_807445A0;
extern s8 D_global_asm_807445A4;
extern s16 D_global_asm_80744490;
extern s16 D_global_asm_8076A0AA;
extern u8 D_global_asm_807FD890;

typedef struct Unk {
    char unk_00[4];
    s32 unk_04;
    s32 unk_08;
} Unk;

typedef struct Struct131B0_1 Struct131B0_1;

struct Struct131B0_1 {
    Struct131B0_1* next;
    s32 unk4;
    s32 unk8;
    s32 unkC;
    s32 unk10;
    char unk_14[0x3C];
    Unk* unk_50;
};

typedef struct {
    s32 unk0;
    s32 unk4;
    s32 unk8;
    s32 unkC;
    s32 unk10;
} Struct131B0_2_unk268;

// P Sure this is OSScTask_s
// typedef struct {
//     s32 unk0;
//     s32 unk4;
//     s32 unk8;
//     s32 unkC;
//     OSTask unk10;
// /* 0x50 */ char unk_50[4];
// /* 0x54 */ OSMesgQueue* queue;
// /* 0x58 */ OSMesg mesg;
// } Struct131B0_2_unk274;

typedef struct {
    /* 0x000 */ u8 pad0[0x18];
    /* 0x018 */ void* func;
    /* 0x01C */ u8 pad1C[0x3C];
    /* 0x058 */ OSMesgQueue* mesgQueue;
    /* 0x05C */ char pad5C[4];
    /* 0x060 */ s32 unk_60;
    /* 0x064 */ char pad64[0xC];                    /* maybe part of unk_60[4]? */
    /* 0x070 */ void* unk70;                        /* inferred */
    /* 0x074 */ char pad74[0x3C];                   /* maybe part of unk70[0x10]? */
    /* 0x0B0 */ OSThread unkB0;                     /* inferred */
    /* 0x260 */ Struct131B0_1* unk260;
    /* 0x264 */ Struct131B0_1* unk264;
    /* 0x268 */ Struct131B0_2_unk268* unk268;
    /* 0x26C */ Struct131B0_1* unk26C;
    /* 0x270 */ Struct131B0_1* unk270;
    /* 0x274 */ OSScTask* unk274;
    /* 0x278 */ void* unk_278; //unknown what this points to
    /* 0x27C */ char pad27C[4];
    /* 0x280 */ s32 unk_280;
    /* 0x284 */ s32 unk_284;
    /* 0x288 */ OSTime unk_288;
    /* 0x290 */ OSTime unk290;
} Struct131B0_2;                                    /* size = 0x298 */

extern s32 D_global_asm_8076A084;
extern UnkMQStruct D_global_asm_807655F0;
extern UnkMQStruct D_global_asm_807656D0;

extern UnkMQStruct D_global_asm_807659E8;

extern OSMesg D_global_asm_8076A108;
extern OSTimer D_global_asm_8076A130;

void func_global_asm_805FB750(s32 arg0, s32 arg1, void* arg2);
void func_dk64_boot_800024E0(u8** arg0, void* arg1, void* arg2);
void func_global_asm_806003EC(s16);
s32 func_global_asm_8060042C(s16 arg0);
void func_global_asm_8060EC80(OSMesgQueue* arg0, void* arg1, s32 arg2, u8 arg3, u8 arg4);
extern s32 D_global_asm_807655E0;
extern s32 D_global_asm_80767CD8;
void func_global_asm_8060FFF0(void);
void func_global_asm_8060A900(void);
void func_global_asm_80600D50(void);
void func_global_asm_8060ED6C(Struct131B0_2* arg0, Struct131B0_1* arg1, s32 arg2, s32 arg3, s32 arg4);
extern s32 D_global_asm_8076A070;
extern s32 D_global_asm_8076A07C;
void setIntroStoryPlaying(u8 arg0);
void func_global_asm_8073239C(void);
void func_global_asm_805FE544(u8 arg0);
void func_global_asm_805FB7E4(void);
void func_global_asm_80610350(u8, u8, s32);
s16 playSound(s16 arg0, s32 arg1, f32 arg2, f32 arg3, u8 arg4, u8 arg5);
void func_global_asm_80601CF0(s32 arg0);
void func_global_asm_8060E930(void);
void func_global_asm_80611724(u32, u32);
void func_global_asm_80611408(void *ptr);
void boot_osPiRawStartDma(s32, s32, void*, s32);
void func_global_asm_805FBC5C(void);
void osViSetSpecialFeatures(u32);
void func_global_asm_805FBE04(void);
void func_global_asm_805FE7FC(void);
void func_global_asm_8070A934(s32 nextMap, s32 nextExit);
void yield_self(void);
void func_dk64_boot_80000980(void);
void load_dk64_overlay(s32, void*, s32);
void osInitialize(void);
void func_arcade_80024000(void);
void func_jetpac_80024000(void);
void func_global_asm_805FC2B0(void);
void func_global_asm_80600B10(void);
void func_global_asm_8066AF40(void);
void func_global_asm_80610268(u32 arg0);
void func_global_asm_80600674(void); //calculate lag boost?
void func_global_asm_80658CCC(void);
void func_global_asm_80700BF4(void);
void raiseException(u8 arg0, s32 arg1, s32 arg2, s32 arg3);
void cFuncLoop(void);
void func_global_asm_80610044(void* arg0, s32 arg1, u8 arg2, u8 arg3, s32 arg4, u8 arg5);

//f32 at unk8 doesn't match with what's in structs.h (s16) 
//for PlayerAdditionalActorData so making this struct

typedef struct {
    s16     button;
    s8      stick_x;        /* -80 <= stick_x <= 80 */
    s8      stick_y;        /* -80 <= stick_y <= 80 */
} DKTV_OSContPad;

extern s32 D_global_asm_80755338; // cutscene_will_play_next_map
extern s32 D_global_asm_8075533C; // cutscene_to_play_next_map
extern Vtx D_global_asm_80754C48[2][4];
extern u16 *D_global_asm_807ECE94;
extern DKTV_OSContPad *D_global_asm_807ECE98;
extern u8 is_autowalking;
extern u16 D_global_asm_8075531C; // Demo Fadeout Timer
extern Gfx *func_global_asm_805FD030(Gfx *dl);

extern Gfx **D_1000118;
extern Mtx D_2000000;
extern Mtx D_2000080;
extern Mtx D_20000C0;
extern Mtx D_2000180;
extern Mtx D_2000200;

extern u8 D_global_asm_807FDB0F; // alpha
extern s32 D_global_asm_807FDB10;
extern void *D_global_asm_807FDB14; // TODO: Function pointer
extern u8 D_global_asm_807FDB18;
extern s16 D_global_asm_807FDB1A;
extern u8 D_global_asm_807FDB1C;
extern u8 D_global_asm_807FDB1D;
extern u8 D_global_asm_807FDB28;
extern s32 D_global_asm_807FDB2C;
extern s32 D_global_asm_807FDB30;
extern u16 D_global_asm_807FDB36;
extern s16 D_global_asm_807FDB38;
extern s16 D_global_asm_807FDB3A;
extern s16 D_global_asm_807FDB3C;
extern s16 D_global_asm_807FDB3E;
extern s16 D_global_asm_807FDB40;
extern s16 D_global_asm_807FDB42;
extern s16 D_global_asm_807F7358;
extern s16 D_global_asm_807F735A;
extern s16 D_global_asm_807F735C;
extern s16 D_global_asm_807F735E;
extern s16 D_global_asm_807FDB3C;
extern s16 D_global_asm_807FDB3E;
extern s16 D_global_asm_807FDB40;
extern s16 D_global_asm_807FDB42;

extern u8 cc_number_of_players;
extern Gfx *func_global_asm_805FCFD8(Gfx *dl);
extern void func_global_asm_8061134C(void *);
extern u8 isFlagSet(s16 flagIndex, u8 flagType);

extern void *getPointerTableFile(enum pointertable_e pointerTableIndex, s32 fileIndex, u8 arg2, u8 arg3);
extern void func_global_asm_8066B434(void *arg0, s32 arg1, s32 arg2);
extern f32 D_global_asm_80750228;
extern f32 D_global_asm_8075022C;

extern void func_global_asm_80658E58(s16 arg0, s16 arg1, s16 arg2, s16 arg3);

typedef struct Struct8062DBDC {
    f64 unk0[8];
} Struct8062DBDC;
 
f32 func_global_asm_8062E040(s16, s16, s16, s16, s16, s16, f32, f32, f32);
s32 func_global_asm_8062E1F8(s32, s16, s16, s16, s16, s16, s16, void *);
extern f32 D_global_asm_807F5E50[];

extern u8 D_805FB000;
extern u8 D_805FB0D0;
extern u32 D_A4500004;
extern u8 D_global_asm_80741310;
extern u32 D_global_asm_807452C8;
extern u8 D_global_asm_80760590;
extern s32 D_global_asm_80770558;

void func_global_asm_80602314(void);

typedef struct {
    void *unk0;
    s16 unk4;
    u8 pad6[2];
    s32 unk8;
    u8 padC[4];
    s32 unk10;
    u8 pad14[4];
    s32 unk18;
    s32 unk1C;
    void *unk20;
    s32 unk24;
    void *unk28;
    u8 pad2C[4];
    void *unk30;
    s32 unk34;
    u8 pad38[0x10];
    Acmd *unk48;
    s32 unk4C;
    s32 unk50;
    s32 unk54;
    void *unk58;
    void *unk5C;
    void *unk60;
    u8 pad64[4];
    s16 unk68;
    u8 pad6A[2];
    void *unk6C;
} Struct8076D708;
typedef struct {
    Struct8076D708* unk0[1];
} Struct8076D4D0;
extern void func_global_asm_80602314(void);
extern s32 D_global_asm_8076D4C0;
extern OSMesgQueue D_global_asm_8076D6D0;
extern s32 D_global_asm_80770194;
extern s32 D_global_asm_8077019C;
extern Struct8076D4D0 D_global_asm_8076D4D0;
extern Acmd *n_alAudioFrame(Acmd *cmdList, s32 *cmdLen, s16 *outBuf, s32 outLen);
extern s32 func_global_asm_8060EE58(s32 arg0);