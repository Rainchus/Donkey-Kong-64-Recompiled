#include "common_structs.h"
#include "debug_config.h"

extern Gfx* handle_interpolation(Gfx * dl, interpolationIDs id, u8 decrement);

typedef struct Unk {
    char unk_00[4];
    s32 unk_04;
    s32 unk_08;
} Unk;

typedef struct Struct131B0_1 Struct131B0_1;

struct Struct131B0_1 {
    Struct131B0_1 *next;
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
} Struct131B0_2;

extern OSScTask* D_global_asm_807F04E0;
extern s32 D_global_asm_807F04E4;
extern s32 D_global_asm_80746820;
extern s32 D_global_asm_80746824;
extern s32 func_global_asm_8060F854(Struct131B0_2 *, OSScTask *);
extern u8 is_cutscene_active;
extern u8 D_global_asm_8076A0B1;
extern unsigned long long __udivdi3_recomp(unsigned long long num, unsigned long long denom);
extern u8 D_global_asm_80746830;
extern Maps current_map;
extern s32 object_timer;
extern s16 D_global_asm_807476F4;
extern u16 D_global_asm_807F5CF4;
extern u16 D_global_asm_8075531C; // Rap Timer
extern OSTime D_global_asm_807F5CE0;
extern u16 D_global_asm_807476F0;

#define TIMING_DEBUG 0
#define FAST_LOADS 1
#define INTRO_STORY_LAG 3
#define RAP_LAG 3
#define RAP_TIME_TO_TIMER(a) (0x1644 - (s32)((a) * 30))

int getFrameDelta(void) {
    // Jetpac & Arcade
    if ((is_cutscene_active == 3) || (is_cutscene_active == 4)) {
        return 1;
    }
    #if FAST_LOADS
        if ((
            !D_global_asm_807F5CE0 // Intro Story not started
        ) || (
            // Intro Story Maps - For ISG Purposes
            (current_map != MAP_HELM_INTRO_STORY) &&
            (current_map != MAP_ROCK_INTRO_STORY) &&
            (current_map != MAP_DK_HOUSE) &&
            (current_map != MAP_DK_ISLES_DK_THEATRE)
        )) {
            if (D_global_asm_80746830) {
                return 0;
            }
        }
    #endif
    // DK Rap
    if ((D_global_asm_807F5CF4 & 4) == 0) {
        switch (current_map) {
            case MAP_DK_RAP:
                if ((D_global_asm_8075531C < RAP_TIME_TO_TIMER(113 - 17)) && (D_global_asm_8075531C > RAP_TIME_TO_TIMER(114 - 17))) {
                    if ((object_timer % 8) == 0) {
                        return RAP_LAG;
                    }
                }
                if ((D_global_asm_8075531C < RAP_TIME_TO_TIMER(115 - 17)) && (D_global_asm_8075531C > RAP_TIME_TO_TIMER(117 - 17))) {
                    if ((object_timer % 8) == 0) {
                        return RAP_LAG;
                    }
                }
                if ((D_global_asm_8075531C < RAP_TIME_TO_TIMER(138 - 17)) && (D_global_asm_8075531C > RAP_TIME_TO_TIMER(144 - 17))) {
                    if ((object_timer % 8) == 0) {
                        return RAP_LAG;
                    }
                }
                if ((D_global_asm_8075531C < RAP_TIME_TO_TIMER(150 - 17)) && (D_global_asm_8075531C > RAP_TIME_TO_TIMER(153 - 17))) {
                    if ((object_timer % 8) == 0) {
                        return RAP_LAG;
                    }
                }
                break;
            // Intro Story
            case MAP_ROCK_INTRO_STORY:
            // cs0 is too slow
                if (D_global_asm_807476F4 == 0) {
                    // K Rool Approaches
                    if ((object_timer % 8) == 0) {
                        return INTRO_STORY_LAG;
                    }
                } else if (D_global_asm_807476F4 == 1) {
                    // Crash into rock
                    if (D_global_asm_807476F0 < 150) {
                        if ((object_timer % 5) < 2) {
                            return INTRO_STORY_LAG;
                        }
                    }
                }
                break;
            case MAP_HELM_INTRO_STORY:
                if (D_global_asm_807476F4 == 0) {
                    // Under 36%
                    if ((object_timer % 25) < 8) {
                        return INTRO_STORY_LAG;
                    }
                } else if (D_global_asm_807476F4 == 4) {
                    if ((object_timer % 3) < 2) {
                        return INTRO_STORY_LAG;
                    }
                } else if (D_global_asm_807476F4 == 8) {
                    if ((object_timer % 10) < 7) {
                        return INTRO_STORY_LAG;
                    }
                } else if ((D_global_asm_807476F4 > 8) && (D_global_asm_807476F4 <= 0xE)) {
                    if ((object_timer % 8) == 0) {
                        return INTRO_STORY_LAG;
                    }
                }
                break;
            case MAP_DK_ISLES_DK_THEATRE:
                if (D_global_asm_807476F4 == 7) {
                    if ((object_timer % 6) == 0) {
                        return INTRO_STORY_LAG;
                    }
                }
                break;
            case MAP_DK_HOUSE:
                if (D_global_asm_807476F4 == 0) {
                    if ((object_timer % 7) == 0) {
                        return INTRO_STORY_LAG;
                    }
                }
                break;
            default:
                break;
        }
    }
    // Normal Gameplay
    return 2;
}

