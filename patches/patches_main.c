#include "common_structs.h"
#include "patches_main.h"
#include "ui.h"

#define gScissorUpLX D_global_asm_80744498
#define gScissorUpLY D_global_asm_8074449C
#define gScissorLowerRightX D_global_asm_807444A0
#define gScissorLowerRightY D_global_asm_807444A4

#define gScissor2LowerRightX D_global_asm_80744490
#define gScissor2LowerRightY D_global_asm_80744494

RECOMP_DECLARE_EVENT(recomp_adjust_dl_allocation(s32 *allocation));

RECOMP_PATCH void func_dk64_boot_8000102C(s32 offset, s32 size, void* dramAddr) {
    while (size & 0xf)
    {
        size++;
    }

    //@recomp: load overlays
    recomp_load_overlays(offset, dramAddr, size);

    osWritebackDCache(dramAddr, size);

    //@recomp: patch to call osPiRawStartDma variant
    //osPiRawStartDma(OS_READ, gOverlayTable[11].rom_code_start + offset, dramAddr, size);
    boot_osPiRawStartDma(OS_READ, gOverlayTable[11].rom_code_start + offset, dramAddr, size);

    do {} while (osPiGetStatus() & PI_STATUS_DMA_BUSY);
    osInvalDCache(dramAddr, size);
}

RECOMP_PATCH void func_dk64_boot_80000450(s32 devAddr, s32 arg1, void* dramAddr) {
    u32 size = arg1 - devAddr;

    //@recomp: load and map compressed address to uncompressed address
    load_dk64_overlay(devAddr, dramAddr, arg1 - devAddr);

    osInvalDCache(dramAddr, size);

    //@recomp: dma uncompressed data
    boot_osPiRawStartDma(OS_READ, devAddr, dramAddr, arg1 - devAddr);
    //osPiRawStartDma(OS_READ, devAddr, dramAddr, size);

    do {} while (osPiGetStatus() & PI_STATUS_DMA_BUSY);
}


RECOMP_PATCH void func_global_asm_80611730(void) {
    s32 corrupted = 0;
    s32 count;
    Unk807F0A58Entry* entry;

    func_global_asm_80611724(0x3791DFFF, 0x4BFFD668);
    //@recomp: patch to just remove this probable anti piracy check (it loads from uncached memory originally)
    //if (~0x3791DFFF != *(s32*)UNK_ADDR) {
    //    corrupted = 1;
    //}

    count = D_global_asm_807F5A58;
    if (count <= 0) {
        return;
    }

    entry = &D_global_asm_807F0A58[0];
    do {
        entry->unk4--;
        if (entry->unk4 == 0 && !corrupted) {
            func_global_asm_80611408(entry->unk0);
            count = --D_global_asm_807F5A58;
            *entry = D_global_asm_807F0A58[count];
        }
        else {
            entry++;
        }
    } while (entry < &D_global_asm_807F0A58[count]);
}

extern int gameIsInDKTVMode(void);
#define DK_DKTV_LAG_START 215
#define DK_DKTV_LAG_END 325
#define DK_DKTV_LAG_START2 420
#define D_global_asm_807463AC (*(volatile s16*)0x807463AC)

void updateLag(s32 value) {
    s32 limit = 1;
    if ((is_cutscene_active != 3) && (is_cutscene_active != 4)) {
        limit = 2;
    }
    if (value < limit) {
        value = limit;
    }
    D_global_asm_80744478 = value;
}

RECOMP_PATCH void func_global_asm_80600674(void) {
    s32 max_boost = 1;
    s32 min_boost = 20;
    s32 newBoost;
    s32 pad;
    s32 cap;
    s32 idx;
    s32 updateLagBoost;
    u32 oldBoost;
    s32 i;
    Struct80767A40* osdata;
    u8 is_dktv = FALSE;

    //@recomp: patch to always greater than 1 (on console, default is 2. if zero, it will divide by zero and crash)
    AlterVolumes();

    //@recomp: patch to always greater than 1 (on console, default is 2. if zero, it will divide by zero and crash)

    if (D_global_asm_80744478 <= 1) {
        D_global_asm_80744478 = 2;
    }

    if ((gameIsInDKTVMode()) && (current_map == MAP_JAPES)) {
        // @recomp: DK's DKTV
        is_dktv = TRUE;
        D_global_asm_80744478 = 3;
    } else if (D_global_asm_8076AF14) {
        osdata = &D_global_asm_80767A40;
        newBoost = osdata->frame_count - D_global_asm_8076AF10;
        newBoost = MAX(1, newBoost);
        D_global_asm_8076AF00[D_global_asm_80745290++] = newBoost;
        updateLagBoost = FALSE;
        if (D_global_asm_80745290 == 8) {
            D_global_asm_80745290 = 0;
        }
        oldBoost = D_global_asm_80744478;
        if (oldBoost >= 4) {
            cap = 1;
        }
        else if (oldBoost < newBoost) {
            cap = 2;
        }
        else {
            cap = 4;
        }
        idx = D_global_asm_80745290;
        for (i = 0; i < cap; i++) {
            idx--;
            if (idx < 0) {
                idx = 7;
            }
            max_boost = MAX(max_boost, D_global_asm_8076AF00[idx]);
            min_boost = MIN(min_boost, D_global_asm_8076AF00[idx]);
        }
        if ((oldBoost < newBoost) && (oldBoost < min_boost)) {
            updateLagBoost = TRUE;
        }
        else if ((newBoost < oldBoost) && (max_boost < oldBoost)) {
            updateLagBoost = TRUE;
        }
        if (updateLagBoost) {
            //@recomp: dont update; stays at 2
            updateLag(newBoost);
        }
        if (object_timer > 10) {
            while (D_global_asm_8076AF10 + D_global_asm_80744478 > osdata->frame_count) {
                //@recomp: yield so this progresses correctly
                yield_self();
            }
        }
        D_global_asm_8076AF10 = osdata->frame_count;
        return;
    }
    osdata = &D_global_asm_80767A40;

    //@recomp: dont update; stays at 2
    if (!is_dktv) {
        updateLag(osdata->frame_count - D_global_asm_8076AF10);
    }

    D_global_asm_8076AF10 = osdata->frame_count;
    //recomp_printf("D_global_asm_80744478 is %d:\n", D_global_asm_80744478);
}


RECOMP_PATCH void func_dk64_boot_800009D0(void) {
    u32* tmp_a0;
    osInitialize();

    //@recomp: patch to cached read
    tmp_a0 = (void*)0x802FE1C0; 

    while (0xAD170014 != *tmp_a0);
    *tmp_a0 = 0xF0F0F0F0;
    func_dk64_boot_80000980();
}

Gfx *func_global_asm_805FE634(Gfx *dl, u8 arg1);
void func_global_asm_805FE71C(Gfx *dl, u8 arg1, s32 *arg2, u8 arg3);
extern void *_malloc(s32);
extern s32 D_global_asm_807FBB64;
extern s32 D_global_asm_8076A058;
extern Gfx *D_global_asm_8076A050[];
extern s32 D_global_asm_8076A088;

// @recomp: DL Allocation handler
RECOMP_PATCH void func_global_asm_805FE544(u8 arg0) {
    s32 temp;
    s32 stored_temp;
    // @recomp: Quadruple the DL Allocation
    if (D_global_asm_807FBB64 & 1) {
        temp = 24000;
        stored_temp = temp;
        recomp_adjust_dl_allocation(&temp);
        if (stored_temp > temp) {
            // Some mod reduced the allocation
            temp = stored_temp;
        }
        D_global_asm_8076A058 = temp;
    } else {
        temp = 12000;
        stored_temp = temp;
        recomp_adjust_dl_allocation(&temp);
        if (stored_temp > temp) {
            // Some mod reduced the allocation
            temp = stored_temp;
        }
        D_global_asm_8076A058 = arg0 * temp;
    }
    D_global_asm_8076A050[0] = _malloc(D_global_asm_8076A058 * sizeof(Gfx));
    D_global_asm_8076A050[1] = _malloc(D_global_asm_8076A058 * sizeof(Gfx));
    func_global_asm_805FE71C(func_global_asm_805FE634(D_global_asm_8076A050[0], 0), 0, &D_global_asm_8076A088, 1);
    func_global_asm_805FE71C(func_global_asm_805FE634(D_global_asm_8076A050[1], 1), 1, &D_global_asm_8076A088, 1);
}

//RECOMP_PATCH void func_global_asm_805FB5C4(OSMesgQueue* arg0, s32 arg1) {
//    OSTime target_time;
//    Struct80744464 sp34;
//    OSTime buffer_time;
//    u8 buffer[1];
//    void* sp20;
//    u8 buffer2[5];
//    static OSTime D_global_asm_807655E8;
//
//
//    sp34 = D_global_asm_80744464;
//    if (arg1 == 2) {
//        osViBlack(1);
//        func_global_asm_80601CF0(1);
//        D_global_asm_80744460 = 1;
//        while (TRUE) {}
//    }
//    osRecvMesg(arg0, &sp20, 1);
//    D_global_asm_80744460 = 1;
//    func_global_asm_80601CF0(1);
//    osStopThread(&D_global_asm_80761430); //this was previously patched to a osDestoryThread; is that neccessary?
//    osSetThreadPri(NULL, 0xB);
//    D_global_asm_807655E8 = osGetTime();
//    while (osGetTime() < D_global_asm_807655E8 + BUFFER_TIME);
//    osViBlack(1);
//
//    //@recomp:patch don't call these because?
//    __osSpSetStatus(0xAAAA82);
//    osDpSetStatus(0x1D6);
//
//    func_global_asm_8060E930();
//    while (TRUE) {}
//}

#define SCREEN_HEIGHT 240 // Normally 240
#define SCREEN_WIDTH 427 // Normally 320
#define OVERSCAN_SIZE 0  // Normally 10

RECOMP_PATCH void func_global_asm_805FB944(u8 arg0) {
    u8 var_a1 = 1;
    s32 var_a2;

    var_a2 = 0;
    func_global_asm_806003EC(D_global_asm_8076A0AA);
    //@recomp Patch to always be 240p by setting D_global_asm_8074450C to 1
    if (current_map == MAP_NINTENDO_LOGO) {
        D_global_asm_8074450C = 1;
    }
    else {
        D_global_asm_8074450C = 1;
    }
    switch (is_cutscene_active) {
    case 3:
        var_a1 = 9;
    case 4:
        if (var_a1 == 1) {
            var_a1 = 0xA;
        }
        gScissorUpLX = 0;
        gScissorUpLY = 0;
        gScissorLowerRightX = (D_global_asm_8074450C * 320);
        gScissorLowerRightY = (D_global_asm_8074450C * 240);
        break;
    default:
        var_a2 = func_global_asm_8060042C(current_map);
        var_a1 = 1;
        if (D_global_asm_807FBB64 & 1) {
            var_a1 = 7;
        }
        else if (D_global_asm_807FBB64 & 0x1000) {
            var_a1 = 6;
        }
        else if (D_global_asm_807FBB64 & 0x104000) {
            var_a1 = 8;
        }
        else if (D_global_asm_807FBB64 & 0x80000) {
            var_a1 = 4;
        }
        else if (D_global_asm_807FBB64 & 0x2000) {
            var_a1 = 5;
        }
        else if (D_global_asm_807FBB64 & 0x04000000) {
            var_a1 = 3;
        }
        else if (D_global_asm_807FBB64 & 0x40000000) {
            var_a1 = 2;
        }
        gScissorUpLX = D_global_asm_8074450C * OVERSCAN_SIZE;
        gScissorUpLY = D_global_asm_8074450C * OVERSCAN_SIZE;
        gScissorLowerRightX = (D_global_asm_8074450C * (320 - OVERSCAN_SIZE));
        gScissorLowerRightY = (D_global_asm_8074450C * (240 - OVERSCAN_SIZE));
        break;
    }
    func_global_asm_80610350(arg0, var_a1, var_a2);
    if (D_global_asm_807445A4 == 0) {
        osViSetMode(&osViModeTable[D_global_asm_80744588[osTvType + osTvType + D_global_asm_8074450C - 1]]);
        if (D_global_asm_807445A0 == 0) {
            osViBlack(1U);
        }
        D_global_asm_80744510 = 0;
        D_global_asm_807445A0 = 0;
    }
    else {
        D_global_asm_80744510 = 1;
        D_global_asm_807445A0 = 1;
        D_global_asm_807445A4 = 0;
        func_global_asm_805FB7E4();
    }
    osViSetSpecialFeatures(VI_CTRL_TYPE_16 | VI_CTRL_SERRATE_ON);
    gScissor2LowerRightX = D_global_asm_8074450C * 320; //width
    gScissor2LowerRightY = D_global_asm_8074450C * 240; //height   
}

