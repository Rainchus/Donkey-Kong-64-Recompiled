#include "common_structs.h"

RECOMP_DECLARE_EVENT(recomp_on_music_bin_load(s32 song, s32 bank, u8 *bin));

extern s16 D_global_asm_807F614A;
extern Prop *D_global_asm_807F6000;
Prop prop_assignment[1000];

// @recomp: Prop array
RECOMP_PATCH s16 func_global_asm_80631FAC(Maps map, u8 arg1) {
    // @recomp: Not needed
    // switch (map) {
    //     case MAP_FUNGI:
    //         D_global_asm_807F614A = 530;
    //         break;
    //     case MAP_FACTORY:
    //         D_global_asm_807F614A = 500;
    //         break;
    //     case MAP_AZTEC:
    //         D_global_asm_807F614A = 500;
    //         break;
    //     case MAP_GALLEON:
    //         D_global_asm_807F614A = 485;
    //         break;
    //     case MAP_JAPES:
    //         D_global_asm_807F614A = 465;
    //         break;
    //     default:
    //         D_global_asm_807F614A = 450;
    //         break;
    // }
    D_global_asm_807F614A = 1000;
    if (arg1) {
        // @recomp: Change to static assignment
        D_global_asm_807F6000 = &prop_assignment[0];
        bzero(&prop_assignment[0], 1000 * sizeof(Prop));
        // func_global_asm_80611690(D_global_asm_807F6000);
    }
    return D_global_asm_807F614A;
}

typedef struct SynthConfig {
    s32 unk0;
    s32 unk4;
    s32 unk8;
    s32 unkC;
    s32 unk10;
    ALHeap *unk14;
    s32 unk18;
    u8 unk1C;
    u8 unk1D;
    u8 pad1E[2];
    s32 unk20;
    s32 unk24;
} SynthConfig;

typedef struct {
    u8 unk0[0xEC];
} struct_sub_8076C328;

typedef struct {
    struct_sub_8076C328 unk0[4];
} struct_8076C328;

extern struct_8076C328 D_global_asm_8076C328[];

typedef struct Struct80600D50_sp60 {
    s32 unk0;
    s32 unk4;
    s32 unk8;
} Struct80600D50_sp60;

typedef struct OverlayInfoStruct {
    s32 rom_code_start;
    s32 rom_data_end;
    void *rdram_start;
    void *overlay_end;
    void *rdram_code_end;
    void *rdram_data_end;
} OverlayInfoStruct;

typedef struct SeqpConfig {
    s32 unk0;
    s32 unk4;
    u8 unk8;
    u8 unk9;
    u8 padA[2];
    ALHeap *unkC;
    void *unk10;
    void *unk14;
    void *unk18;
    s32 unk1C;
} SeqpConfig;

typedef struct UnkConfig {
    s32 unk0;
    s32 unk4;
    s32 unk8;
    ALHeap *unkC;
    s16 unk10;
} UnkConfig;

void func_global_asm_80737E50(ALCSPlayer *);
void func_global_asm_80737F0C(ALSeqPlayer*, s32, u8);
void func_global_asm_80738080(ALSeqPlayer*, u8, u8);
void func_global_asm_807380CC(ALSeqPlayer*, s32, u8);
void func_global_asm_80738118(ALSeqPlayer*, s32, u8);
void func_global_asm_80601A10(SynthConfig *, s32, Struct80600D50_sp60 *);
void func_global_asm_80732DEC(SeqpConfig *, s32);
void func_global_asm_80732F10(ALSeqPlayer *, SeqpConfig *);
void func_global_asm_80735AA0(UnkConfig *);
void _free(void *ptr);
void func_global_asm_8060B140(u32 arg0, void *arg1, s32 *arg2, u8 arg3, u8 arg4, u8 arg5, u8 *arg6);
void func_global_asm_80735A50(ALSeqPlayer *arg0, ALBank *arg1);
void func_global_asm_806010A0(void);
void func_global_asm_80737C20(s32 arg0);
void func_global_asm_80737CF4(s32 arg0, s32 arg1);
void func_global_asm_80601CC0(void);
extern u16 D_global_asm_8076BF30[];
extern void *D_global_asm_8076BF38[];
extern ALHeap D_global_asm_8076D1E8;
extern ALBank *D_global_asm_8076D1FC;
extern u8 D_global_asm_80770F50;
extern s32 D_global_asm_807452B0[];
extern OverlayInfoStruct gOverlayTable[];
extern ALBank *D_global_asm_8076D1F8;
extern ALSeqPlayer *D_global_asm_8076BF20[];