#if TIMING_DEBUG
s32 LastPrintedSecond = -1;
s32 frameCounter = 0;
#endif

RECOMP_PATCH void func_global_asm_8060F730(Struct131B0_2* arg0) {
    OSScTask* temp_s0;
    
    temp_s0 = arg0->unk_278;
    arg0->unk_278 = NULL;
    temp_s0->flags |= 8;

    if ((getFrameDelta() == 0) || (D_global_asm_80746820 < arg0->unk_284)) {
        D_global_asm_807F04E0 = NULL;
        if (D_global_asm_8076A0B1 & 2) {
            D_global_asm_8076A0B1 ^= 2;
            osViBlack(0);
        }
        osViSwapBuffer(temp_s0->framebuffer);
        D_global_asm_80746824 = D_global_asm_80746820;
        D_global_asm_80746820 = arg0->unk_284 + getFrameDelta();
        #if TIMING_DEBUG
            frameCounter++;
            if ((s32)(D_global_asm_80746820 / 60) != LastPrintedSecond) {
                LastPrintedSecond = D_global_asm_80746820 / 60;
                recomp_printf("Delay %d:\n", getFrameDelta());
                recomp_printf("Frames %d:\n", frameCounter);
                frameCounter = 0;
            }
        #endif
        osDpSetStatus(8);
    } else {
        D_global_asm_807F04E0 = temp_s0;
    }
    D_global_asm_807F04E4 = __udivdi3_recomp((osGetTime() - arg0->unk_288), 0x1E91);
    func_global_asm_8060F854(arg0, temp_s0);
}

extern OSTime D_global_asm_807F04F8;
extern s8 D_global_asm_80744510;
extern OSViMode	osViModeTable[];
extern s32 D_global_asm_8074684C[];
extern s32 	osTvType;	
extern void *D_global_asm_80744470[];
extern OSScTask* D_global_asm_807F04E0;
extern void func_global_asm_8060FAA4(OSMesgQueue *arg0, OSMesg arg1, s32 arg2);
extern OSTimer D_global_asm_807F0540;
extern void func_global_asm_8060F928(Struct131B0_2 *arg0, Struct131B0_1 *arg1); 

static s16 D_global_asm_80746858_copy = 0;

