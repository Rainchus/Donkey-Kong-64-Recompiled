#ifndef __DONK_CONFIG_H__
#define __DONK_CONFIG_H__

#include <filesystem>
#include <string>
#include <string_view>

#include "json/json.hpp"

namespace dk64 {
    inline const std::u8string program_id = u8"DK64Recompiled";
    inline const std::string program_name = "DK64: Recompiled";

    namespace configkeys {
        namespace general {
            inline const std::string camera_invert_mode = "camera_invert_mode";
            inline const std::string analog_cam_mode = "analog_cam_mode";
            inline const std::string third_person_camera_invert_mode = "third_person_camera_invert_mode";
            inline const std::string swimming_invert_mode = "swimming_invert_mode";
            inline const std::string first_person_invert_mode = "first_person_invert_mode";
            inline const std::string analog_camera_sensitivity = "analog_camera_sensitivity";
            inline const std::string story_skip = "story_skip";
            inline const std::string camera_type = "camera_type";
            inline const std::string lightning_flashes = "lightning_flashes";
        }

        namespace sound {
            inline const std::string bgm_volume = "bgm_volume";
            inline const std::string sfx_volume = "sfx_volume";
        }

        namespace graphics {
            inline const std::string cutscene_borders = "cutscene_borders";
            inline const std::string draw_distance = "draw_distance";
        }

        namespace technical {
            inline const std::string multiplayer_enabled = "multiplayer_enabled";
        }
    }

    // TODO: Move loading configs to the runtime once we have a way to allow per-project customization.
    void init_config();

    enum class CameraInvertMode {
        InvertNone,
        InvertX,
        InvertY,
        InvertBoth
    };

    CameraInvertMode get_camera_invert_mode();

    CameraInvertMode get_third_person_camera_mode();

    CameraInvertMode get_swimming_invert_mode();

    CameraInvertMode get_first_person_invert_mode();

    uint32_t get_analog_cam_sensitivity();

    enum class StorySkipMode {
        Off,
        IntroStory,
        VanillaOn
    };

    StorySkipMode get_story_skip();

    enum class CameraTypeMode {
        Free,
        Follow,
        BetterFree,
        Mouse
    };

    CameraTypeMode get_camera_type();

    enum class LightningFlashMode {
        Vanilla,
        Reduced,
        Off
    };
    LightningFlashMode get_lightning_flash();

    enum class CutsceneBordersMode {
        On,
        Off
    };
    CutsceneBordersMode get_cutscene_borders();

    enum class MultiplayerEnabled {
        Off,
        On
    };
    MultiplayerEnabled get_multiplayer_enabled();

    void open_quit_game_prompt();
};

#endif