u8 audio_engine[0x38000];

// @recomp: Audio Engine
RECOMP_PATCH void func_global_asm_80600D50(void) {
    UnkConfig spCC;
    ALBankFile *temp_v0_2;
    ALBankFile *temp_v0_3;
    s32 spC0;
    SynthConfig sp98;
    SeqpConfig sp78;
    Struct80600D50_sp60 sp6C;
    ALSeqPlayer *player;
    void *temp_v0;
    s32 i;

    alHeapInit(&D_global_asm_8076D1E8, &D_global_asm_80770F50, 0x7BD80);
    sp98.unk0 = 0x2C;
    sp98.unk4 = 0x1E;
    sp98.unk8 = 0x40;
    sp98.unkC = 1;
    sp98.unk10 = 0;
    sp98.unk1C = 6;
    sp98.unk1D = 6;
    sp98.unk18 = 0;
    sp98.unk14 = &D_global_asm_8076D1E8;
    sp6C.unk0 = 22050;
    sp6C.unk4 = 2;
    sp6C.unk8 = 0xC00;
    func_global_asm_80601A10(&sp98, 0x14, &sp6C);
    spC0 = gOverlayTable[15].rom_data_end - gOverlayTable[15].rom_code_start;
    temp_v0 = &audio_engine[0]; // @recomp: Change this to be a static address
    func_global_asm_8060B140(gOverlayTable[15].rom_code_start, temp_v0, &spC0, 0xD, 0, 2, 0);
    temp_v0_2 = alHeapDBAlloc(NULL, 0, &D_global_asm_8076D1E8, 1, spC0);
    func_global_asm_8060B140(gOverlayTable[15].rom_code_start, temp_v0_2, &spC0, 0xD, 0, 2, 0);
    alBnkfNew(temp_v0_2, (u8*)gOverlayTable[14].rom_code_start);
    D_global_asm_8076D1FC = temp_v0_2->bankArray[0];
    spC0 = gOverlayTable[16].rom_data_end - gOverlayTable[16].rom_code_start;
    func_global_asm_8060B140(gOverlayTable[16].rom_code_start, temp_v0, &spC0, 0xD, 0, 2, 0);
    _free(temp_v0);
    temp_v0_3 = alHeapDBAlloc(NULL, 0, &D_global_asm_8076D1E8, 1, spC0);
    func_global_asm_8060B140(gOverlayTable[16].rom_code_start, temp_v0_3, &spC0, 0xD, 0, 2, 0);
    alBnkfNew(temp_v0_3, (u8*)gOverlayTable[17].rom_code_start);
    D_global_asm_8076D1F8 = temp_v0_3->bankArray[0];
    sp78.unk0 = 0x2C;
    sp78.unk4 = 0x40;
    sp78.unk9 = 0;
    sp78.unk8 = 0x10;
    sp78.unkC = &D_global_asm_8076D1E8;
    func_global_asm_80732DEC(&sp78, 0x58);
    for (i = 0; i < 4; i++) {
        D_global_asm_8076BF30[i] = 0xFFFF;
        D_global_asm_8076BF38[i] = alHeapDBAlloc(NULL, 0, &D_global_asm_8076D1E8, 1, D_global_asm_807452B0[i]);
        D_global_asm_8076BF20[i] = alHeapDBAlloc(NULL, 0, &D_global_asm_8076D1E8, 1, 0x8C);
        player = D_global_asm_8076BF20[i];
        if ((!D_global_asm_8076BF38[i]) || (player = D_global_asm_8076BF20[i], !player)) {
            player = D_global_asm_8076BF20[i];
        }
        func_global_asm_80732F10(player, &sp78);
        func_global_asm_80735A50(D_global_asm_8076BF20[i], temp_v0_2->bankArray[0]);
    }
    func_global_asm_806010A0();
    spCC.unk4 = 0x40;
    spCC.unk0 = 0x40;
    spCC.unk8 = 0x14;
    spCC.unk10 = 8;
    spCC.unkC = &D_global_asm_8076D1E8;
    func_global_asm_80735AA0(&spCC);
    func_global_asm_80737C20(4);
    func_global_asm_80737CF4(0, 4);
    func_global_asm_80601CC0();
}

