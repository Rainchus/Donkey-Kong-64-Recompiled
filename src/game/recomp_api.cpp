#include <cmath>

#include "recomp.h"
#include "librecomp/overlays.hpp"
#include "librecomp/addresses.hpp"
#include "banjo_config.h"
#include "recompinput/recompinput.h"
#include "recompui/recompui.h"
#include "recompui/renderer.h"
#include "banjo_sound.h"
#include "librecomp/helpers.hpp"
#include "../patches/input.h"
#include "../patches/graphics.h"
#include "../patches/sound.h"
#include "ultramodern/ultramodern.hpp"
#include "ultramodern/config.hpp"
#include "../lib/N64ModernRuntime/thirdparty/xxHash/xxh3.h"

extern "C" void recomp_update_inputs(uint8_t* rdram, recomp_context* ctx) {
    recompinput::poll_inputs();
}

extern "C" void recomp_puts(uint8_t* rdram, recomp_context* ctx) {
    PTR(char) cur_str = _arg<0, PTR(char)>(rdram, ctx);
    u32 length = _arg<1, u32>(rdram, ctx);

    for (u32 i = 0; i < length; i++) {
        fputc(MEM_B(i, (gpr)cur_str), stdout);
    }
}

extern "C" void recomp_exit(uint8_t* rdram, recomp_context* ctx) {
    ultramodern::quit();
}

extern "C" void recomp_error(uint8_t* rdram, recomp_context* ctx) {
    std::string str{};
    PTR(u8) str_ptr = _arg<0, PTR(u8)>(rdram, ctx);

    for (size_t i = 0; MEM_B(str_ptr, i) != '\x00'; i++) {
        str += (char)MEM_B(str_ptr, i);
    }

    recompui::message_box(str.c_str());
    assert(false);
    ultramodern::error_handling::quick_exit(__FILE__, __LINE__, __FUNCTION__);
}

extern "C" void recomp_get_gyro_deltas(uint8_t* rdram, recomp_context* ctx) {
    float* x_out = _arg<0, float*>(rdram, ctx);
    float* y_out = _arg<1, float*>(rdram, ctx);

    // TODO: use controller number
    recompinput::get_gyro_deltas(0, x_out, y_out);
}

extern "C" void recomp_get_mouse_deltas(uint8_t* rdram, recomp_context* ctx) {
    float* x_out = _arg<0, float*>(rdram, ctx);
    float* y_out = _arg<1, float*>(rdram, ctx);

    recompinput::get_mouse_deltas(x_out, y_out);
}

extern "C" void recomp_powf(uint8_t* rdram, recomp_context* ctx) {
    float a = _arg<0, float>(rdram, ctx);
    float b = ctx->f14.fl; //_arg<1, float>(rdram, ctx);

    _return(ctx, std::pow(a, b));
}

extern "C" void recomp_get_target_framerate(uint8_t* rdram, recomp_context* ctx) {
    int frame_divisor = _arg<0, u32>(rdram, ctx);

    _return(ctx, ultramodern::get_target_framerate(60 / frame_divisor));
}

extern "C" void recomp_get_window_resolution(uint8_t* rdram, recomp_context* ctx) {
    int width, height;
    recompui::get_window_size(width, height);

    gpr width_out = _arg<0, PTR(u32)>(rdram, ctx);
    gpr height_out = _arg<1, PTR(u32)>(rdram, ctx);

    MEM_W(0, width_out) = (u32)width;
    MEM_W(0, height_out) = (u32)height;
}

extern "C" void recomp_get_target_aspect_ratio(uint8_t* rdram, recomp_context* ctx) {
    ultramodern::renderer::GraphicsConfig graphics_config = ultramodern::renderer::get_graphics_config();
    float original = _arg<0, float>(rdram, ctx);
    int width, height;
    recompui::get_window_size(width, height);

    switch (graphics_config.ar_option) {
        case ultramodern::renderer::AspectRatio::Original:
        default:
            _return(ctx, original);
            return;
        case ultramodern::renderer::AspectRatio::Expand:
            _return(ctx, std::max(static_cast<float>(width) / height, original));
            return;
    }
}