RECOMP_PATCH void func_global_asm_805FBC5C(void) {
    UnkMQStruct* mq;
    D_global_asm_8076A084 = gOverlayTable[12].rom_data_end - gOverlayTable[12].rom_code_start;
    osCreateMesgQueue(&D_global_asm_807655F0.mq, &D_global_asm_807655F0.msgs[0], 0x32);
    osCreateMesgQueue(&D_global_asm_807656D0.mq, &D_global_asm_807656D0.msgs[0], 0xC0);
    func_global_asm_8060EC80(
        &D_global_asm_80767A40.queue,
        &D_global_asm_80767A40,
        0x19,
        D_global_asm_80744588[osTvType + osTvType], 1);
    osCreateMesgQueue(&D_global_asm_807659E8.mq, &D_global_asm_807659E8.msgs[0], 0x10);
    func_global_asm_8060ED6C(
        (void*) &D_global_asm_80767A40,
        (void*)&D_global_asm_80767CD8,
        (s32) &D_global_asm_807659E8, 1, 1);
    current_map = next_map;
    func_global_asm_805FB944(0);
    D_global_asm_8076A07C = 5;
    func_global_asm_8060FFF0();
    func_global_asm_8060A900();
    func_global_asm_80600D50();
    setIntroStoryPlaying(0);
    func_global_asm_8073239C();
    mq = (void*)&D_global_asm_8076A110;
    osCreateMesgQueue((void*)mq, &D_global_asm_8076A108, 2);
    //@recomp: patch this timer to be significantly faster; we dont need to wait long
    osSetTimer(&D_global_asm_8076A130, OS_USEC_TO_CYCLES(100000), 0, (void*)mq, mq->msgs[0]); //wait 0.1 seconds
    //@recomp move playsound to a bit later
    //playSound(0x23C, 0x7FFF, 63.0f, 1.0f, 0, 0);
}

extern s32 D_global_asm_807F5E64;
extern f32 D_global_asm_807F5FA8;
extern f32 D_global_asm_807F5FAC;
extern f32 D_global_asm_807F5FB0;
extern f32 D_global_asm_807F5FB4;
extern u8 cc_player_index; // index into character_change_array, current_character_index[]
extern f32 D_global_asm_807F5FE0;
extern f32 D_global_asm_807F5FDC;
extern f32 D_global_asm_807F5E68;
extern f32 D_global_asm_807F5E20[][3];
extern s32 D_global_asm_807F5DE4; // TODO: Actually a pointer to a struct (map model?)
extern s32 D_global_asm_807F5E60;
extern u8 D_global_asm_807F5FEC;
extern s32 D_global_asm_807F5FF0;
extern u8 D_global_asm_80750AB4;
extern void* D_global_asm_807F5DE8;
extern void* D_global_asm_807F5DEC;

f32 func_global_asm_80612D10(f32 arg0);
void func_global_asm_8062DB70(f32 arg0, f32 arg1, f32 arg2, f32 arg3, f32 arg4, f32 arg5);
void func_global_asm_8062A944(f32 arg0, f32 arg1, f32 arg2);
void func_global_asm_8062AC68(void* arg0);
void func_global_asm_8062AD28(f32 arg0, f32 arg1, f32 arg2, void* arg3, f32* arg4);
void func_global_asm_8062D620(s32, s32, s32, f32, f32, f32, s32, s32, s32); //first and second arg here was s32, now void*
Gfx* func_global_asm_80722294(Gfx*, Actor*, s16);
void func_global_asm_8062C99C(CharacterChange250*, s32, s32, s32, s32);
Gfx* func_global_asm_8065919C(Gfx* dl);
Gfx* func_global_asm_8070835C(Gfx*, u8);
extern Mtx D_2000180;
Gfx* func_global_asm_8062CA70(Gfx* dl, s32 arg1, s32 arg2, f32 arg3, f32 arg4, f32 arg5, s32 arg6);

extern s32 func_global_asm_80626F8C(f32 arg0, f32 arg1, f32 arg2, f32 *arg3, f32 *arg4, s32 arg5, f32 arg6, s32 arg7);
//@recomp: Seems to be used for the culling of many objects, including actors and props
RECOMP_PATCH s32 func_global_asm_80658E8C(f32 arg0, f32 arg1, f32 arg2, u8 arg3, u8 arg4) {
    f32 sp44;
    f32 sp40;
    s16 sp3E;
    s16 sp3C;
    f32 d;
    f32 temp_f2_2;
    s16 var_a0;
    s16 var_a1;
    s16 var_a2;
    s16 var_v1;

    func_global_asm_80626F8C(arg0, arg1, arg2, &sp44, &sp40, 0, 1.0f, cc_player_index);
    sp3E = sp44;
    sp3C = sp40;
    d = _sqrtf(
        SQ(arg0 - character_change_array[cc_player_index].unk21C) +
        SQ(arg1 - character_change_array[cc_player_index].unk220) +
        SQ(arg2 - character_change_array[cc_player_index].unk224)
    );
    if (d < 90.0f) {
        return 0;
    }
    if (d >= 180.0f) {
        var_v1 = 0x14;
        var_a0 = 0x14;
        var_a1 = 0;
        var_a2 = 0x32;
    } else {
        temp_f2_2 = (d - 60.0f) / 120.0f;
        if (temp_f2_2 > 0.0) {
            var_v1 = (-60.0f * temp_f2_2) + 80.0f;
            var_a0 = (-60.0f * temp_f2_2) + 80.0f;
            var_a1 = (-130.0f * temp_f2_2) + 130.0f;
            var_a2 = (-80.0f * temp_f2_2) + 130.0f;
        } else {
            var_v1 = 0x50;
            var_a0 = 0x50;
            var_a1 = 0x82;
            var_a2 = 0x82;
        }
    }
    // @recomp: Remove x bound checks
    if ((sp3C < ((D_global_asm_807F735A - var_a1) - arg4)) || ((((D_global_asm_807F735E + var_a2 + arg4) < sp3C)))) {
        return 1;
    } else {
        return 0;
    }
}



//@recomp: Sprite culling. Also used for the scissor of the sprite itself
RECOMP_PATCH void func_global_asm_80714A68(s16 arg0, s16 arg1, s16 arg2, s16 arg3) {
    D_global_asm_807FDB3C = OVERSCAN_SIZE;
    D_global_asm_807FDB3E = OVERSCAN_SIZE;
    D_global_asm_807FDB40 = (320 - OVERSCAN_SIZE);
    D_global_asm_807FDB42 = (240 - OVERSCAN_SIZE);
}