typedef struct HeapHeader HeapHeader;
struct HeapHeader {
    HeapHeader *prev;
    s32 size;
    u8 unk8;
    u8 unk9;
    u8 unkA;
    u8 unkB;
    s32 unkC;
};

extern HeapHeader *D_global_asm_807F5A64;

void *func_global_asm_806111F8(s32 arg0, u32 arg1);

typedef struct HeapStruct {
    void *prev_obj;
    u32 size;
    struct HeapStruct *prev;
    struct HeapStruct *next;
} HeapStruct;

typedef struct HeapArenaMeta {
    void *index;
    HeapStruct *start;
    HeapStruct *tail;
    s32 bin_size;
    s16 chunk_size;
    u8 pad12[2];
} HeapArenaMeta;

#define GET_OBJ_HEADER(a) ((&((HeapStruct *)(a))[-1]))

void func_global_asm_806109EC(void);
void func_global_asm_80610A88(HeapStruct *src, s32 *dest, u32 count);
void func_global_asm_80610BD8(void *, void*, s32);
s32 func_global_asm_80610C74(s32*, void *, s32, s32);
void func_global_asm_80610B84(HeapStruct *arg0, s32 *arg1, u32 arg2);
void func_global_asm_8061159C(HeapHeader *arg0);
void *_malloc(s32);
extern u16 *D_global_asm_80744470[];
extern u32 D_global_asm_80744474;
extern u8 D_global_asm_8074450C;
extern s32 D_global_asm_8076A060;
extern void *D_global_asm_807F05A4;
extern s32 D_global_asm_807F0974;
extern s32 D_global_asm_807F0978;
extern s32 D_global_asm_807F097C;
extern s32 D_global_asm_807F0980;
extern s32 D_global_asm_807F0984;
extern HeapArenaMeta D_global_asm_807F0988[5];
extern s32 D_global_asm_807F0A28;
extern s16 D_global_asm_807F0A30;
extern s16 D_global_asm_807F0A40;
extern s32 D_global_asm_807F0A50;
extern s32 D_global_asm_807F5A58;
extern s32 D_global_asm_807F5A5C;
extern s32 D_global_asm_807F5A60;
extern s32 D_global_asm_807F5A68;
extern s32 ** D_global_asm_807F5A70[];
extern OverlayInfoStruct gOverlayTable[];

extern u8 framebuffer0[];
extern u8 framebuffer1[];
extern u8 depthBuffer[];

typedef struct Struct80610350sp60 {
    u8 pad0[4];
    s32 unk4;
    u8 pad8[0x10-0x8];
    u8 unk10[1];  // Unsure of size
} Struct80610350sp60;

u8 heap_slot[0x40000];