extern "C" void recomp_get_cutscene_aspect_ratio(uint8_t *rdram, recomp_context *ctx) {
    float original = _arg<0, float>(rdram, ctx);
    int width, height;
    recompui::get_window_size(width, height);

    switch (dk64::get_cutscene_aspect_ratio_mode()) {
        case dk64::CutsceneAspectRatioMode::Original:
            _return(ctx, original);
            return;
        case dk64::CutsceneAspectRatioMode::Clamp16x9:
        default:
            _return(ctx, 16.0f / 9.0f);
            return;
        case dk64::CutsceneAspectRatioMode::Full:
            _return(ctx, std::max(static_cast<float>(width) / height, original));
            return;
    }
}

extern "C" void recomp_get_bgm_volume(uint8_t* rdram, recomp_context* ctx) {
    _return(ctx, dk64::get_bgm_volume() / 100.0f);
}

extern "C" void recomp_get_analog_cam_sensitivity(uint8_t* rdram, recomp_context* ctx) {
    _return<uint32_t>(ctx, dk64::get_analog_cam_sensitivity());
}


extern "C" void recomp_time_us(uint8_t* rdram, recomp_context* ctx) {
    _return(ctx, static_cast<u32>(std::chrono::duration_cast<std::chrono::microseconds>(ultramodern::time_since_start()).count()));
}

extern "C" void recomp_load_overlays(uint8_t * rdram, recomp_context * ctx) {
    u32 rom = _arg<0, u32>(rdram, ctx);
    PTR(void) ram = _arg<1, PTR(void)>(rdram, ctx);
    u32 size = _arg<2, u32>(rdram, ctx);

    load_overlays(rom, ram, size);
}

extern "C" void recomp_high_precision_fb_enabled(uint8_t * rdram, recomp_context * ctx) {
    _return(ctx, static_cast<s32>(recompui::renderer::RT64HighPrecisionFBEnabled()));
}

extern "C" void recomp_get_resolution_scale(uint8_t* rdram, recomp_context* ctx) {
    _return(ctx, ultramodern::get_resolution_scale());
}

extern "C" void recomp_get_inverted_axes(uint8_t* rdram, recomp_context* ctx) {
    s32* x_out = _arg<0, s32*>(rdram, ctx);
    s32* y_out = _arg<1, s32*>(rdram, ctx);

    dk64::CameraInvertMode mode = dk64::get_camera_invert_mode();

    *x_out = (mode == dk64::CameraInvertMode::InvertX || mode == dk64::CameraInvertMode::InvertBoth);
    *y_out = (mode == dk64::CameraInvertMode::InvertY || mode == dk64::CameraInvertMode::InvertBoth);
}

extern "C" void recomp_get_analog_inverted_axes(uint8_t* rdram, recomp_context* ctx) {
    s32* x_out = _arg<0, s32*>(rdram, ctx);
    s32* y_out = _arg<1, s32*>(rdram, ctx);

    dk64::CameraInvertMode mode = dk64::get_third_person_camera_mode();

    *x_out = (mode == dk64::CameraInvertMode::InvertX || mode == dk64::CameraInvertMode::InvertBoth);
    *y_out = (mode == dk64::CameraInvertMode::InvertY || mode == dk64::CameraInvertMode::InvertBoth);
}

extern "C" void recomp_get_flying_and_swimming_inverted_axes(uint8_t* rdram, recomp_context* ctx) {
    s32* x_out = _arg<0, s32*>(rdram, ctx);
    s32* y_out = _arg<1, s32*>(rdram, ctx);

    dk64::CameraInvertMode mode = dk64::get_flying_and_swimming_invert_mode();

    *x_out = (mode == dk64::CameraInvertMode::InvertX || mode == dk64::CameraInvertMode::InvertBoth);
    *y_out = (mode == dk64::CameraInvertMode::InvertY || mode == dk64::CameraInvertMode::InvertBoth);
}

extern "C" void recomp_get_first_person_inverted_axes(uint8_t* rdram, recomp_context* ctx) {
    s32* x_out = _arg<0, s32*>(rdram, ctx);
    s32* y_out = _arg<1, s32*>(rdram, ctx);

    dk64::CameraInvertMode mode = dk64::get_first_person_invert_mode();

    *x_out = (mode == dk64::CameraInvertMode::InvertX || mode == dk64::CameraInvertMode::InvertBoth);
    *y_out = (mode == dk64::CameraInvertMode::InvertY || mode == dk64::CameraInvertMode::InvertBoth);
}