//@recomp: Used for a bunch of display list initialization
RECOMP_PATCH Gfx *func_global_asm_807132DC(Gfx *dl) {
    dl = func_global_asm_805FD030(dl);
    gSPDisplayList(dl++, &D_1000118);
    gSPMatrix(dl++, &D_20000C0, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_PROJECTION);
    gSPMatrix(dl++, &D_2000180, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gDPPipeSync(dl++);
    gDPSetCombineMode(dl++, G_CC_MODULATEI_PRIM, G_CC_MODULATEI_PRIM);
    gDPSetPrimColor(dl++, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF);
    gDPSetScissor(dl++, G_SC_NON_INTERLACE, OVERSCAN_SIZE, OVERSCAN_SIZE, 320 - OVERSCAN_SIZE, 240 - OVERSCAN_SIZE);
    return dl;
}

//@recomp: Sprite reset function. Adjust scissor variables to not be tied to the original screen dimensions
RECOMP_PATCH void func_global_asm_80714A9C(void) {
    D_global_asm_807FDB0F = 0;
    D_global_asm_807FDB10 = 1;
    D_global_asm_807FDB14 = 0;
    D_global_asm_807FDB18 = 0;
    D_global_asm_807FDB1C = 1;
    D_global_asm_807FDB1A = 0;
    D_global_asm_807FDB1D = 0;
    D_global_asm_807FDB28 = 0;
    D_global_asm_807FDB2C = 0;
    D_global_asm_807FDB30 = 0;
    D_global_asm_807FDB36 = 0;
    D_global_asm_807FDB38 = -1;
    D_global_asm_807FDB3C = D_global_asm_8074450C * OVERSCAN_SIZE;
    D_global_asm_807FDB3E = D_global_asm_8074450C * OVERSCAN_SIZE;
    D_global_asm_807FDB40 = D_global_asm_8074450C * ((320 - OVERSCAN_SIZE));
    D_global_asm_807FDB42 = D_global_asm_8074450C * ((240 - OVERSCAN_SIZE));
    D_global_asm_807FDB3A = 0x258;
}

Gfx* func_global_asm_8062CEA8(Gfx*, void*, u8);      /* extern */
Gfx* func_global_asm_8063A968(Gfx*, s32);           /* extern */
void* func_global_asm_80656B98(Gfx*, s32, s32);       /* extern */
Gfx* func_global_asm_8065D994(Gfx*, s16);           /* extern */
void* func_global_asm_8065FD88(Gfx*, s32, s32);       /* extern */

typedef struct Struct807F6C0C {
    s16 unk0;
    s16 unk2;
    s16 unk4;
    s16 unk6;
    void *unk8;
    void *unkC;
} Struct807F6C0C;

typedef struct {
    u8 loaded;
    u8 unk1;
    u8 unk2;
    u8 unk3;
    u8 unk4;
    u8 visible;
    u8 unk6;
    u8 unk7;
    s32 unk8;
    s32 unkC;
    s32 unk10;
    void *unk14;
    void *unk18;
    void *unk1C;
    void *unk20;
    s32 unk24;
    u8 pad28[0x2C - 0x28];
    s32 unk2C;
    s32 unk30;
    s32 unk34;
    s32 unk38;
    u8 pad3C[0x4C - 0x3C];
    void *unk4C;
    s32 unk50;
    u8 pad54[0x58 - 0x54];
    void *unk58;
    u8 pad5C[0x60 - 0x5C];
    s32 unk60[1];
    s32 unk64;
    s32 deload1;
    s32 deload2;
    s32 deload3;
    s32 deload4;
    void *unk78;
    void *unk7C;
    s16 unk80;
    s16 unk82;
    s16 unk84;
    s16 unk86;
    u8 pad2[0x1C8 - 0x88];
} Chunk;

typedef struct Struct80630B70 Struct80630B70;
struct Struct80630B70 {
    Actor *unk0;
    s32 unk4;
    s32 unk8;
    f32 unkC;
    u8 pad10[0x14-0x10];
    Struct80630B70 *unk14;
    u8 pad18[0x24 - 0x18];
    u8 unk24;
};

extern void* D_global_asm_807F5DE8;
extern void* D_global_asm_807F5DEC;
extern u8 D_global_asm_807F5FEC;
extern Struct80630B70* D_global_asm_807F5FFC;
extern u8 D_global_asm_807F6009;
extern s32 D_global_asm_807F600C;
extern s16 D_global_asm_807F6BF0[];
extern u8 D_global_asm_807F6C08;
extern Struct807F6C0C* D_global_asm_807F6C0C;
extern s16 D_global_asm_807F6C58[];
extern s32 D_global_asm_807F6C80;
extern void* D_global_asm_807F7074;
extern Chunk *chunk_array_pointer;
extern void func_global_asm_8062EE48(u8 arg0);
extern Gfx *func_critter_80027034(Gfx *dl);
extern Gfx *func_global_asm_806634A4(Gfx *dl);
extern Gfx *func_global_asm_80630B70(Gfx*, void *, f32, f32, f32, s32, s16, u8);
extern Gfx *func_global_asm_806592B4(Gfx *dl);
extern Gfx *func_global_asm_8062EDA8(Gfx *dl, u8 arg1);

typedef struct Struct80655DD0_arg1 {
    s32 unk0;
    s32 unk4;
    void *unk8;
} Struct80655DD0_arg1;

Gfx *determineChunkFix(Gfx *dl, Chunk *ch) {
    if (cc_number_of_players == 1) {
        // Needed for screen squishes (lol)
        gDPSetScissor(dl++, G_SC_NON_INTERLACE,
            0,
            character_change_array[cc_player_index].unk270[1],
            D_global_asm_80744490,
            character_change_array[cc_player_index].unk270[3]);
    } else {
        gDPSetScissor(dl++, G_SC_NON_INTERLACE,
            character_change_array[cc_player_index].unk270[0],
            character_change_array[cc_player_index].unk270[1],
            character_change_array[cc_player_index].unk270[2],
            character_change_array[cc_player_index].unk270[3]);
    }
    func_global_asm_80658E58(
        ch->deload1,
        ch->deload2,
        ch->deload3,
        ch->deload4);
    return dl;
}

// @recomp: Chunk bounds fix
RECOMP_PATCH Gfx *func_global_asm_80655DD0(Gfx * dl, Struct80655DD0_arg1 * arg1, f32 arg2, f32 arg3, f32 arg4, s32 arg5) {
    s32 i;
    s32 var_s6;
    void *var_a1;
    s32 sp60;

    var_s6 = 0;
    sp60 = D_global_asm_807F5FEC && character_change_array[cc_player_index].fov_y == 45.0;
    gDPPipeSync(dl++);
    if (arg5 & 0x10) {
        gSPMatrix(dl++, osVirtualToPhysical(D_global_asm_807F7074), G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPSegment(dl++, 0x06, osVirtualToPhysical(D_global_asm_807F5DE8));
        gSPSegment(dl++, 0x07, osVirtualToPhysical(D_global_asm_807F5DEC));
        dl = func_global_asm_8062CEA8(dl, arg1, 1);
        if (arg1->unk8 != (void*)-1) {
            gDPPipeSync(dl++);
            gSPDisplayList(dl++, osVirtualToPhysical(arg1->unk8));
        }
        dl = func_global_asm_8065FD88(dl, 0, 0);
        for (i = 0; i < D_global_asm_807F6C08; i++) {
            if (D_global_asm_807F6C0C[D_global_asm_807F6BF0[i]].unk8 != (void*)-1) {
                gDPPipeSync(dl++);
                gSPDisplayList(dl++, osVirtualToPhysical(D_global_asm_807F6C0C[D_global_asm_807F6BF0[i]].unk8));
            }
            if (D_global_asm_807F6C0C[D_global_asm_807F6BF0[i]].unkC != (void*)-1) {
                gDPPipeSync(dl++);
                gSPDisplayList(dl++, osVirtualToPhysical(D_global_asm_807F6C0C[D_global_asm_807F6BF0[i]].unkC));
            }
        }
        gSPMatrix(dl++, &D_2000180, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    }
    for (i = D_global_asm_807F6C80 - 1; i >= 0; i--) {
        var_s6 = D_global_asm_807F6C58[i];
        if (chunk_array_pointer[var_s6].loaded == 1) {
            if (chunk_array_pointer[var_s6].unk2 != 0) {
                func_global_asm_8062EE48(var_s6);
                chunk_array_pointer[var_s6].unk2 = 0;
            }
            gDPPipeSync(dl++);
            dl = determineChunkFix(dl, &chunk_array_pointer[var_s6]);
            dl = func_global_asm_806592B4(dl);
            if (arg5 & 0x10) {
                dl = func_global_asm_8062EDA8(dl, var_s6);
                if (chunk_array_pointer[var_s6].unk2C != -1) {
                    gSPMatrix(dl++, osVirtualToPhysical(D_global_asm_807F7074), G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
                    gSPSegment(dl++, 0x06, osVirtualToPhysical(chunk_array_pointer[var_s6].unk58));
                    gSPSegment(dl++, 0x07, osVirtualToPhysical(D_global_asm_807F5DEC));
                    dl = func_global_asm_80656B98(dl, var_s6, 0);
                    gDPPipeSync(dl++);
                    gSPMatrix(dl++, &D_2000180, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
                }
                if (chunk_array_pointer[var_s6].unk34 != -1) {
                    gSPMatrix(dl++, osVirtualToPhysical(D_global_asm_807F7074), G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
                    gSPSegment(dl++, 0x06, osVirtualToPhysical(chunk_array_pointer[var_s6].unk58));
                    gSPSegment(dl++, 0x07, osVirtualToPhysical(D_global_asm_807F5DEC));
                    dl = func_global_asm_80656B98(dl, var_s6, 2);
                    gSPMatrix(dl++, &D_2000180, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
                }
            }
            gDPPipeSync(dl++);
            if (!(D_global_asm_807FBB64 & 0x01000000)) {
                dl = func_global_asm_8065D994(dl, var_s6);
            }
            dl = func_global_asm_8063A968(dl, chunk_array_pointer[var_s6].unk24);
            D_global_asm_807F6009 = 0xFF;
            var_a1 = ((u8)sp60) ? chunk_array_pointer[var_s6].unk18 : chunk_array_pointer[var_s6].unk14;
            dl = func_global_asm_80630B70(dl, var_a1, arg2, arg3, arg4, arg5, var_s6, sp60);
            if (arg5 & 0x10) {
                gSPLoadGeometryMode(dl++, 0);
                gSPSetGeometryMode(dl++, G_SHADE | G_SHADING_SMOOTH);
                dl = func_global_asm_806592B4(dl);
                dl = func_global_asm_8062EDA8(dl, var_s6);
                if (chunk_array_pointer[var_s6].unk30 != -1) {
                    gSPMatrix(dl++, osVirtualToPhysical(D_global_asm_807F7074), G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
                    gSPSegment(dl++, 0x06, osVirtualToPhysical(chunk_array_pointer[var_s6].unk58));
                    gSPSegment(dl++, 0x07, osVirtualToPhysical(D_global_asm_807F5DEC));
                    dl = func_global_asm_80656B98(dl, var_s6, 1);
                    gSPMatrix(dl++, &D_2000180, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
                    gDPPipeSync(dl++);
                }
                if (chunk_array_pointer[var_s6].unk38 != -1) {
                    gSPMatrix(dl++, osVirtualToPhysical(D_global_asm_807F7074), G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
                    gSPSegment(dl++, 0x06, osVirtualToPhysical(chunk_array_pointer[var_s6].unk58));
                    gSPSegment(dl++, 0x07, osVirtualToPhysical(D_global_asm_807F5DEC));
                    dl = func_global_asm_80656B98(dl, var_s6, 3);
                    gDPPipeSync(dl++);
                    gSPMatrix(dl++, &D_2000180, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
                }
            }
        }
    }
    if (arg5 & 0x10) {
        gSPMatrix(dl++, osVirtualToPhysical(D_global_asm_807F7074), G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPSegment(dl++, 0x06, osVirtualToPhysical(D_global_asm_807F5DE8));
        gSPSegment(dl++, 0x07, osVirtualToPhysical(D_global_asm_807F5DEC));
        dl = func_global_asm_8062CEA8(dl, arg1, 2);
        if (D_global_asm_807FBB64 & 0x1000) {
            gDPPipeSync(dl++);
            dl = func_critter_80027034(dl);
            gSPMatrix(dl++, osVirtualToPhysical(D_global_asm_807F7074), G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        }
        dl = func_global_asm_8065FD88(dl, var_s6, 1);
        gSPMatrix(dl++, &D_2000180, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gDPPipeSync(dl++);
        dl = func_global_asm_806634A4(dl);
    }
    for (i = D_global_asm_807F6C80 - 1; i >= 0; i--) {
        var_s6 = D_global_asm_807F6C58[i];
        if (chunk_array_pointer[var_s6].loaded == 1) {
            gDPPipeSync(dl++);
            dl = determineChunkFix(dl, &chunk_array_pointer[var_s6]);
            D_global_asm_807F6009 = 0xFF;
            var_a1 = ((u8)sp60) ? chunk_array_pointer[var_s6].unk20 : chunk_array_pointer[var_s6].unk1C;
            dl = func_global_asm_80630B70(dl, var_a1, arg2, arg3, arg4, arg5, var_s6, sp60);
            if (D_global_asm_807FBB64 & 0x01000000) {
                dl = func_global_asm_8065D994(dl, var_s6);
            }
        }
    }
    if (arg5 & 0x10) {
        gSPMatrix(dl++, osVirtualToPhysical(D_global_asm_807F7074), G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPSegment(dl++, 0x06, osVirtualToPhysical(D_global_asm_807F5DE8));
        gSPSegment(dl++, 0x07, osVirtualToPhysical(D_global_asm_807F5DEC));
        dl = func_global_asm_8062CEA8(dl, arg1, 3);
        gSPMatrix(dl++, &D_2000180, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gDPPipeSync(dl++);
    }
    gDPPipeSync(dl++);
    gDPSetScissor(dl++, G_SC_NON_INTERLACE,
        character_change_array[cc_player_index].unk270[0],
        character_change_array[cc_player_index].unk270[1],
        character_change_array[cc_player_index].unk270[2],
        character_change_array[cc_player_index].unk270[3]);
    func_global_asm_80658E58(
        character_change_array[cc_player_index].unk270[0],
        character_change_array[cc_player_index].unk270[1],
        character_change_array[cc_player_index].unk270[2],
        character_change_array[cc_player_index].unk270[3]);
    D_global_asm_807F600C = 0;
    dl = func_global_asm_80630B70(dl, D_global_asm_807F5FFC, arg2, arg3, arg4, arg5, -1, 0U);
    dl = func_global_asm_8065D994(dl, -1);
    return dl;
}

//@recomp: Water Screen Overlay
RECOMP_PATCH Gfx *func_global_asm_80701CA0(Gfx *dl) {
    CameraPaad* camera_paad;
    PlayerAdditionalActorData* player_aad;
    f32 var_f2;
    u8 spC3;
    u8 var_fp;
    s8 spBA;
    u8 i;
    Mtx* sp6C;
    Mtx* sp68;

    spC3 = FALSE;
    var_fp = 0x64;
    spBA = FALSE;
    switch (current_map) {
        case MAP_GALLEON_MERMAID:
            spBA = TRUE;
            goto block_12;
        case MAP_GALLEON_SUBMARINE:
        case MAP_GALLEON_SHIPWRECK_DIDDY_LANKY_CHUNKY:
        case MAP_GALLEON_SHIPWRECK_DK_TINY:
        case MAP_GALLEON_SHIPWRECK_LANKY_TINY:
            return dl;
        case MAP_GALLEON:
            if (((character_change_array->chunk == 9) || (character_change_array->chunk == 0xA)) && (isFlagSet(0x9C, 0U))) {
                return dl;
            }
        default:
    block_12:
            for (i = 0; i < cc_number_of_players; i++) {
                if (character_change_array[i].does_player_exist) {
                    player_aad = character_change_array[i].playerPointer->PaaD;
                    camera_paad = player_aad->unk104->CaaD;
                    if (spBA || (
                        character_change_array[i].unk2E8 && 
                        (character_change_array[i].unk220 < (character_change_array[i].unk24C + 3.0f))
                    )) {
                        spC3 = TRUE;
                        gDPPipeSync(dl++);
                        dl = func_global_asm_805FCFD8(dl);
                        gDPSetScissor(dl++, G_SC_NON_INTERLACE,
                            gScissorUpLX,
                            gScissorUpLY,
                            gScissorLowerRightX,
                            gScissorLowerRightY);
                        gDPSetRenderMode(dl++, G_RM_CLD_SURF, G_RM_CLD_SURF2);
                        gDPSetCombineMode(dl++, G_CC_PRIMITIVE, G_CC_PRIMITIVE);
                        var_f2 = character_change_array[i].unk24C - character_change_array[i].unk220;
                        switch (current_map) {
                            case MAP_GALLEON:
                                var_f2 *= 0.07;
                                break;
                            case MAP_GALLEON_TREASURE_CHEST:
                                var_fp = 50;
                                var_f2 *= 0.07;
                                break;
                            case MAP_GALLEON_PUFFTOSS:
                                var_fp = 50;
                            default:
                                var_f2 *= 0.4;
                                break;
                        }
                        if (var_f2 > 80.0f) {
                            var_f2 = 80.0f;
                        }
                        if (D_global_asm_807FD890) {
                            player_aad->unk1E8 = var_f2;
                        } else {
                            player_aad->unk1E8 += ((var_f2 - player_aad->unk1E8) * 0.05);
                            var_f2 = (u8) player_aad->unk1E8;
                        }
                        gDPSetPrimColor(dl++, 0, 0, 0x00, 0x00, 0x3C, (u8)(var_fp + var_f2));
                        gDPSetCycleType(dl++, G_CYC_1CYCLE);\
                        gSPMatrix(dl++, &D_2000080, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_PROJECTION);
                        gSPMatrix(dl++, &D_2000180, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
                        //@recomp: Used to be a verts-based draw, now is just fillrect
                        gDPFillRectangle(dl++,
                            character_change_array[i].unk270[0],
                            character_change_array[i].unk270[1],
                            character_change_array[i].unk270[2],
                            character_change_array[i].unk270[3]
                        );
                        gDPPipeSync(dl++);
                    }
                    character_change_array[i].unk2E8 = camera_paad->unkFA;
                    character_change_array[i].unk24C = camera_paad->unk90;
                }
            }
            if (spC3) {
                gSPMatrix(dl++, &D_2000000, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_PROJECTION);
                gSPMatrix(dl++, &D_2000200, G_MTX_NOPUSH | G_MTX_MUL | G_MTX_PROJECTION);
                gSPMatrix(dl++, &D_2000180, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
                D_global_asm_807FD890 = 0;
            } else {
                D_global_asm_807FD890 = 1;
            }
            break;
    }
    return dl;
}

//@recomp: Sandstorm screen overlay
RECOMP_PATCH Gfx* func_global_asm_8068D264(Gfx* dl, f32* cooldown_timer) {
    void* temp_v0;
    f32 cooldown;
    f32 half_lag;

    cooldown = *cooldown_timer;
    half_lag = D_global_asm_80744478 * 0.5;
    temp_v0 = getPointerTableFile(TABLE_25_TEXTURES_GEOMETRY, 0x173C, 1U, 0U);
    func_global_asm_8066B434(temp_v0, 0x1B2, 0x46);
    gDPPipeSync(dl++);
    gDPSetScissor(dl++, G_SC_NON_INTERLACE, \
        character_change_array->unk270[0], \
        character_change_array->unk270[1], \
        character_change_array->unk270[2], \
        character_change_array->unk270[3]);
    gDPSetCycleType(dl++, G_CYC_1CYCLE);
    gDPSetTextureLOD(dl++, G_TL_LOD);
    gSPLoadGeometryMode(dl++, 0);
    gSPSetGeometryMode(dl++, G_SHADE | G_SHADING_SMOOTH);
    gSPTexture(dl++, 0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON);
    gDPSetRenderMode(dl++, G_RM_CLD_SURF, G_RM_CLD_SURF2);
    gDPSetCombineMode(dl++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
    if (current_map == MAP_AZTEC) {
        gDPSetPrimColor(dl++, 0, 0, 0x8A, 0x52, 0x16, 200.0f * cooldown);
    } else {
        gDPSetPrimColor(dl++, 0, 0, 0xFF, 0xFF, 0xFF, 35.0f * cooldown);
    }
    gDPLoadTextureBlock(dl++, OS_PHYSICAL_TO_K0(temp_v0), G_IM_FMT_IA, G_IM_SIZ_8b, 64, 64, 0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, 6, 6, G_TX_NOLOD, G_TX_NOLOD);
    gSPTextureRectangle(
        dl++,
        character_change_array->unk270[0] * 4,
        character_change_array->unk270[1] * 4,
        character_change_array->unk270[2] * 4,
        character_change_array->unk270[3] * 4,
        G_TX_RENDERTILE,
        (s32)(D_global_asm_8075022C * 8.0f),
        (s32)(D_global_asm_80750228 * 8.0f),
        -1024,
        1024
    );
    gDPPipeSync(dl++);

    if (current_map == MAP_AZTEC) {
        D_global_asm_80750228 -= (2.0 * half_lag);
        D_global_asm_8075022C -= (14.0 * half_lag);
    } else {
        D_global_asm_80750228 -= (f64)half_lag;
        D_global_asm_8075022C -= (0.5 * half_lag);
    }
    if (D_global_asm_80750228 < 0.0) {
        D_global_asm_80750228 += 255.0f;
    }
    if (D_global_asm_8075022C < 0.0) {
        D_global_asm_8075022C += 255.0f;
    }
    return dl;
}

//@recomp: Patch "wrong cutscene" fade transition to match func_global_asm_80703374 (roughly)
RECOMP_PATCH Gfx *func_global_asm_80703AB0(Gfx *dl, u8 arg1) {
    if (arg1 != 0) {
        gSPClearGeometryMode(dl++, G_ZBUFFER | G_SHADE | G_CULL_BOTH | G_FOG | G_LIGHTING | G_TEXTURE_GEN | G_TEXTURE_GEN_LINEAR | G_LOD | G_SHADING_SMOOTH | G_CLIPPING | 0x0040F9FA);
        gSPSetGeometryMode(dl++, G_SHADE | G_SHADING_SMOOTH);
        gSPTexture(dl++, 0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF);
        gDPPipeSync(dl++);
        gDPSetRenderMode(dl++, G_RM_CLD_SURF, G_RM_CLD_SURF2);
        gDPSetPrimColor(dl++, 0, 0, 0x00, 0x00, 0x00, arg1);
        gDPSetCycleType(dl++, G_CYC_1CYCLE);
        gDPSetCombineMode(dl++, G_CC_PRIMITIVE, G_CC_PRIMITIVE);
        gDPFillRectangle(dl++, gScissorUpLX, gScissorUpLY, gScissorLowerRightX, gScissorLowerRightY);
    }
    gDPPipeSync(dl++);
    return dl;
}

// @recomp: Patch the static effect in the Rap to fill the full bounds
RECOMP_PATCH Gfx *func_global_asm_807035C4(Gfx *dl, Actor *arg1) {
    s16 var_s1;
    s16 temp_s2;
    s16 temp_t2;

    gDPPipeSync(dl++);
    gDPSetRenderMode(dl++, G_RM_CLD_SURF, G_RM_CLD_SURF2);
    gSPTexture(dl++, 0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF);
    gDPSetPrimColor(dl++, 0, 0, 0xC8, 0xC8, 0xC8, 0x80);
    gDPSetCycleType(dl++, G_CYC_1CYCLE);
    gDPSetCombineLERP(dl++, NOISE, 0, PRIMITIVE, 0, 0, 0, 0, PRIMITIVE, NOISE, 0, PRIMITIVE, 0, 0, 0, 0, PRIMITIVE);
    gDPSetScissor(dl++, G_SC_NON_INTERLACE, character_change_array->unk270[0], character_change_array->unk270[1], character_change_array->unk270[2], character_change_array->unk270[3]);
    gDPFillRectangle(dl++, character_change_array->unk270[0], character_change_array->unk270[1], character_change_array->unk270[2], character_change_array->unk270[3]);
    gDPPipeSync(dl++);
    gDPSetPrimColor(dl++, 0, 0, 0x00, 0x00, 0x00, 0xFF);
    gDPSetCombineMode(dl++, G_CC_PRIMITIVE, G_CC_PRIMITIVE);
    for (var_s1 = 0; var_s1 < 10; var_s1++) {
        temp_s2 = (RANDNUM() >> 0xF) % (character_change_array->unk270[3] - character_change_array->unk270[1]) + character_change_array->unk270[1];
        temp_t2 = ((RANDNUM() >> 0xF) % 10) + 2;
        gDPFillRectangle(dl++, character_change_array->unk270[0], temp_s2, character_change_array->unk270[2], temp_s2 + temp_t2);
    }
    gDPPipeSync(dl++);
    return dl;
}

RECOMP_PATCH s32 func_global_asm_806522CC(s16 arg0, s16 arg1, s16 arg2) {
    return 1;
}

#define macro_8062DBDC_IF(a0, a1, a2) ((a0 * D_global_asm_807F5E50[0]) + (a1 * D_global_asm_807F5E50[1]) + (a2 * D_global_asm_807F5E50[2])) + D_global_asm_807F5E50[3]

//@recomp: Culling patch for maps
RECOMP_PATCH s32 func_global_asm_8062DBDC(s16 arg0, s16 arg1, s16 arg2, s16 arg3, s16 arg4, s16 arg5, f32 arg6, f32 arg7, f32 arg8, f32 arg9, Struct8062DBDC *arg10) {
    s32 var_a0;
    s32 var_a1;
    s32 var_a2;
    s32 i;
    s32 pad[3];
 
    if (macro_8062DBDC_IF(arg0, arg1, arg2) < 0.0) {
        if (macro_8062DBDC_IF(arg0, arg1, arg5) < 0.0) {
            if (macro_8062DBDC_IF(arg0, arg4, arg2) < 0.0) {
                if (macro_8062DBDC_IF(arg0, arg4, arg5) < 0.0) {
                    if (macro_8062DBDC_IF(arg3, arg1, arg2) < 0.0) {
                        if (macro_8062DBDC_IF(arg3, arg1, arg5) < 0.0) {
                            if (macro_8062DBDC_IF(arg3, arg4, arg2) < 0.0) {
                                if (macro_8062DBDC_IF(arg3, arg4, arg5) < 0.0) {
                                    return TRUE;  //@recomp: Was false
                                 }
                            }
                        }
                    }
                }
            }
        }
    }
    var_a0 = FALSE;
    var_a1 = FALSE;
    var_a2 = FALSE;
    if ((arg0 <= arg6) && (arg6 <= arg3) && (arg1 <= arg7) && (arg7 <= arg4) && (arg2 <= arg8) && (arg8 <= arg5)) {
        return TRUE;
    }
    for (i = 0; (i < 5) && (!var_a2); i++) {
        if ((arg10[i].unk0[7] + (((arg10[i].unk0[4] * arg0) + (arg10[i].unk0[5] * arg1)) + (arg10[i].unk0[6] * arg2))) < 0) {
            var_a0 = TRUE;
        }
        if ((arg10[i].unk0[7] + (((arg10[i].unk0[4] * arg0) + (arg10[i].unk0[5] * arg4)) + (arg10[i].unk0[6] * arg2))) < 0) {
            var_a1 = TRUE;
        }
        if ((var_a0) && (var_a1)) {
            var_a2 = TRUE;
        }
    }
    if ((!var_a2) && (func_global_asm_8062E040(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8) < arg9)) {
        return TRUE;
    }
    for (i = 0; i < 5; i++) {
        if (func_global_asm_8062E1F8(i, arg0, arg1, arg2, arg3, arg4, arg5, arg10)) {
            return TRUE;
        }
    }
    return TRUE;  // @recomp: Was false
}

//@recomp: Audio buffer get
RECOMP_PATCH s32 func_global_asm_80601EE4(Struct8076D708 *arg0, Struct8076D708 *lastInfo) {
    s16 *audioPtr;
    s32 var_v1; // samples left?
    s32 sp34;
    s32 *new_var;
    Acmd *temp_v1; // cmdp
    s32 temp;

    audioPtr = (s16 *) osVirtualToPhysical(arg0->unk0); // audioPtr = (s16 *) osVirtualToPhysical(info->data);

    new_var = &D_global_asm_80770558;
    func_global_asm_80602314();
    var_v1 = osAiGetLength() >> 2;
    if (lastInfo) {
        s32 frameSamples = lastInfo->unk4 << 2;
        osAiSetNextBuffer(lastInfo->unk0, frameSamples); // outputDataPointer, frameSamples
    }
    if ((var_v1 < 0x5C) && (*new_var == 0)) {
        arg0->unk4 = D_global_asm_8077019C;
        D_global_asm_80770558 = 2;
    } else {
        temp = *new_var;
        if ((var_v1 >= 0x115) && (temp == 0)) {
            arg0->unk4 = D_global_asm_80770194;
            D_global_asm_80770558 = 2;
        } else {
            arg0->unk4 = 0x2E0;
            if (temp != 0) {
                temp -= 1;
                D_global_asm_80770558 = temp;
            }
        }
    }

    temp_v1 = n_alAudioFrame((Acmd *) (&D_global_asm_8076D4D0)[D_global_asm_807452C8].unk0[0], &sp34, audioPtr, (s32) arg0->unk4);

    if (sp34 == 0) {
        return 0;
    }
    
    arg0->unk8 = 0; //  next
    arg0->unk5C = &D_global_asm_8076D6D0; // msgQ
    arg0->unk60 = (OSMesg) (&arg0->unk68); // msg
    arg0->unk10 = 1;
    arg0->unk58 = &D_global_asm_8076D4C0;
    arg0->unk48 = (Acmd *) (&D_global_asm_8076D4D0)[D_global_asm_807452C8].unk0[0];
    arg0->unk4C = (temp_v1 - ((Acmd *) (&D_global_asm_8076D4D0)[D_global_asm_807452C8].unk0[0])) * sizeof(Acmd); // t->list.t.data_size
    arg0->unk18 = 2; // t->list.t.flags = OS_TASK_DP_WAIT;
    arg0->unk20 = &D_805FB000;
    arg0->unk24 = (&D_805FB0D0) - (&D_805FB000); // t->list.t.ucode_boot_size = (s32) ((s32) rspbootTextEnd - (s32) rspbootTextStart);
    arg0->unk1C = 0;
    arg0->unk28 = &D_global_asm_80741310;
    arg0->unk30 = &D_global_asm_80760590;
    arg0->unk34 = SP_UCODE_DATA_SIZE; // t->list.t.ucode_data_size = SP_UCODE_DATA_SIZE;
    arg0->unk50 = 0;
    arg0->unk54 = 0x400;

    osWritebackDCacheAll();

    osSendMesg(
        (OSMesgQueue *) func_global_asm_8060EE58((s32) (&D_global_asm_80767A40)), // OSMesgQueue*
        &arg0->unk8, // OsMesg*
        0 // flags
    );

    D_global_asm_807452C8 ^= 1; // swap which acmd list you use each frame

    return 1;
}

extern void func_arcade_80026680(Gfx **dl_ptr);
extern void func_arcade_800268AC(Gfx **dl_ptr);
extern void func_arcade_80026EF4(Gfx **dl_ptr);
extern void func_arcade_800275E8(Gfx **dl_ptr);
extern void func_arcade_80027A38(Gfx **dl_ptr);
extern u8  D_arcade_8004C724;
extern s32 D_arcade_8004C6DC;
extern u8  arcade_background_visual;

// @recomp: Arcade DL Stuff
RECOMP_PATCH void func_arcade_800259D0(Gfx **arg0) {
    Gfx *dl = *arg0;
    gDPSetAlphaCompare(dl++, G_AC_NONE);
    gDPSetTexturePersp(dl++, G_TP_NONE);
    // @recomp: Remove texture filtering
    gDPSetTextureFilter(dl++, G_TF_POINT);
    gDPSetTextureConvert(dl++, G_TC_FILT);
    gDPSetTextureDetail(dl++, G_TD_CLAMP);
    gDPSetTextureLOD(dl++, G_TL_TILE);
    gDPSetTextureLUT(dl++, G_TT_NONE);
    gDPSetPrimColor(dl++, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF);
    gDPSetRenderMode(dl++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
    gDPSetCombineMode(dl++, G_CC_MODULATEIDECALA_PRIM, G_CC_MODULATEIDECALA_PRIM);
    gDPSetScissor(dl++, G_SC_NON_INTERLACE, 48, 36, 272, 232);

    switch (D_arcade_8004C724) {
        case 0://L80025B68
            if (D_arcade_8004C6DC & 0x200) {
                func_arcade_80026680(&dl);
            }
            break;
        case 1:
        case 4:
        case 5://L80025B88
            switch (arcade_background_visual) {
                case 1:
                    func_arcade_800268AC(&dl);
                    break;
                case 2:
                    func_arcade_80026EF4(&dl);
                    break;
                case 3:
                    func_arcade_800275E8(&dl);
                    break;
                default:
                    func_arcade_80027A38(&dl);
                    break;
            }
            break;
    }
    *arg0 = dl;
}

// @recomp: (Prevent the ) draw of borders for overscan
RECOMP_PATCH Gfx *func_global_asm_80704960(Gfx *dl) {
    return dl;
}

typedef struct {
    f32 unk0;
    f32 unk4;
    f32 unk8;
    f32 unkC;
    f32 unk10;
    f32 unk14;
    f32 unk18;
    f32 unk1C;
    f32 unk20;
    f32 unk24;
    f32 unk28;
    f32 unk2C;
} Struct807F6C14;
extern Struct807F6C14 *D_global_asm_807F6C14;
typedef struct {
    f32 unk0;
    f32 unk4;
} Struct807F6C88;
typedef struct {
    f32 unk0;
    f32 unk4;
    f32 unk8;
} Struct807F6D78;

extern Struct807F6C88 D_global_asm_807F6C88;
extern Struct807F6C88 D_global_asm_807F6C90;
extern Struct807F6C88 D_global_asm_807F6C98;
extern Struct807F6C88 D_global_asm_807F6CA0;
extern Struct807F6C88 D_global_asm_807F6CA8;
extern Struct807F6C88 D_global_asm_807F6CB0;
extern Struct807F6D78 D_global_asm_807F6F58;
extern Struct807F6D78 D_global_asm_807F6E68;
extern Struct807F6D78 D_global_asm_807F6D78[];
void func_global_asm_806582F8(Struct807F6D78 *, Struct807F6D78 *, s32, s32 *, s32);

// @recomp: Frustum Bound check
RECOMP_PATCH s32 func_global_asm_80658134(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    s32 sp28;
    s32 sp24;
    Struct807F6C14 *temp_v0;
    s32 var_a2;

    sp24 = 0;
    sp28 = 0;
    temp_v0 = &D_global_asm_807F6C14[arg0];
    D_global_asm_807F6C88.unk0 = temp_v0->unk0;
    D_global_asm_807F6C88.unk4 = temp_v0->unk10;
    D_global_asm_807F6C90.unk0 = temp_v0->unk20;
    D_global_asm_807F6C90.unk4 = temp_v0->unk4;
    D_global_asm_807F6C98.unk0 = temp_v0->unk14;
    D_global_asm_807F6C98.unk4 = temp_v0->unk24;
    D_global_asm_807F6CA0.unk0 = temp_v0->unk8;
    D_global_asm_807F6CA0.unk4 = temp_v0->unk18;
    D_global_asm_807F6CA8.unk0 = temp_v0->unk28;
    D_global_asm_807F6CA8.unk4 = temp_v0->unkC;
    D_global_asm_807F6CB0.unk0 = temp_v0->unk1C;
    D_global_asm_807F6CB0.unk4 = temp_v0->unk2C;

    // @recomp: Disable X Viewport checks with depth buffer checking
    func_global_asm_806582F8((Struct807F6D78 *)&D_global_asm_807F6C88, &D_global_asm_807F6E68, 4, &sp28, 0);
    if (sp28 == 0) return 0;
    _memcpy(&D_global_asm_807F6F58, &D_global_asm_807F6E68, sizeof(tuple_f) * sp28);
    sp24 = sp28;
    func_global_asm_806582F8(&D_global_asm_807F6F58, &D_global_asm_807F6E68, sp24, &sp28, 2);
    if (sp28 == 0) return 0;
    _memcpy(&D_global_asm_807F6F58, &D_global_asm_807F6E68, sizeof(tuple_f) * sp28);
    sp24 = sp28;
    func_global_asm_806582F8(&D_global_asm_807F6F58, D_global_asm_807F6D78, sp24, &sp28, 4);
    var_a2 = sp28;
    if (var_a2 < 3) {
        var_a2 = 0;
    }
    return var_a2;
}

// Used for chunk debugging
s32 func_global_asm_806574B8(s32);

typedef struct Struct807F6C1C {
    u8 pad0[0x2];
    s16 unk2;
    s16 unk4;
    s16 unk6;
    u8 unk8;
    u8 unk9;
} Struct807F6C1C;

extern Struct807F6C0C *D_global_asm_807F6C0C;
extern s32 D_global_asm_807F6C10;
extern Struct807F6C1C *D_global_asm_807F6C1C;
extern s32 D_global_asm_807F6C20;
extern u8 *D_global_asm_807F6C2C;
extern s16 D_global_asm_807F6C30[];
extern u8 D_global_asm_807F7078[];
extern u8 func_global_asm_806575D0(s32 arg0, f32 arg1, f32 arg2, f32 arg3);
extern void func_global_asm_8065756C(s16 arg0);
extern void func_global_asm_80657CB0(s32 arg0, f32 arg1, f32 arg2, f32 arg3, f32 *arg4, f32 *arg5, f32 *arg6);
extern void func_global_asm_80657E24(s32 arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4, s32 arg5, s32 arg6, s32 arg7, s32 *arg8, s32 *arg9, s32 *argA, s32 *argB);
extern s32 func_global_asm_80657F14(s32 arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4, s32 arg5, s32 arg6, s32 arg7, s32 *arg8, s32 *arg9, s32 *argA, s32 *argB);
extern s32 func_global_asm_80655CF8(s16 arg0, s32 arg1);
extern u8 func_global_asm_80658000(s32 arg0, f32 arg1, f32 arg2, f32 arg3, s16 arg4);
extern void func_global_asm_80658624(s32 arg0, s32 *arg1, s32 *arg2, s32 *arg3, s32 *arg4);
extern void func_global_asm_80657508(s32 arg0);
extern s32 func_global_asm_80663040(s32 arg0);

// @recomp: Chunk Renderer
RECOMP_PATCH void func_global_asm_80656F14(s16 arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4, s32 arg5, f32 arg6, f32 arg7, f32 arg8) {
    s32 temp_s1;
    s32 temp_v0;
    s32 var_s0;
    s32 spC8;
    s32 spC4;
    s32 spC0;
    s32 spBC;
    s32 spB8;
    s32 spB4;
    s32 spB0;
    s32 spAC;
    s32 spA8;
    s32 spA4;
    s32 spA0;
    s32 sp9C;
    u8 cond2;
    u8 cond3;
    s32 cond1;
    s32 var_s2;
    s32 var_v1;

    func_global_asm_8065756C(arg0);
    for (var_s2 = 0; var_s2 < D_global_asm_807F6C20; var_s2++) {
        if ((arg0 == D_global_asm_807F6C1C[var_s2].unk2) && (!func_global_asm_80655CF8(D_global_asm_807F6C1C[var_s2].unk4, arg1))) {
            if (D_global_asm_807F6C1C[var_s2].unk9) {
                D_global_asm_807F6C1C[var_s2].unk8 = 1;
                temp_s1 = var_s2 / 2;
                if (func_global_asm_806574B8(temp_s1)) {
                    cond3 = func_global_asm_806575D0(temp_s1, arg6, arg7, arg8);
                    if (cond3) {
                        spB0 = arg4;
                        spB8 = arg2;
                        spB4 = arg3;
                        spAC = arg5;
                    } else if (func_global_asm_80658000(temp_s1, arg6, arg7, arg8, D_global_asm_807F6C1C[var_s2].unk6)) {
                        cond1 = func_global_asm_80658134(temp_s1, arg6, arg7, arg8);
                        if (cond1) {
                            cond2 = TRUE;
                            spB0 = arg4;
                            spB8 = arg2;
                            spB4 = arg3;
                            spAC = arg5;
                            // func_global_asm_80658624(cond1, &spC8, &spC4, &spC0, &spBC);
                            // if (newly_pressed_input & 0x0800) {
                            //     recomp_printf("--------------\n");
                            //     recomp_printf("Chunk %d:\n", D_global_asm_807F6C1C[var_s2].unk4);
                            //     recomp_printf("Ch %d, %d, %d, %d\n", spC8, spC4, spC0, spBC);
                            //     recomp_printf("SC %d, %d, %d, %d\n", arg2, arg3, arg4, arg5);
                            // }
                            // cond2 = func_global_asm_80657F14(spC8, spC4, spC0, spBC, arg2, arg3, arg4, arg5, &spB8, &spB4, &spB0, &spAC);
                        }
                    } else {
                        cond1 = 0;
                        var_v1 = FALSE;
                        var_s0 = 0;
                        while (!var_v1 && (var_s0 < D_global_asm_807F6C10)) {
                            if ((var_s2 == D_global_asm_807F6C0C[var_s0].unk2) && (D_global_asm_807F6C0C[var_s0].unk0 == D_global_asm_807F6C1C[var_s2].unk4)) {
                                var_v1 = TRUE;
                            } else {
                                var_s0++;
                            }
                        }
                        if ((var_v1 != 0) && (func_global_asm_80658000(temp_s1, arg6, arg7, arg8, D_global_asm_807F6C0C[var_s0].unk4))) {
                            temp_v0 = func_global_asm_80658134(temp_s1, arg6, arg7, arg8);
                            if (temp_v0) {
                                // func_global_asm_80658624(temp_v0, &spC8, &spC4, &spC0, &spBC);
                                // if (func_global_asm_80657F14(spC8, spC4, spC0, spBC, arg2, arg3, arg4, arg5, &spB8, &spB4, &spB0, &spAC) != 0) {
                                    func_global_asm_80657508(var_s0);
                                // }
                            }
                        }
                        // if (func_global_asm_80663040(var_s2)) {
                        //     temp_v0 = func_global_asm_80658134(temp_s1, arg6, arg7, arg8);
                        //     if (temp_v0) {
                        //         func_global_asm_80658624(temp_v0, &spC8, &spC4, &spC0, &spBC);
                        //         if (func_global_asm_80657F14(spC8, spC4, spC0, spBC, arg2, arg3, arg4, arg5, &spB8, &spB4, &spB0, &spAC)) {
                        //             D_global_asm_807F6C2C[var_s2] = 1;
                        //         }
                        //     }
                        // }
                    }
                    if (((cond1) && (cond2)) || (cond3)) {
                        D_global_asm_807F6C2C[var_s2] = 1;
                        temp_s1 = D_global_asm_807F6C1C[var_s2].unk4;
                        if (D_global_asm_807F7078[temp_s1]) {
                            chunk_array_pointer[temp_s1].loaded = 1;
                            chunk_array_pointer[temp_s1].unk1 |= 1 << cc_player_index;
                            func_global_asm_80657E24(spB8, spB4, spB0, spAC,
                                chunk_array_pointer[temp_s1].deload1,
                                chunk_array_pointer[temp_s1].deload2,
                                chunk_array_pointer[temp_s1].deload3,
                                chunk_array_pointer[temp_s1].deload4,
                                &spA8, &spA4, &spA0, &sp9C);
                            chunk_array_pointer[temp_s1].deload1 = spA8;
                            chunk_array_pointer[temp_s1].deload2 = spA4;
                            chunk_array_pointer[temp_s1].deload3 = spA0;
                            chunk_array_pointer[temp_s1].deload4 = sp9C;
                            D_global_asm_807F6C30[arg1] = arg0;
                            func_global_asm_80656F14(temp_s1, arg1 + 1, spB8, spB4, spB0, spAC, arg6, arg7, arg8);
                        }
                    }
                }
            }
        }
    }
}

typedef struct Struct807F93F4 {
    f32 unk0;
    f32 unk4;
    f32 unk8;
    f32 unkC;
    u8 unk10;
    u8 unk11;
    s8 unk12;
    s8 unk13;
} Struct807F93F4;

typedef struct Struct807F93F0 {
    u8 pad0[0x38];
    s16 unk38;
    s16 unk3A;
    s16 unk3C;
    u8 unk3E;
    u8 unk3F;
} Struct807F93F0;

extern u8 *D_global_asm_807F6C2C;
extern Struct807F93F0 *D_global_asm_807F93F0;
extern Struct807F93F4 *D_global_asm_807F93F4;
extern s32 D_global_asm_807F93F8;
extern s16 func_global_asm_80665DE0(f32 arg0, f32 arg1, f32 arg2, f32 arg3);
extern f32 func_global_asm_80611BB4(f32 arg0, f32 arg1);

// @recomp: Distant Screen renderer
RECOMP_PATCH void func_global_asm_8066308C(f32 arg0, f32 arg1, f32 arg2) {
    f32 temp_f12;
    f32 delta;
    s32 i;

    for (i = 0; i < D_global_asm_807F93F8; i++) {
        D_global_asm_807F93F4[i].unk11 = 0;
        // if (D_global_asm_807F6C2C[D_global_asm_807F93F0[i].unk38]) {
            D_global_asm_807F93F4[i].unk0 = _sqrtf(SQ(arg0 - D_global_asm_807F93F4[i].unk4) + SQ(arg1 - D_global_asm_807F93F4[i].unk8) + SQ(arg2 - D_global_asm_807F93F4[i].unkC));
            temp_f12 = (D_global_asm_807F93F4[i].unk0 - D_global_asm_807F93F0[i].unk3A) / (D_global_asm_807F93F0[i].unk3C - D_global_asm_807F93F0[i].unk3A);
            if (temp_f12 > 1.0) {
                D_global_asm_807F93F4[i].unk10 = D_global_asm_807F93F0[i].unk3F;
            } else if (temp_f12 < 0.0) {
                D_global_asm_807F93F4[i].unk10 = D_global_asm_807F93F0[i].unk3E;
            } else {
                delta = D_global_asm_807F93F0[i].unk3F - D_global_asm_807F93F0[i].unk3E;
                delta *= temp_f12;
                D_global_asm_807F93F4[i].unk10 = delta + D_global_asm_807F93F0[i].unk3E;
            }
            D_global_asm_807F93F4[i].unk12 = (s32) (func_global_asm_80665DE0(arg0, arg2, D_global_asm_807F93F4[i].unk4, D_global_asm_807F93F4[i].unkC) * 0.5) % 255;
            D_global_asm_807F93F4[i].unk13 = ((s32) ((s16)((func_global_asm_80611BB4(D_global_asm_807F93F4[i].unk8 - arg1, _sqrtf(SQ(arg2 - D_global_asm_807F93F4[i].unkC) + SQ(arg0 - D_global_asm_807F93F4[i].unk4))) * 2048.0) / 3.1415927410125732) * 0.5) % 255);
            D_global_asm_807F93F4[i].unk11 = 1;
        // } else {
        //     D_global_asm_807F93F4[i].unk11 = 0;
        // }
    }
}

Gfx* func_global_asm_80703AB0(Gfx*, u8);
Gfx* func_global_asm_80703CF8(Gfx*, u8);
void func_global_asm_8070B324(f32, s32, s32 *);
u8 isIntroStoryPlaying(void);
void func_global_asm_80703850(u8 arg0);
u8 func_global_asm_805FF000(u8 map);
u8 func_global_asm_80600454(s16 arg0, u8 *arg1);
void func_global_asm_8069D2AC(u8 arg0, s16 arg1, s16 arg2, char *arg3, u16 arg4, u16 arg5, u8 arg6, u8 arg7);
u8 *getTextString(u8 fileIndex, s32 stringIndex, s32 arg2);
void func_global_asm_80602B60(s32 arg0, u8 arg1);
Gfx *func_global_asm_80703374(Gfx *dl, u8 r, u8 g, u8 b, u8 a);

extern u8 D_global_asm_80750AC0;
extern s32 D_global_asm_80754CC8[];
extern s16 D_global_asm_8076A0AA;
extern u8 D_global_asm_8076A0B1;
extern f32 D_global_asm_807FD888;
extern f32 loading_zone_transition_speed;
extern u8 D_global_asm_8076A0AB;

// @recomp: Transition manager
RECOMP_PATCH Gfx* func_global_asm_80704484(Gfx *dl, u8 arg1) {
    f32 temp_f0;
    u8 sp5B;
    u16 var_v1;
    f32 sp54;
    s32 var_v0;

    if (loading_zone_transition_speed != 0.0f) {
        if (arg1 == 2) {
            var_v0 = 2;
        } else {
            var_v0 = 1;
        }
        D_global_asm_807FD888 += loading_zone_transition_speed * var_v0;
        if (D_global_asm_807FD888 < 0.0f) {
            func_global_asm_805FF000(D_global_asm_8076A0AB);
            if (((D_global_asm_8076A0B1 & 0x20) == 0) && (func_global_asm_80600454(D_global_asm_8076A0AA, &sp5B))) {
                func_global_asm_8069D2AC(1, 0, 0x64, getTextString(0x23, sp5B, 1), 0, 0x28, 6, 8);
                D_global_asm_8076A0B1 |= 0x20;
            }
            func_global_asm_80602B60(0x2B, 0);
            if (isIntroStoryPlaying() == 2) {
                setIntroStoryPlaying(0);
            }
            loading_zone_transition_speed = 0.0f;
            D_global_asm_807FD888 = 0.0f;
            D_global_asm_8076A0B1 &= ~0x40;
        } else if (D_global_asm_807FD888 > 31.0f) {
            if (D_global_asm_8076A0B1 & 1) {
                func_global_asm_80602B60(0x2C, 0);
            }
            loading_zone_transition_speed = 0.0f;
            D_global_asm_807FD888 = 31.0f;
            osViBlack(1);
            D_global_asm_8076A0B1 |= 0xC;
            dl = func_global_asm_80703374(dl, 0, 0, 0, 0xFF);
        } else {
            sp54 = D_global_asm_807FD888 * 0.032258064f;

            temp_f0 = (D_global_asm_807FD888 * 255.0f) / 28.0f;
            var_v1 = (temp_f0 < 0.0f) ? 0.0f : MIN(255.0f, (temp_f0));
            
            if ((D_global_asm_807FD888 < ((loading_zone_transition_speed * 2) + 31.0f)) && (D_global_asm_8076A0B1 & 4)) {
                D_global_asm_8076A0B1 ^= 6;
            }
            if ((D_global_asm_807FD888 > 28.0f) && (arg1 != 3)) {
                dl = func_global_asm_80703374(dl, 0, 0, 0, 0xFF);
            } else {
                switch (arg1) {
                    case 3:
                        if (D_global_asm_80750AC0 == 1) {
                            func_global_asm_80703850(var_v1);
                            break;
                        }
                    case 2:
                        dl = func_global_asm_80703AB0(dl, var_v1);
                        break;
                    case 1: // Spin transition
                        // @recomp: Spin transition fix for non-overscan
                        if (D_global_asm_807FD888 > 25.0f) { // Spin progress
                            // Render nothing but black
                            dl = func_global_asm_80703374(dl, 0, 0, 0, 0xFF);
                        } else {
                            // Render spin
                            dl = func_global_asm_80703CF8(dl, var_v1);
                        }
                        break;
                    default:
                        dl = func_global_asm_80703374(dl, 0, 0, 0, var_v1);
                        break;
                }
            }
            if ((D_global_asm_8076A0B1 & 0x41) || !(D_global_asm_8076A0B1 & 0x20)) {
                s32 sp44[3] = { 0 };
                s32 pad;
                func_global_asm_8070B324(1.0f - sp54, loading_zone_transition_speed >= 0.0f, sp44);
            }
        }
    } else if (D_global_asm_807FD888 > 28.0f) {
        // @recomp: Fix a bug with mid-transition
        dl = func_global_asm_80703374(dl, 0, 0, 0, 0xFF);
    }
    return dl;
}

typedef struct {
    f32 unk0;
    f32 unk4;
    f32 unk8;
    f32 unkC;
    f32 unk10;
    f32 unk14;
    f32 unk18;
    f32 unk1C;
    f32 unk20;
    f32 unk24;
    f32 unk28;
    f32 unk2C;
    s16 unk30;
    s16 unk32;
    s16 unk34;
    u8 unk36;
    u8 unk37;
    u8 unk38;
    u8 unk39;
    s8 unk3A;
    s8 unk3B;
} Struct807F7500;

#define LIGHT_LIMIT 0x30
Struct807F7500 lightarray[LIGHT_LIMIT];
extern f32 D_global_asm_807F7ED0;
extern f32 D_global_asm_807F7ED4;
extern f32 D_global_asm_807F7ED8;
extern f32 D_global_asm_807F7EDC;
extern f32 D_global_asm_807F7EE0;
extern f32 D_global_asm_807F7EE4;
extern s16 D_global_asm_80748300;

extern f32 *D_global_asm_8076A0B4;
extern f32 *D_global_asm_8076A0B8;
extern f32 *D_global_asm_8076A0BC;
extern f32 *D_global_asm_8076A0C0;
extern f32 *D_global_asm_8076A0C4;
extern f32 *D_global_asm_8076A0C8;

extern s32 D_global_asm_807F6C28;
extern s16 D_global_asm_807F7EC8;

extern u8 D_global_asm_807F7EF8;
extern f32 D_global_asm_807F7ECC;
extern f32 D_global_asm_807F7ED0;
extern f32 D_global_asm_807F7EE8;
extern s16 D_global_asm_807F7EFA;
extern s16 D_global_asm_807F7EFC;
extern s16 D_global_asm_807F7EFE;

RECOMP_PATCH void createLight(f32 arg0, f32 arg1, f32 arg2, f32 arg3, f32 arg4, f32 arg5, f32 arg6, u8 arg7, u8 arg8, u8 arg9, u8 argA) {
    if (D_global_asm_80748300 == LIGHT_LIMIT) {
        recomp_printf("Too many lights\n");
        return;
    }
    lightarray[D_global_asm_80748300].unk34 = D_global_asm_807F7EFE;
    lightarray[D_global_asm_80748300].unk3A = D_global_asm_807F7EF8;
    lightarray[D_global_asm_80748300].unk30 = D_global_asm_807F7EFA;
    lightarray[D_global_asm_80748300].unk32 = D_global_asm_807F7EFC;
    lightarray[D_global_asm_80748300].unk8 = D_global_asm_807F7EE8;
    lightarray[D_global_asm_80748300].unk0 = D_global_asm_807F7EE4;
    lightarray[D_global_asm_80748300].unk18 = arg0;
    lightarray[D_global_asm_80748300].unk1C = arg1;
    lightarray[D_global_asm_80748300].unkC = D_global_asm_807F7EDC;
    lightarray[D_global_asm_80748300].unk4 = D_global_asm_807F7ED8;
    lightarray[D_global_asm_80748300].unk39 = arg7;
    lightarray[D_global_asm_80748300].unk10 = D_global_asm_807F7EE0;
    lightarray[D_global_asm_80748300].unk20 = arg2;
    lightarray[D_global_asm_80748300].unk24 = arg3;
    lightarray[D_global_asm_80748300].unk28 = arg4;
    lightarray[D_global_asm_80748300].unk2C = arg5;
    lightarray[D_global_asm_80748300].unk14 = arg6;
    lightarray[D_global_asm_80748300].unk36 = arg8;
    lightarray[D_global_asm_80748300].unk37 = arg9;
    lightarray[D_global_asm_80748300].unk38 = argA;
    D_global_asm_80748300++;
    D_global_asm_807F7ED8 = D_global_asm_807F7ECC;
    D_global_asm_807F7EDC = D_global_asm_807F7ED0;
    D_global_asm_807F7EE0 = D_global_asm_807F7ED4;
    D_global_asm_807F7EF8 = 0;
    D_global_asm_807F7EFA = -1;
    D_global_asm_807F7EFC = 700;
    D_global_asm_807F7EFE = 600;
    D_global_asm_807F7EE4 = 25.0f;
    D_global_asm_807F7EE8 = 65.0f;
}

typedef struct {
    Actor* unk0;
    s32 unk4;
} GlobalASMStruct53;
s32 func_global_asm_8065C240(void*);
void addActorRecolor(Actor *actor, s16 x, s16 y, s16 z, u8 alpha, u8 red, u8 green, u8 blue, u8);
void func_global_asm_8065D254(Actor *actor, s32 arg1, s32 arg2, s32 arg3, s32 arg4, s32 arg5, s32 arg6, s32 arg7, u32 arg8, s32 arg9, f32 arg10);
extern s32 D_global_asm_80748304;
extern s32 D_global_asm_80748308;
extern u16 D_global_asm_807FBB34;
extern GlobalASMStruct53 D_global_asm_807FB930[];
#define drawShadow8065A884(m) \
    temp_res = ac->shadow_opacity / 255.0; \
    func_global_asm_8065D254(ac, \
    0x3C9, 0x40, 0x40, \
    spC0 * ac->unk16E, spC4 * ac->unk16F, \
    1, 0x64, \
    (temp_res * D_global_asm_80748304 * (m)), \
    var_s3 + 0xC00, var_f24 * (m))

RECOMP_PATCH void func_global_asm_8065A884(void) {
    s32 i;
    Struct807F7500* light;
    s32 sp14C;
    f32 temp_f14;
    f32 temp_f2;
    f32 temp_f0;
    f32 temp_f0_10;
    f32 temp_f0_11;
    f32 temp_f0_6;
    f32 temp_f12;
    f32 temp_f16;
    f32 temp_f18;
    f32 temp_f20;
    f32 temp_f26;
    f32 temp_f28;
    f32 temp_f2_5;
    f32 temp_f30;
    f32 var_f20;
    f32 var_f24; // 10c
    s16 var_s3;
    u8 spC8[0x40];
    f32 spC4;
    f32 spC0;
    Actor* ac;
    f32 var_f20_2;
    f32 var_f22;
    f32 var_f2;
    s32 j;
    f32 temp_res;

    sp14C = 0;
    for (i = 0; i < D_global_asm_807FBB34; i++) {
        spC8[i] = FALSE;
    }
    for (i = 0; (i < D_global_asm_80748300) && (cc_number_of_players < 3); i++) {
        light = &lightarray[i];
        if ((func_global_asm_8065C240(light)) && (sp14C < 0xC)) {
            sp14C += 1;
            for (j = 0; j < D_global_asm_807FBB34; j++) {
                ac = D_global_asm_807FB930[j].unk0;
                if ((ac->object_properties_bitfield & 0x2000) == 0) {
                    switch (light->unk39) {
                        case 1:
                            temp_f0 = light->unk24 - light->unk18;
                            temp_f2 = light->unk28 - light->unk1C;
                            temp_f14 = light->unk2C - light->unk20;
                            var_f20 = _sqrtf(
                                SQ(temp_f0) + 
                                SQ(temp_f2) + 
                                SQ(temp_f14));
                            if (var_f20 == 0.0f) {
                                var_f20 = 0.1f;
                            }
                            temp_f26 = temp_f0 / var_f20;
                            temp_f28 = temp_f2 / var_f20;
                            temp_f30 = temp_f14 / var_f20;
                            temp_f0 = ac->position.f[0] - light->unk18;
                            temp_f2 = ac->position.f[1] - light->unk1C;
                            temp_f14 = ac->position.f[2] - light->unk20;
                            var_f20_2 = _sqrtf(
                                SQ(temp_f0) + 
                                SQ(temp_f2) + 
                                SQ(temp_f14)
                            );
                            if (var_f20_2 == 0.0f) {
                                var_f20_2 = 0.1f;
                            }
                            temp_f12 = temp_f0 / var_f20_2;
                            temp_f16 = temp_f2 / var_f20_2;
                            temp_f18 = temp_f14 / var_f20_2;
                            var_f22 = (temp_f12 * temp_f26) + (temp_f16 * temp_f28) + (temp_f18 * temp_f30);
                            if (var_f22 < 0.0f) {
                                var_f22 = 0.0f;
                            }
                            if (light->unkC <= var_f22) {
                                if (light->unk4 <= var_f22) {
                                    var_f22 = 1.0f;
                                } else {
                                    var_f22 = (var_f22 - light->unkC) * light->unk10;
                                }
                                addActorRecolor(ac,
                                    light->unk18,
                                    light->unk1C,
                                    light->unk20, 0xFF,
                                    light->unk36 * var_f22,
                                    light->unk37 * var_f22,
                                    light->unk38 * var_f22, 0U);
                                if ((ac->object_properties_bitfield & 0x01000000) && (ac->object_properties_bitfield & 4)) {
                                    temp_f0_6 = _sqrtf(
                                        SQ(light->unk20 - ac->position.f[2]) + 
                                        SQ(light->unk18 - ac->position.f[0])
                                    );
                                    if (temp_f0_6 != 0.0f) {
                                        var_f2 = temp_f0_6;
                                    } else {
                                        var_f2 = 0.001f;
                                    }
                                    var_f24 = (light->unk1C - ac->position.f[1]) / var_f2;
                                    if (!(var_f24 > 0.0f)) {
                                        var_f24 = -var_f24;
                                    }
                                    if (var_f24 == 0.0f) {
                                        var_f24 = 0.1f;
                                    }
                                    var_f24 = (ac->unk15E * 0.4f) / var_f24;
                                    var_s3 = func_global_asm_80665DE0(
                                        light->unk18, light->unk20,
                                        ac->position.f[0], ac->position.f[2]
                                    );
                                    if (ac->animation_state) {
                                        spC0 = ac->animation_state->scale[0];
                                        spC4 = ac->animation_state->scale[2];
                                    } else {
                                        spC0 = 0.15f;
                                        spC4 = 0.15f;
                                    }
                                    temp_res = ac->shadow_opacity / 255.0;
                                    func_global_asm_8065D254(ac,
                                        0x3C9, 0x40, 0x40,
                                        spC0 * ac->unk16E, spC4 * ac->unk16F,
                                        1, 0x64,
                                        (D_global_asm_80748304 * var_f22 * temp_res),
                                        var_s3 + 0xC00, var_f24 * var_f22);
                                    spC8[j] = TRUE;
                                }
                            }
                            break;
                        case 0:
                            var_f22 = _sqrtf(
                                SQ(light->unk18 - ac->position.f[0]) +
                                SQ(light->unk1C - ac->position.f[1]) + 
                                SQ(light->unk20 - ac->position.f[2])
                            ) - (f32) ac->unkCE;
                            if (var_f22 < 0.0) {
                                var_f22 = 0.1f;
                            }
                            temp_f20 = light->unk14 * 1.3f;
                            if ((ac->object_properties_bitfield & 0x01000000) && (var_f22 < temp_f20) && (ac->object_properties_bitfield & 4)) {
                                var_f2 = _sqrtf(
                                    SQ(light->unk20 - ac->position.f[2]) +
                                    SQ(light->unk18 - ac->position.f[0])
                                );
                                if (var_f2 == 0.0f) {
                                    var_f2 = 1.0f;
                                }
                                var_f24 = (light->unk1C - ac->position.f[1]) / var_f2;
                                if (!(var_f24 > 0.0f)) {
                                    var_f24 = -var_f24;
                                }
                                if (var_f24 == 0.0f) {
                                    var_f24 = 0.1f;
                                }
                                var_f24 = (ac->unk15E * 0.4f) / var_f24;
                                var_s3 = func_global_asm_80665DE0(light->unk18, light->unk20, ac->position.f[0], ac->position.f[2]);
                                if (var_f24 > 10.0f) {
                                    var_f24 = 10.0f;
                                }
                                if (ac->animation_state) {
                                    spC0 = ac->animation_state->scale[0];
                                    spC4 = ac->animation_state->scale[2];
                                } else {
                                    spC0 = 0.15f;
                                    spC4 = 0.15f;
                                }
                                spC8[j] = 1;
                            }
                            temp_f0_10 = temp_f20 / 3.0f;
                            if (var_f22 < temp_f0_10) {
                                addActorRecolor(ac,
                                    light->unk18,
                                    light->unk1C,
                                    light->unk20, 0xFFU,
                                    light->unk36,
                                    light->unk37,
                                    light->unk38, 0U);
                                if ((ac->object_properties_bitfield & 0x01000000) && (ac->object_properties_bitfield & 4)) {
                                    drawShadow8065A884(1.0f);
                                }
                            } else {
                                var_f22 -= temp_f0_10;
                                temp_f2_5 = temp_f20 - temp_f0_10;
                                if (var_f22 < temp_f2_5) {
                                    temp_f0_11 = 1.0f - (var_f22 / temp_f2_5);
                                    addActorRecolor(ac,
                                        light->unk18,
                                        light->unk1C,
                                        light->unk20, 0xFFU,
                                        light->unk36 * temp_f0_11,
                                        light->unk37 * temp_f0_11,
                                        light->unk38 * temp_f0_11, 0U);
                                    if ((ac->object_properties_bitfield & 0x01000000) && (ac->object_properties_bitfield & 4)) {
                                        drawShadow8065A884(temp_f0_11);
                                    }
                                }
                            }
                            break;
                    }
                }
            }
        }
    }
    for (i = 0; i < D_global_asm_807FBB34; i++) {
        if (spC8[i] == 0) {
            ac = D_global_asm_807FB930[i].unk0;
            if ((ac->object_properties_bitfield & 0x01000000) && (ac->object_properties_bitfield & 4)) {
                if (ac->animation_state) {
                    spC0 = ac->animation_state->scale[0];
                    spC4 = ac->animation_state->scale[2];
                } else {
                    spC0 = 0.15f;
                    spC4 = 0.15f;
                }
                temp_res = ac->shadow_opacity / 255.0;
                func_global_asm_8065D254(ac,
                    0x3C9, 0x40, 0x40,
                    spC0 * ac->unk16E, spC4 * ac->unk16F,
                    1, 0x64,
                    (D_global_asm_80748308 * temp_res),
                    0, 1.0f);
            }
        }
    }
}

typedef struct {
    f32 unk0;
    f32 unk4;
    f32 unk8;
    f32 unkC;
    f32 unk10;
    f32 unk14;
    s32 unk18;
    f32 unk1C;
    f32 unk20;
    f32 unk24;
    f32 unk28;
    f32 unk2C;
    f32 unk30;
    f32 unk34;
    s16 unk38;
    s16 unk3A;
    s16 unk3C;
    u8 unk3E;
    u8 unk3F; // Used
} Struct807F78C0;
s32 func_global_asm_8065BF18(Struct807F7500*, f32, f32, s32, s32, s32, s32, s32, s32, s32, s32);
void func_global_asm_8065BE74(s16 arg0);
extern Struct807F78C0 D_global_asm_807F78C0[];
extern s16 D_global_asm_807F7BC0;

RECOMP_PATCH s16 func_global_asm_8065BAA0(f32 arg0, f32 arg1, s32 arg2, s16 arg3, s16 arg4, s16 arg5, s16 arg6, s16 arg7, s16 arg8, s32 arg9, u8* argA) {
    f32 dz;
    f32 dy;
    f32 dx;
    f32 var_f2;
    s32 i;

    *argA = FALSE;
    D_global_asm_807F7BC0 = 0;
    for (i = 0; i < D_global_asm_80748300; i++) {
        if (func_global_asm_8065BF18(&lightarray[i], arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9)) {
            if (D_global_asm_807F7BC0 < 0xC) {
                if (lightarray[i].unk3A & 1) {
                    *argA = TRUE;
                }
                D_global_asm_807F78C0[D_global_asm_807F7BC0].unk3F = i;
                D_global_asm_807F78C0[D_global_asm_807F7BC0].unk38 = lightarray[i].unk18 * 3.0f;
                D_global_asm_807F78C0[D_global_asm_807F7BC0].unk3A = lightarray[i].unk1C * 3.0f;
                D_global_asm_807F78C0[D_global_asm_807F7BC0].unk3C = lightarray[i].unk20 * 3.0f;
                D_global_asm_807F78C0[D_global_asm_807F7BC0].unk2C = lightarray[i].unk36 / 255.0;
                D_global_asm_807F78C0[D_global_asm_807F7BC0].unk30 = lightarray[i].unk37 / 255.0;
                D_global_asm_807F78C0[D_global_asm_807F7BC0].unk34 = lightarray[i].unk38 / 255.0;
                D_global_asm_807F78C0[D_global_asm_807F7BC0].unk3E = lightarray[i].unk39;
                switch (lightarray[i].unk39) {
                    case 1:
                        dx = lightarray[i].unk24 - lightarray[i].unk18;
                        dy = lightarray[i].unk28 - lightarray[i].unk1C;
                        dz = lightarray[i].unk2C - lightarray[i].unk20;
                        var_f2 = _sqrtf(SQ(dx) + SQ(dy) + SQ(dz));
                        if (var_f2 == 0.0f) {
                            var_f2 = 0.1f;
                        }
                        D_global_asm_807F78C0[D_global_asm_807F7BC0].unk18 = 1210000;
                        D_global_asm_807F78C0[D_global_asm_807F7BC0].unk4 = lightarray[i].unk4;
                        D_global_asm_807F78C0[D_global_asm_807F7BC0].unkC = dx / var_f2;
                        D_global_asm_807F78C0[D_global_asm_807F7BC0].unk0 = lightarray[i].unkC;
                        D_global_asm_807F78C0[D_global_asm_807F7BC0].unk10 = dy / var_f2;
                        D_global_asm_807F78C0[D_global_asm_807F7BC0].unk8 = lightarray[i].unk10;
                        D_global_asm_807F78C0[D_global_asm_807F7BC0].unk14 = dz / var_f2;
                        break;
                    case 0:
                        if (lightarray[i].unk14 == 0.0f) {
                            lightarray[i].unk14 = 0.1f;
                        }
                        D_global_asm_807F78C0[D_global_asm_807F7BC0].unk20 = ((lightarray[i].unk14 / 3.0f) * 3.0f);
                        D_global_asm_807F78C0[D_global_asm_807F7BC0].unk18 = SQ(lightarray[i].unk14 * 3.0f);
                        D_global_asm_807F78C0[D_global_asm_807F7BC0].unk1C = 1.0f / ((lightarray[i].unk14 * 3.0f) - D_global_asm_807F78C0[D_global_asm_807F7BC0].unk20);
                        D_global_asm_807F78C0[D_global_asm_807F7BC0].unk28 = ((lightarray[i].unk14 * 1.3f) / 3.0f) * 3.0f;
                        D_global_asm_807F78C0[D_global_asm_807F7BC0].unk24 = 1.0f / (((lightarray[i].unk14 * 1.3f) * 3.0f) - D_global_asm_807F78C0[D_global_asm_807F7BC0].unk28);
                        break;
                }
                func_global_asm_8065BE74(D_global_asm_807F7BC0);
                D_global_asm_807F7BC0++;
            }
        }
    }
    return D_global_asm_807F7BC0;
}