RECOMP_PATCH void func_global_asm_80610350(u8 arg0, u8 arg1, s32 arg2) {
    s32 spFC;
    s32 spF4[2];
    s32 spF0;
    s32 spEC;
    void *spE8;
    void *spE4;
    s32 spE0;
    s32 pad1[5];
    void *addr[5];
    OverlayInfoStruct *overlay;
    HeapStruct *hdr;
    s32 padAC[1];
    s32 size[5];
    s32 var_v0_2;
    s32 *current_object;
    HeapStruct *sp8C;
    HeapStruct *sp88;
    s32 sp84;
    void *sp80;
    s32 temp_lo;
    s32 var_t1;
    s32 var_t2;
    HeapStruct *last_header; // sp70
    s32 fb_end;
    s32 var_v1;
    s32 i;
    s32 temp_s2;
    s32 var_s3;
    u32 *var_v0;
    u32 var_a1_2;
    u32 temp_v1_6;

    temp_lo = D_global_asm_8074450C * 0x25800 * D_global_asm_8074450C;
    D_global_asm_807F0A28 = 0;
    D_global_asm_807F0A50 = 0;
    D_global_asm_807F0A30 = 0;
    D_global_asm_807F0A40 = 0;
    D_global_asm_807F5A60 = 0;
    D_global_asm_807F5A58 = 0;
    D_global_asm_807F5A5C = 2;
    spF0 = 0;
    spEC = 0;
    spE8 = 0;
    spE4 = 0;
    spE0 = 0;
    var_t1 = -1;
    switch (arg1) { /* switch 1 */
        default:
            var_t2 = temp_lo;
            break;
        case 0: /* switch 1 */
            var_t2 = 0;
            break;
        case 9: /* switch 1 */
            var_t1 = 8;
            var_t2 = 0;
            break;
        case 10: /* switch 1 */
            var_t1 = 9;
            var_t2 = 0;
            break;
        case 7: /* switch 1 */
            var_t1 = 7;
            var_t2 = temp_lo;
            break;
        case 6: /* switch 1 */
            var_t1 = 6;
            var_t2 = temp_lo;
            break;
        case 5: /* switch 1 */
            var_t1 = 5;
            var_t2 = temp_lo;
            break;
        case 8: /* switch 1 */
            var_t1 = 4;
            var_t2 = temp_lo;
            break;
        case 4: /* switch 1 */
            var_t1 = 3;
            var_t2 = temp_lo;
            break;
        case 3: /* switch 1 */
            var_t1 = 2;
            var_t2 = temp_lo;
            break;
        case 2: /* switch 1 */
            var_t1 = 0xA;
            var_t2 = temp_lo;
            break;
    }
    if (var_t1 != -1) {
        overlay = &gOverlayTable[var_t1];
        spE4 = overlay->rdram_code_end;
        spE8 = overlay->rdram_start;
        spEC = overlay->rom_code_start;
        spE0 = (s32) overlay->rdram_data_end - (s32) overlay->rdram_code_end;
        spF0 = (s32)spE4 - (s32)spE8;
    }

    fb_end = (spF0 + spE0 + 0x8002403F);
    D_global_asm_80744470[0] = (u16*)(fb_end & ~0x3F); // 8002403F start of framebuffers, aligned to 64
    fb_end = temp_lo + (fb_end & ~0x3F);
    D_global_asm_80744474 = fb_end;
    var_v1 = 0x805FAE00 - var_t2;   // 0x805FAE00 is end of depth buffer
    D_global_asm_8076A060 = var_v1; // depth buffer pointer
    for (i = 4; i >= 0; i--) {
        switch (i) { /* switch 2 */
            case 0:  /* switch 2 */
                temp_lo = fb_end + temp_lo;
                spFC = var_v1 - (temp_lo);
                break;
            case 1: /* switch 2 */
                spFC = 0x3810;
                break;
            case 2: /* switch 2 */
                spFC = 0xC010;
                break;
            case 3: /* switch 2 */
                spFC = 0x2810;
                break;
            case 4: /* switch 2 */
                spFC = 0x61620;
                break;
        }
        var_v1 -= spFC;
        addr[i] = (void*)var_v1;
        size[i] = spFC;
    }
    if (arg0 != 0 && (D_global_asm_807F5A68 != 0)) {
        func_global_asm_806109EC();
        for (i = 1; i < D_global_asm_807F5A68; i++) {
            current_object = *D_global_asm_807F5A70[i - 1];
            hdr = GET_OBJ_HEADER(current_object);
            temp_s2 = hdr->size + (s32) current_object;
            current_object = *D_global_asm_807F5A70[i];
            *D_global_asm_807F5A70[i] = (s32 *) (temp_s2 + sizeof(HeapStruct));
            hdr = GET_OBJ_HEADER(current_object);
            var_s3 = hdr->size + sizeof(HeapStruct);
            func_global_asm_80610A88(hdr, (s32*)temp_s2, var_s3);
            func_global_asm_80610B84(hdr, (s32*)temp_s2, var_s3);
        }
        last_header = GET_OBJ_HEADER(*D_global_asm_807F5A70[0]);
        temp_s2 = (s32)addr[0];
        var_s3 = 0;
        for (i = 0; i < D_global_asm_807F5A68; i++) {
            current_object = *D_global_asm_807F5A70[i];
            hdr = GET_OBJ_HEADER(current_object);
            var_s3 += hdr->size + sizeof(HeapStruct);
            *D_global_asm_807F5A70[i] = (s32 *) (((s32) current_object + temp_s2) - (s32) last_header);
        }
        func_global_asm_80610A88(last_header, (s32*)temp_s2, var_s3);
        func_global_asm_80610B84(last_header, (s32*)temp_s2, var_s3);
        hdr = GET_OBJ_HEADER(*D_global_asm_807F5A70[0]);
        sp84 = hdr->size;
        sp80 = hdr->prev_obj;
        sp8C = hdr->next;
        sp88 = hdr->prev;
    } else {
        D_global_asm_807F5A68 = 0;
        D_global_asm_807F5A70[0] = NULL;
    }
    func_global_asm_80610BD8(0, addr[0], size[0]);
    D_global_asm_807F0974 = func_global_asm_80610C74(&D_global_asm_807F0974, addr[1], size[1], 0x10);
    D_global_asm_807F0978 = func_global_asm_80610C74(&D_global_asm_807F0978, addr[2], size[2], 0x20);
    D_global_asm_807F097C = func_global_asm_80610C74(&D_global_asm_807F097C, addr[3], size[3], 0x30);
    D_global_asm_807F0980 = func_global_asm_80610C74(&D_global_asm_807F0980, addr[4], size[4], 0x1000);
    last_header = NULL;
    if (D_global_asm_807F5A68 != 0) {
        for (i = 0; i < D_global_asm_807F5A68; i++) {
            hdr = GET_OBJ_HEADER(*D_global_asm_807F5A70[i]);
            hdr->prev_obj = last_header;
            last_header = hdr;
        }
        D_global_asm_807F0988[0].start->prev_obj = sp80;
        D_global_asm_807F0988[0].start->size = sp84;
        D_global_asm_807F0988[0].start->next = sp8C;
        D_global_asm_807F0988[0].start->prev_obj = 0;
        D_global_asm_807F0988[0].start->prev = sp88;
        D_global_asm_807F0988[0].tail = (HeapStruct *) ((s32) last_header + last_header->size + sizeof(HeapStruct));
        D_global_asm_807F0988[0].tail->prev_obj = last_header;

        // Problems here, cba to fix IDO optimizations for memory offset calcs
        var_v0_2 = D_global_asm_807F0988[0].bin_size;
        var_v0_2 -= 0x20;
        var_v0_2 = ((var_v0_2) - (s32) D_global_asm_807F0988[0].tail) + (s32) D_global_asm_807F0988[0].start;
        D_global_asm_807F0988[0].tail->size = var_v0_2;
        D_global_asm_807F0988[0].tail->prev = 0;
        D_global_asm_807F0988[0].tail->next = 0;
        hdr = GET_OBJ_HEADER(D_global_asm_807F0988[0].bin_size + (s32) D_global_asm_807F0988[0].start);
        hdr->prev_obj = D_global_asm_807F0988[0].tail;
    }
    D_global_asm_807F5A64 = _malloc(0x10);
    func_global_asm_8061159C(D_global_asm_807F5A64);
    if (arg2 == 0) {
        D_global_asm_807F0984 = 0x28000;
    } else {
        D_global_asm_807F0984 = arg2;
    }
    D_global_asm_807F05A4 = &heap_slot[0]; // @recomp: Replace with a static address
    func_global_asm_8061159C(D_global_asm_807F05A4);
    if (spF0 != 0) {
        if (spE4 != 0) {
            bzero(spE4, spE0);
        }
        func_global_asm_8060B140(spEC, spE8, &spF0, 1, 1, 1, 0);
        osWritebackDCache(spE8, spF0);
        osInvalICache(spE8, spF0);
    }
    for (var_a1_2 = 0; var_a1_2 < 2; var_a1_2++) {
        var_v0 = (u32*)D_global_asm_80744470[var_a1_2];
        temp_v1_6 = (((s32) (D_global_asm_8074450C * 0x12C00 * D_global_asm_8074450C) / 2) * 4) + (s32) var_v0;
        for (; (u32) var_v0 < temp_v1_6; var_v0++) {
            *var_v0 = 0x00010001;
        }
    }
}
void func_global_asm_80737F40(ALSeqPlayer*);
s32 func_global_asm_80737E30(ALSeqPlayer *seqp);
void n_alCSeqNew(ALCSeq *seq, u8 *ptr);
void func_global_asm_8060A398(s32);
extern void *D_global_asm_8076BF38[];
extern s32 D_global_asm_8076D200[];
extern u8 D_global_asm_80770560[];
extern f32 D_global_asm_80770568[];
extern u8 D_global_asm_80770598[];
extern ALCSeq D_global_asm_8076BF48[];
u8 music_decompression_buffer[0x8000];