RECOMP_PATCH void func_global_asm_8060F254(Struct131B0_2* arg0) {
    Struct131B0_1* sp54;
    s32 temp_a0;
    Struct131B0_1* temp_v0_3;
    s32 sp48;
    s32 i;
    OSMesgQueue *audioQueue;

    arg0->unk_284 += 1;
    arg0->unk_280 += 1;
    D_global_asm_807F04F8 = osGetTime();

    if (D_global_asm_80744510 != 0) {
        if (D_global_asm_80744510 == 1) {
            if (D_global_asm_80746858_copy == 0) {
                osViBlack(1);
            }
            if (D_global_asm_80746858_copy == 0x3C) {
                osViSetMode(&osViModeTable[D_global_asm_8074684C[osTvType]]);
                osViSetSpecialFeatures(0x42U);
                osViBlack(0);
                D_global_asm_80744510 = 2;
            }
            D_global_asm_80746858_copy++;
        }
        osViSwapBuffer(D_global_asm_80744470[0]);
    }

    if (D_global_asm_807F04E0 != NULL) {
        if ((getFrameDelta() == 0) || (D_global_asm_80746820 <= arg0->unk_284)) { // Slow down input rate for this
            if (D_global_asm_8076A0B1 & 2) {
                D_global_asm_8076A0B1 ^= 2;
                osViBlack(0);
            }
            osViSwapBuffer(D_global_asm_807F04E0->framebuffer);
            D_global_asm_80746824 = D_global_asm_80746820;
            D_global_asm_80746820 = arg0->unk_284 + getFrameDelta();
            #if TIMING_DEBUG
                frameCounter++;
                if ((s32)(D_global_asm_80746820 / 60) != LastPrintedSecond) {
                    LastPrintedSecond = D_global_asm_80746820 / 60;
                    recomp_printf("Delay(2) %d:\n", getFrameDelta());
                    recomp_printf("Frames(2) %d:\n", frameCounter);
                    frameCounter = 0;
                }
            #endif
            osDpSetStatus(8);
            temp_a0 = (s32)D_global_asm_807F04E0->msgQ;
            if (temp_a0 != 0) {
                func_global_asm_8060FAA4((OSMesgQueue* ) temp_a0, (void* ) D_global_asm_807F04E0->msg, 0);
            }
            D_global_asm_807F04E0 = NULL;
        }
    } else {
        if ((osViGetCurrentFramebuffer() == osViGetNextFramebuffer()) && (osDpGetStatus() & DPC_STATUS_FREEZE)) {
            arg0->unk_288 = osGetTime();
            osDpSetStatus(DPC_STATUS_FLUSH);
        }
    }
    for (i = 0, sp48 = arg0->unk_60; i < sp48; i++) {
        //the i == -1 is definitely fake, but we have no way to know what was actually written here
        if (osRecvMesg((OSMesgQueue *)&arg0->mesgQueue, (OSMesg *)&sp54, OS_MESG_NOBLOCK) == -1 || i == -1) {
            
        }
        if (((u32) arg0->unk_284 % (u32) sp54->unk_50->unk_08) == 0) {
            func_global_asm_8060F928(arg0, (Struct131B0_1* ) sp54);
        } else {
            osSendMesg((OSMesgQueue *)&arg0->mesgQueue, sp54, OS_MESG_NOBLOCK);
        }        
    }

    if ((arg0->unk264 != 0) && !(arg0->unk_284 & 1)) {
        // osSetTimer(&D_global_asm_807F0540, 280000, 0, (OSMesgQueue *)arg0->unk264->unk_50->unk_04, (void* )5);
        /*
            @recomp: To those reading this, this was an *annoying* side-quest to go on to fix this
            Turns out that this (the above commented out code) is a race condition with many layers, just like ogres.
            The audio engine would die (either via crash or just not making a sound) if too much load was placed on the PC.
            Reducing the timer would fix this issue on lower-power systems, but would cause double-audio
            issues on those which ran fine.
            Having a guard of validCount == 0 blocks the double-audio issue whilst solving the race condition.
        */
        audioQueue = (OSMesgQueue *)arg0->unk264->unk_50->unk_04;
        if (audioQueue->validCount == 0) {
            osSendMesg(audioQueue, (void*)5, OS_MESG_NOBLOCK);
        }
    }
    
    for (temp_v0_3 = arg0->unk260; temp_v0_3 != NULL; temp_v0_3 = temp_v0_3->next) {
        if (temp_v0_3->unkC == 3) {
            osSendMesg((OSMesgQueue* ) temp_v0_3->unk4, (void* )0x29A, 0);
        }  
    }
}

extern u8  D_global_asm_807444FC;
extern s16 D_global_asm_80744490;
extern Gfx *func_global_asm_8068C20C(Gfx *, u8);

RECOMP_PATCH Gfx *func_global_asm_805FE4D4(Gfx *dl) {
    gEXEnable(dl++);
    // Frame delta
    int delta = getFrameDelta();
    if (delta == 0) {
        delta = 1;
    }
    gEXSetRefreshRate(dl++, 60 / delta);
    // Interpolation
    dl = handle_interpolation(dl, MTXTAG_CAMERAPROJECTION, TRUE);
    // 
    gEXSetNearClipping(dl++, FALSE);
    gEXSetTexcoordWrapPoint(dl++, 256 * 4, 256 * 4);
    gDPSetColorImage(dl++, 0, 2, D_global_asm_80744490, osVirtualToPhysical(D_global_asm_80744470[D_global_asm_807444FC]));
    return dl;
}