extern "C" void recomp_get_analog_cam_enabled(uint8_t* rdram, recomp_context* ctx) {
    _return<s32>(ctx, dk64::get_analog_cam_mode() == dk64::AnalogCamMode::On);
}

extern "C" void recomp_get_note_saving_enabled(uint8_t* rdram, recomp_context* ctx) {
    _return<s32>(ctx, dk64::get_note_saving_mode() == dk64::NoteSavingMode::On);
}

extern "C" void recomp_get_right_analog_inputs(uint8_t* rdram, recomp_context* ctx) {
    float* x_out = _arg<0, float*>(rdram, ctx);
    float* y_out = _arg<1, float*>(rdram, ctx);

    // Don't return right analog inputs while game input is disabled.
    if (recompinput::game_input_disabled()) {
        *x_out = 0.0f;
        *y_out = 0.0f;
        return;
    }

    // TODO: Use controller number.
    recompinput::get_right_analog(0, x_out, y_out);
}

extern "C" void recomp_set_right_analog_suppressed(uint8_t* rdram, recomp_context* ctx) {
    s32 suppressed = _arg<0, s32>(rdram, ctx);

    recompinput::set_right_analog_suppressed(suppressed);
}

constexpr uint32_t k1_to_phys(uint32_t addr) {
    return addr & 0x1FFFFFFF;
}

extern "C" void osPiReadIo_recomp(RDRAM_ARG recomp_context * ctx) {
    uint32_t devAddr = recomp::rom_base | ctx->r4;
    gpr dramAddr = ctx->r5;
    uint32_t physical_addr = k1_to_phys(devAddr);

    if (physical_addr > recomp::rom_base) {
        // cart rom
        recomp::do_rom_pio(PASS_RDRAM dramAddr, physical_addr);
    } else {
        // sram
        assert(false && "SRAM ReadIo unimplemented");
    }

    ctx->r2 = 0;
}

extern "C" void osPfsInit_recomp(uint8_t * rdram, recomp_context* ctx) {
    ctx->r2 = 11; // PFS_ERR_DEVICE
}

// u32 rom_addr, void *ram_addr, u32 size
extern "C" void recomp_load_overlays_by_rom(uint8_t* rdram, recomp_context* ctx) {
    u32 rom_addr = _arg<0, u32>(rdram, ctx);
    PTR(void) ram_addr = _arg<1, PTR(void)>(rdram, ctx);
    u32 size = _arg<2, u32>(rdram, ctx);

    load_overlays(rom_addr, ram_addr, size);
}

extern "C" void recomp_abort(uint8_t* rdram, recomp_context* ctx) {
    std::string msg = _arg_string<0>(rdram, ctx);
    recompui::message_box(msg.c_str());
    assert(false);
    ultramodern::error_handling::quick_exit(__FILE__, __LINE__, __FUNCTION__);
}

extern "C" void recomp_xxh3(uint8_t* rdram, recomp_context* ctx) {
    PTR(void) data = _arg<0, PTR(void)>(rdram, ctx);
    u32 size = _arg<1, u32>(rdram, ctx);
    XXH3_state_t xxh3;
    XXH3_64bits_reset(&xxh3);

    // Hash 1 byte at a time to account for byteswapping.
    for (size_t i = 0; i < size; i++) {
        XXH3_64bits_update(&xxh3, TO_PTR(u8, data + i), 1);
    }

    uint64_t ret = XXH3_64bits_digest(&xxh3);
    
    ctx->r2 = (int32_t)(ret >> 32);
    ctx->r3 = (int32_t)(ret >> 0);
}

