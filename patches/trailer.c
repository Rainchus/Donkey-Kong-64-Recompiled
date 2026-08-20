#include "common_structs.h"
#include "patches_main.h"
#include "ui.h"

u8 *_strchr(const u8 *str, s32 c);
u32 func_dk64_boot_80002C74(const u8 *str, const u8 *src);
u8 *func_dk64_boot_80002A64(u8 *dest, const u8 *src, u32 count);
size_t strlen(const char *);
s32 getCenterOfString(s16 renderStyle, u8 *string);
void func_global_asm_80627C10(u8 arg0);
u8 *getTextString(u8 fileIndex, s32 stringIndex, s32 arg2);
void func_global_asm_8069D2AC(u8 arg0, s16 arg1, s16 arg2, char *arg3, u16 arg4, u16 arg5, u8 arg6, u8 arg7);

RECOMP_PATCH void func_global_asm_80627C5C(u8 arg0, s16 arg1, u8 arg2) {
    u8 sp98[64];
    u8 sp58[64];
    u8 *var_s0;
    u8 sp44[16];
    s16 sp42;

    switch (arg0) {
        default:
            var_s0 = getTextString(1, arg0, 1);
            break;
        case 30:
            var_s0 = (u8*)"K.ROOL:THOSE PESKY DEVELOPERS...";
            break;
        case 31:
            var_s0 = (u8*)"THEY DON'T EVEN KNOW WHEN THE DK64 RECOMP IS COMING OUT";
            break;
        case 32:
            var_s0 = (u8*)"IN FACT, SUMMER 2026 IS NEARLY OVER!";
            break;
        case 33:
            var_s0 = (u8*)"NOTHING CAN STOP ME NOW. SOON ONLY I WILL BE ABLE TO PLAY DK64!";
            break;
        case 34:
            var_s0 = (u8*)"AH, THERE YOU ARE. I TRUST YOU'VE STOCKPILED ALL THE REMAINING N64 CONSOLES?";
            break;
        case 35:
            var_s0 = (u8*)"DOGADON:OH, IT WAS AWFUL, MASTER.";
            break;
        case 36:
            var_s0 = (u8*)"THEY KNOCKED ME INTO THE LAVA AT 200 FRAMES PER SECOND IN ULTRAWIDE.";
            break;
        case 37:
            var_s0 = (u8*)"EVERYTHING IS IN 4K RESOLUTION AND OBJECTS LOAD FROM MILES AWAY.";
            break;
        case 38:
            var_s0 = (u8*)"THEY COULD TAG KONGS WITHOUT A TAG BARREL.";
            break;
        case 39:
            var_s0 = (u8*)"THOSE HORRIBLE DEVELOPERS JUST RELEASED THE DK64 RECOMP!!!";
            break;
        case 40:
            var_s0 = (u8*)"I'M JUST GONNA GO PLAY IT MYSELF. GOODBYE.";
            break;
    }
    if (_strchr(var_s0, 0x3A) != NULL) {
        sp42 = func_dk64_boot_80002C74(var_s0, ":") + 1;
        func_dk64_boot_80002A64(sp44, var_s0, sp42);
        sp44[sp42] = 0;
        func_global_asm_8069D2AC(0xAU, 0x14, 0x18, (char*)sp44, 0U, arg1, 0x32U, 0x32U);
        func_global_asm_80627C10(arg2);
        var_s0 += strlen(sp44);
    }
    if (((s16) getCenterOfString(6, var_s0)) >= 0x12D) {
        sp42 = ((s32) strlen(var_s0)) / 2;
        while (var_s0[sp42] != ' ') {
            sp42--;
        }

        func_dk64_boot_80002A64(sp98, var_s0, sp42);
        sp98[sp42] = 0;
        func_dk64_boot_80002A64(sp58, (&var_s0[sp42]) + 1, 0xFFU);
        func_global_asm_8069D2AC(0U, 0, 0xCD, (char*)sp98, 0U, arg1, 0x32U, 0x32U);
        func_global_asm_80627C10(arg2);
        func_global_asm_8069D2AC(0U, 0, 0xD7, (char*)sp58, 0U, arg1, 0x32U, 0x32U);
        func_global_asm_80627C10(arg2);
        return;
    }
    func_global_asm_8069D2AC(0U, 0, 0xCD, (char*)var_s0, 0U, arg1, 0x32U, 0x32U);
    func_global_asm_80627C10(arg2);
}