// @recomp: Music Loading
RECOMP_PATCH void func_global_asm_8060A1B0(s32 arg0, u8 arg1, f32 arg2) {
    ALSeqPlayer** temp_s2;
    u32 i;
    u8* temp_s0;
    s32 temp_a1;
    
    temp_s2 = &D_global_asm_8076BF20[arg0];
    i = 0;
    alSeqpStop(*temp_s2);
    while (func_global_asm_80737E30(*temp_s2) && i < 0x1E8480) i++;
    if (i >= 0x1E8480U) {
        alSeqpStop(*temp_s2);
        while (func_global_asm_80737E30(*temp_s2) && i < 0x3D0900U) i++;
    }
    D_global_asm_80770598[arg0] = 0;
    D_global_asm_80770568[arg0] = arg2;
    D_global_asm_80770560[arg0] = arg1;
    temp_s0 = &music_decompression_buffer[0];
    temp_a1 = D_global_asm_8076D200[arg1 + 1] - D_global_asm_8076D200[arg1]; // @recomp Music size
    if (temp_a1 & 1) {
        temp_a1++;
    }
    func_global_asm_8060B140(
        D_global_asm_8076D200[arg1] + gOverlayTable[11].rom_code_start,
        D_global_asm_8076BF38[arg0], &temp_a1, 0x80U, 0U, 1U, temp_s0);
    // free(temp_s0);
    recomp_on_music_bin_load(arg1, arg0, D_global_asm_8076BF38[arg0]);
    n_alCSeqNew(&D_global_asm_8076BF48[arg0], D_global_asm_8076BF38[arg0]);
    alSeqpSetSeq(*temp_s2, (ALSeq *)&D_global_asm_8076BF48[arg0]);
    func_global_asm_80737F40(*temp_s2);
    func_global_asm_8060A398(arg0);
    func_global_asm_80737E50((ALCSPlayer* ) *temp_s2);
}
extern s8 D_global_asm_807F5D84;
extern s8 D_global_asm_807F5D85;
extern u16 *D_global_asm_807F5D80;
extern u8  D_global_asm_807444FC;
extern s16 D_global_asm_807F5D86;
extern s16 D_global_asm_807F5D88;
extern f32 D_global_asm_807F5D8C;
extern f32 D_global_asm_807F5D94;
extern u8 D_global_asm_80747B20;
u16 *func_global_asm_806FFF5C(void);
void func_global_asm_8062A348(void);
void func_global_asm_8070A848(void *arg0, void *arg1);