//map compressed rom address -> decompressed rom address, then load the overlay
extern "C" void load_dk64_overlay(uint32_t compressed_rom, int32_t ram_addr, uint32_t size) {
    uint32_t decompressed_rom = 0;
    switch (compressed_rom) {
        case 0x113F0: //global_asm
            decompressed_rom = 0x2000000;
            ram_addr = 0x805FB300;
            size = 0x165D50;
            break;
        case 0xCBE70: //menu
            decompressed_rom = 0x2165D50;
            ram_addr = 0x80024000;
            size = 0xFF10;
            break;
        case 0xD4B00: //multiplayer
            decompressed_rom = 0x2175C60;
            ram_addr = 0x80024000;
            size = 0x3100;
            break;
        case 0xD6B00: //minecart
            decompressed_rom = 0x2178D60;
            ram_addr = 0x80024000;
            size = 0x4E10;
            break;
        case 0xD9A40: //bonus
            decompressed_rom = 0x217DB70;
            ram_addr = 0x80024000;
            size = 0x9EF0;
            break;
        case 0xDF600: //race
            decompressed_rom = 0x2187A60;
            ram_addr = 0x80024000;
            size = 0xC160;
            break;
        case 0xE6780: //critter
            decompressed_rom = 0x2193BC0;
            ram_addr = 0x80024000;
            size = 0x61B0;
            break;
        case 0xEA0B0: //boss
            decompressed_rom = 0x2199D70;
            ram_addr = 0x80024000;
            size = 0x12DC0;
            break;
        case 0xF41A0: //arcade
            decompressed_rom = 0x21ACB30;
            ram_addr = 0x80024000;
            size = 0x26C00;
            break;
        case 0xFD2F0: //jetpac
            decompressed_rom = 0x21D3730;
            ram_addr = 0x80024000;
            size = 0xAC30;
            break;
    }
    if (decompressed_rom != 0) {
        load_overlays(decompressed_rom, ram_addr, size);
    }
}

extern "C" void boot_osPiRawStartDma(uint8_t* rdram, recomp_context* ctx) {
    uint32_t direction = ctx->r4;
    uint32_t device_address = ctx->r5;
    gpr rdram_address = ctx->r6;
    uint32_t size = ctx->r7;

    assert(direction == 0); // Only reads

    printf("boot_osPiRawStartDma: Rom %08X, Ram %08X, Size: %08X\n", device_address, rdram_address, size);

    // Complete the DMA synchronously (the game immediately waits until it's done anyways)
    recomp::do_rom_read(rdram, rdram_address, device_address + recomp::rom_base, size);
}

extern "C" void __f_to_ull_recomp(uint8_t * rdram, recomp_context * ctx) {
    uint64_t ret = (uint64_t)ctx->f12.fl;
    ctx->r2 = (int32_t)(ret >> 32);
    ctx->r3 = (int32_t)(ret >> 0);
}

extern "C" void __osSpSetStatus_recomp(RDRAM_ARG recomp_context * ctx) {
    //assert(false && "SRAM ReadIo unimplemented");
}

extern "C" void osViGetCurrentMode_recomp(uint8_t* rdram, recomp_context* ctx) {
    constexpr gpr os_vi_curr_ptr_addr = 0xFFFFFFFF80010190ull;
    const gpr vi_curr_ctx = (gpr)(int32_t)MEM_W(0, os_vi_curr_ptr_addr);

    if (vi_curr_ctx == 0) {
        ctx->r2 = 0;
        return;
    }

    const gpr modep = (gpr)(int32_t)MEM_W(0x8, vi_curr_ctx);
    if (modep == 0) {
        ctx->r2 = 0;
        return;
    }

    ctx->r2 = MEM_BU(0x3, modep);
}

extern "C" void dk64_apply_gameplay_widescreen(uint8_t* rdram, recomp_context* ctx) {
    (void)rdram;
    (void)ctx;

    // constexpr gpr scissor_min_x_addr = 0xFFFFFFFF80744498ull;
    constexpr gpr scissor_max_x_addr = 0xFFFFFFFF807444A0ull;
    constexpr gpr screen_height_addr = 0xFFFFFFFF80744494ull;
    const int32_t current_height = (int32_t)MEM_H(0, screen_height_addr);
    const float original_aspect = 4.0f / 3.0f;

    int window_width = 0;
    int window_height = 0;
    recompui::get_window_size(window_width, window_height);

    ultramodern::renderer::GraphicsConfig graphics_config = ultramodern::renderer::get_graphics_config();
    float target_aspect = original_aspect;
    if (graphics_config.ar_option == ultramodern::renderer::AspectRatio::Expand) {
        const float window_aspect = static_cast<float>(window_width) / static_cast<float>(window_height);
        target_aspect = std::max(window_aspect, original_aspect);
    }
    int32_t target_width = (int32_t)std::lroundf(static_cast<float>(current_height) * target_aspect);
    MEM_H(0, scissor_max_x_addr) = (int16_t)(target_width - 1);
}