// @recomp: Screenshot for transition rendering
RECOMP_PATCH void func_global_asm_806291B4(u8 arg0) {
    if ((D_global_asm_807F5D84 == 0) || (arg0 == 7)) {
        if (D_global_asm_807F5D84 == 0) {
            // D_global_asm_807F5D80 = malloc(D_global_asm_80744490 * D_global_asm_80744494 * 2);
        }
        func_global_asm_8070A848(D_global_asm_807F5D80, D_global_asm_80744470[D_global_asm_807444FC]); // @recomp: Call this to initiate the rt64 snapshot
        D_global_asm_807F5D84 = 1;
        D_global_asm_80747B20 = 5;
        switch (arg0) {
            case 1:
                D_global_asm_807F5D86 = 0xFF;
                break;
            case 2:
                D_global_asm_807F5D86 = 0;
                break;
            case 0:
                D_global_asm_807F5D86 = 0x136;
                break;
            case 3:
                D_global_asm_807F5D86 = 0xA0;
                D_global_asm_807F5D88 = 0xA0;
                break;
            case 4:
            case 5:
                D_global_asm_807F5D8C = -5.0f;
                break;
            case 6:
                D_global_asm_807F5D94 = 0.0f;
                break;
            case 7:
                func_global_asm_8062A348();
                break;
        }
        D_global_asm_807F5D85 = arg0;
    }
}

extern Actor *gCurrentActorPointer;
extern s16 D_global_asm_80744490;
u16 *func_global_asm_806FFEAC(u16 *arg0, u16 *arg1);
u16 fairy_photo[0x5000];
// @recomp: Take fairy photo
RECOMP_PATCH u16 *func_global_asm_806FFF88(void) {
    u16 *sp34;
    s16 var_s3;
    u16 *temp_v0;
    u16 *var_s1;
    s32 var_s2;
    s16 var_s0;

    var_s3 = 7;
    if (gCurrentActorPointer->unk58 == ACTOR_UNKNOWN_217) {
        return func_global_asm_806FFF5C();
    }
    temp_v0 = &fairy_photo[0];
    sp34 = temp_v0;
    var_s1 = temp_v0;
    var_s2 = (s32)((s32)((s32)D_global_asm_80744470[D_global_asm_807444FC] + (var_s3 * (16 * D_global_asm_80744490))) + 0xA0);
    var_s3 = 0;
    do {
        var_s0 = 0;
        do {
            var_s1 = func_global_asm_806FFEAC(var_s1, (u16*)(((var_s0 << 1) << 5) + var_s2));
            var_s0 += 1;
        } while (var_s0 < 5);
        var_s2 += (D_global_asm_80744490 << 3) << 4;
        var_s3 += 1;
    } while (var_s3 < 2);

    return sp34;
}

extern Gfx *func_global_asm_806FF628(Gfx *, Actor *);
void func_global_asm_80613C48(Actor *arg0, s16 arg1, f32 arg2, f32 arg3);
void func_global_asm_80614D00(Actor *arg0, f32 arg1, f32 arg2);
void func_global_asm_80604CBC(Actor* arg0, s16 arg1, u8 arg2, u8 arg3, u8 arg4, u8 arg5, f32 arg6, s8 arg7);
void func_global_asm_80605314(Actor *arg0, u8 arg1);
void func_global_asm_807002AC(u16 *arg0, s16 **arg1, f32 arg2);
void func_global_asm_80688320(Actor *actor, s32 arg1, s16 arg2, void *arg3);
void changeCollectableCount(s32 HUDItemIndex, u8 playerIndex, s16 amount);
void playSong(MUSIC_E arg0, f32 arg1);
s16 playSound(s16 arg0, s32 arg1, f32 arg2, f32 arg3, u8 arg4, u8 arg5);
void addActorToTextOverlayRenderArray(void *arg0, Actor *arg1, u8 arg2);
void func_global_asm_80699070(s16 *arg0, s16 *arg1, f32 x1, f32 y1, f32 z1, f32 x2, f32 y2, f32 z2);
u8 setAction(s16 actionIndex, Actor *actor, u8 playerIndex);
s32 deleteActor(Actor*);
extern PlayerAdditionalActorData *extra_player_info_pointer;

typedef struct {
    PlayerAdditionalActorData *unk0;
    u16 *unk4;
    s16 *unk8[2];
    u16 unk10;
} AAD_80699284;

typedef struct {
    u8 unk0[0x128 - 0x0];
    s16 unk128;
} Struct80699128_arg1;

void func_global_asm_80699128(Gfx *dl, Struct80699128_arg1 *arg1);
void renderActor(Actor*, u8);

// @recomp: Fairy picture code
RECOMP_PATCH void func_global_asm_80699284(void) {
    AAD_80699284 *aaD;
    s16 spEA;
    s16 spE8;
    f32 spA8[4][4];
    f32 sp68[4][4];
    s16 sp66;
    f32 var_f0;
    u8 sp5F;
    f32 temp_f0_2;

    aaD = gCurrentActorPointer->additional_actor_data;
    sp5F = 0;
    if (!(gCurrentActorPointer->object_properties_bitfield & 0x10)) {
        aaD->unk10 = 0x5A;
        gCurrentActorPointer->object_properties_bitfield |= 0x400;
        gCurrentActorPointer->draw_distance = 0x3E8;
        aaD->unk4 = func_global_asm_806FFF88();
        func_global_asm_80613C48(gCurrentActorPointer, 0x40B, 0.0f, 0.0f);
        func_global_asm_80614D00(gCurrentActorPointer, 0.15f, 0.0f);
    } else;
    gCurrentActorPointer->object_properties_bitfield &= ~4;
    sp66 = 0x5A - aaD->unk10;
    if (sp66 == 0xD) {
        func_global_asm_80604CBC(gCurrentActorPointer, 0x222, 0, 0, 0, 0xFF, 1.0f, -1);
    }
    if (sp66 == 0x1B) {
        func_global_asm_80605314(gCurrentActorPointer, 0);
    }
    var_f0 = MIN(1.0, ((MAX(0, sp66 - 0x1E) / 60.0f) * 5.0));
    if ((var_f0 < 1.0) && (gCurrentActorPointer->unk58 != ACTOR_UNKNOWN_217)) {
        func_global_asm_807002AC(aaD->unk4, &aaD->unk8[D_global_asm_807444FC], var_f0);
        func_global_asm_80688320(gCurrentActorPointer, 0, 0, aaD->unk8[D_global_asm_807444FC]);
    } else {
        func_global_asm_80688320(gCurrentActorPointer, 0, 0, aaD->unk4);
        if (gCurrentActorPointer->control_state_progress == 0) {
            if (extra_player_info_pointer->unk1EC == 1) {
                changeCollectableCount(0xC, extra_player_info_pointer->unk1A4, 1);
                playSong(0x2E, 1.0f);
            } else if (extra_player_info_pointer->unk1EC == 2) {
                playSound(0x98, 0x7FFF, 63.0f, 1.0f, 0, 0x80);
            }
            gCurrentActorPointer->control_state_progress++;
        }
        if ((extra_player_info_pointer->unk1EC != 0xFF) && (aaD->unk10)) {
            addActorToTextOverlayRenderArray(func_global_asm_806FF628, gCurrentActorPointer, 3);
        }
    }
    func_global_asm_80699070(&spE8, &spEA, character_change_array->look_at_eye_x, character_change_array->look_at_eye_y, character_change_array->look_at_eye_z, character_change_array->look_at_at_x, character_change_array->look_at_at_y, character_change_array->look_at_at_z);
    temp_f0_2 = MIN(30.0, sp66) / 30.0;
    gCurrentActorPointer->y_rotation = spEA;
    gCurrentActorPointer->z_rotation = spE8 - (2048.0f + (-2048.0f * temp_f0_2));
    guRotateF(spA8, (spE8 / 4095.0) * 360.0, 1.0f, 0.0f, 0.0f);
    guRotateF(sp68, (spEA / 4095.0) * 360.0, 0.0f, 1.0f, 0.0f);
    guMtxCatF(spA8, sp68, spA8);
    guTranslateF(sp68, character_change_array->look_at_eye_x, character_change_array->look_at_eye_y, character_change_array->look_at_eye_z);
    guMtxCatF(spA8, sp68, spA8);
    guMtxXFMF(spA8, 0.0f, 0.0f, -30.0f * temp_f0_2, &gCurrentActorPointer->x_position, &gCurrentActorPointer->y_position, &gCurrentActorPointer->z_position);
    if (aaD->unk10 != 0) {
        aaD->unk10--;
    } else {
        gCurrentActorPointer->object_properties_bitfield &= ~0x8000;
        gCurrentActorPointer->shadow_opacity -= 0xA;
        if (gCurrentActorPointer->shadow_opacity <= 0) {
            sp5F = 1;
        }
    }
    if ((sp5F) || !(aaD->unk0->unk1F0 & 0x8000)) {
        if (aaD->unk0->unk1EC == 1) {
            setAction(0x58, NULL, 0);
        }
        func_global_asm_80605314(gCurrentActorPointer, 0);
        aaD->unk0->unk1F0 &= ~0x8000;
        aaD->unk0->unk1EC = 0xFF;
        // @recomp: Don't attempt to free this
        // if (gCurrentActorPointer->unk58 == ACTOR_UNKNOWN_217) {
        //     func_global_asm_8066B434(aaD->unk4, 0xF6, 0x45);
        // } else {
        //     func_global_asm_8061134C(aaD->unk4);
        // }
        aaD->unk0->vehicle_actor_pointer = NULL;
        deleteActor(gCurrentActorPointer);
        return;
    }
    guTranslateF(gCurrentActorPointer->unkC, 0.0f, -70.0f, 0.0f);
    renderActor(gCurrentActorPointer, 1);
    addActorToTextOverlayRenderArray(func_global_asm_80699128, gCurrentActorPointer, 3);
}

typedef struct {
    void *unk0; // Used
    s8 unk4; // Used
    s8 unk5;
    s8 unk6;
    s8 unk7;
} Struct807F0A58;
void func_global_asm_80611730(void);
extern Struct807F0A58 D_global_asm_807F0A58[];

RECOMP_PATCH void func_global_asm_8061138C(void *arg0) {
    while (D_global_asm_807F5A58 >= 0xA00) {
        recomp_printf("Exceeded defer budget: %d\n", D_global_asm_807F5A58);
        func_global_asm_80611730();
    }
    D_global_asm_807F0A58[D_global_asm_807F5A58].unk0 = arg0;
    D_global_asm_807F0A58[D_global_asm_807F5A58].unk4 = D_global_asm_807F5A5C;
    D_global_asm_807F5A58++;
}