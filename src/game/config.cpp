#include "donk_config.h"
#include "recompui/recompui.h"
#include "recompui/config.h"
#include "recompinput/recompinput.h"
#include "donk_sound.h"
#include "donk_draw.h"
#include "banjo_support.h"
#include "ultramodern/config.hpp"
#include "librecomp/files.hpp"
#include "librecomp/config.hpp"
#include "util/file.h"
#include <filesystem>
#include <fstream>
#include <iomanip>

#if defined(_WIN32)
#include <Shlobj.h>
#elif defined(__linux__)
#include <unistd.h>
#include <pwd.h>
#elif defined(__APPLE__)
#include "apple/rt64_apple.h"
#endif

static void add_general_options(recomp::config::Config &config) {
    using EnumOptionVector = const std::vector<recomp::config::ConfigOptionEnumOption>;

    static EnumOptionVector camera_invert_mode_options = {
        {dk64::CameraInvertMode::InvertNone, "InvertNone", "None"},
        {dk64::CameraInvertMode::InvertX, "InvertX", "Invert X"},
    };
    config.add_enum_option(
        dk64::configkeys::general::third_person_camera_invert_mode,
        "Invert Camera",
        "Inverts the camera controls for the third person camera if it's enabled.",
        camera_invert_mode_options,
        dk64::CameraInvertMode::InvertNone
    );
    static EnumOptionVector first_person_invert_mode_options = {
        {dk64::CameraInvertMode::InvertNone, "InvertNone", "None"},
        {dk64::CameraInvertMode::InvertX, "InvertX", "Invert X"},
        {dk64::CameraInvertMode::InvertY, "InvertY", "Invert Y"},
        {dk64::CameraInvertMode::InvertBoth, "InvertBoth", "Invert Both"}
    };
    config.add_enum_option(
        dk64::configkeys::general::first_person_invert_mode,
        "Invert First Person View",
        "Inverts the camera controls in first person view. <recomp-color primary>Invert Y</recomp-color> is the default and matches the original game.",
        first_person_invert_mode_options,
        dk64::CameraInvertMode::InvertY
    );
    static EnumOptionVector swimming_invert_options = {
        {dk64::CameraInvertMode::InvertNone, "InvertNone", "None"},
        {dk64::CameraInvertMode::InvertX, "InvertX", "Invert X"},
        {dk64::CameraInvertMode::InvertY, "InvertY", "Invert Y"},
        {dk64::CameraInvertMode::InvertBoth, "InvertBoth", "Invert Both"}
    };
    config.add_enum_option(
        dk64::configkeys::general::swimming_invert_mode,
        "Invert Swimming",
        "Inverts the controls for swimming. <recomp-color primary>Invert Y</recomp-color> is the default and matches the original game.",
        swimming_invert_options,
        dk64::CameraInvertMode::InvertY
    );
    // Story Skip
    static EnumOptionVector story_skip_options = {
        {dk64::StorySkipMode::Off, "Off", "None"},
        {dk64::StorySkipMode::IntroStory, "IntroStory", "Intro Story Only"},
        {dk64::StorySkipMode::VanillaOn, "VanillaOn", "On"}
    };
    config.add_enum_option(
        dk64::configkeys::general::story_skip,
        "Story Skip",
        "Skips some story cutscenes.<br /><recomp-color primary>Off</recomp-color>: No story cutscenes are skipped.<br /><recomp-color primary>Intro Story Only</recomp-color>: Only the 5-minute introductory story is skipped. The K. Rool level intros will still play.<br /><recomp-color primary>On</recomp-color>: Both the introductory story and all K. Rool level intros are skipped.",
        story_skip_options,
        dk64::StorySkipMode::Off
    );
    // Camera Type
    static EnumOptionVector camera_type_options = {
        {dk64::CameraTypeMode::Free, "Free", "Free Cam"},
        {dk64::CameraTypeMode::Follow, "Follow", "Follow Cam"},
        {dk64::CameraTypeMode::BetterFree, "BetterFree", "Better Free Cam"},
        {dk64::CameraTypeMode::Analog, "Analog", "Analog Camera"}
    };
    config.add_enum_option(
        dk64::configkeys::general::camera_type,
        "Camera Type",
        "Changes the camera behavior.<br /><recomp-color primary>Free Cam</recomp-color>: Camera can be controlled via pressing C-Left and C-Right. The camera does not try to push itself behind the player.<br /><recomp-color primary>Follow Cam</recomp-color>: Similar to <recomp-color secondary>Free Cam</recomp-color>, but the camera tries to push itself behind the player.<br /><recomp-color primary>Better Free Cam</recomp-color>: Similar to <recomp-color secondary>Free Cam</recomp-color>, but instead of a button press turning the camera 45 degrees, holding the button moves the camera at 5 degrees per frame.",
        camera_type_options,
        dk64::CameraTypeMode::Free
    );
    config.add_number_option(
        dk64::configkeys::general::analog_camera_sensitivity,
        "Analog Camera Sensitivity",
        "Sets the sensitivity of the right stick analog camera, if enabled.",
        1, 10, 1, 0, false, 3
    );
    config.add_option_hidden_dependency(
        dk64::configkeys::general::analog_camera_sensitivity,
        dk64::configkeys::general::camera_type,
        dk64::CameraTypeMode::Free,
        dk64::CameraTypeMode::Follow,
        dk64::CameraTypeMode::BetterFree
    );
    // Lightning Flashes
    static EnumOptionVector lightning_flash_options = {
        {dk64::LightningFlashMode::Off, "Off", "Off"},
        {dk64::LightningFlashMode::Reduced, "Reduced", "Reduced"},
        {dk64::LightningFlashMode::Vanilla, "Vanilla", "Vanilla"}
    };
    config.add_enum_option(
        dk64::configkeys::general::lightning_flashes,
        "Lightning Flash Intensity",
        "Changes the intensity of lightning flashes within the game.<br /><recomp-color primary>Vanilla</recomp-color>: Lightning flashes at 100% intensity<br /><recomp-color primary>Reduced</recomp-color>: Lightning flashes at 60% intensity<br /><recomp-color primary>Off</recomp-color>: Lightning flashes are completely disabled",
        lightning_flash_options,
        dk64::LightningFlashMode::Reduced
    );
    
}

template <typename T = uint32_t>
T get_general_config_enum_value(const std::string& option_id) {
    return static_cast<T>(std::get<uint32_t>(recompui::config::get_general_config().get_option_value(option_id)));
}

template <typename T = uint32_t>
T get_general_config_number_value(const std::string& option_id) {
    return static_cast<T>(std::get<double>(recompui::config::get_general_config().get_option_value(option_id)));
}

dk64::CameraInvertMode dk64::get_camera_invert_mode() {
    return get_general_config_enum_value<dk64::CameraInvertMode>(dk64::configkeys::general::camera_invert_mode);
}

dk64::CameraInvertMode dk64::get_third_person_camera_mode() {
    return get_general_config_enum_value<dk64::CameraInvertMode>(dk64::configkeys::general::third_person_camera_invert_mode);
}

dk64::CameraInvertMode dk64::get_swimming_invert_mode() {
    return get_general_config_enum_value<dk64::CameraInvertMode>(dk64::configkeys::general::swimming_invert_mode);
}

dk64::StorySkipMode dk64::get_story_skip() {
    return get_general_config_enum_value<dk64::StorySkipMode>(dk64::configkeys::general::story_skip);
}

dk64::CameraTypeMode dk64::get_camera_type() {
    return get_general_config_enum_value<dk64::CameraTypeMode>(dk64::configkeys::general::camera_type);
}

dk64::LightningFlashMode dk64::get_lightning_flash() {
    return get_general_config_enum_value<dk64::LightningFlashMode>(dk64::configkeys::general::lightning_flashes);
}

dk64::CameraInvertMode dk64::get_first_person_invert_mode() {
    return get_general_config_enum_value<dk64::CameraInvertMode>(dk64::configkeys::general::first_person_invert_mode);
}

uint32_t dk64::get_analog_cam_sensitivity() {
    return get_general_config_number_value(dk64::configkeys::general::analog_camera_sensitivity);
}

template <typename T = uint32_t>
T get_graphics_config_enum_value(const std::string& option_id) {
    return static_cast<T>(std::get<uint32_t>(recompui::config::get_graphics_config().get_option_value(option_id)));
}

template <typename T = uint32_t>
T get_graphics_config_number_value(const std::string& option_id) {
    return static_cast<T>(std::get<double>(recompui::config::get_graphics_config().get_option_value(option_id)));
}

template <typename T = uint32_t>
T get_technical_config_enum_value(const std::string& option_id) {
    return static_cast<T>(std::get<uint32_t>(recompui::config::get_config("technical").get_option_value(option_id)));
}

static void add_sound_options(recomp::config::Config &config) {
    config.add_percent_number_option(
        dk64::configkeys::sound::bgm_volume,
        "Background Music Volume",
        "Controls the overall volume of background music.",
        100.0f
    );
    config.add_percent_number_option(
        dk64::configkeys::sound::sfx_volume,
        "SFX Volume",
        "Controls the overall volume of sound effects.",
        100.0f
    );
}
template <typename T = uint32_t>
T get_sound_config_number_value(const std::string& option_id) {
    return static_cast<T>(std::get<double>(recompui::config::get_sound_config().get_option_value(option_id)));
}

int dk64::get_bgm_volume() {
    return get_sound_config_number_value<int>(dk64::configkeys::sound::bgm_volume);
}

int dk64::get_sfx_volume() {
    return get_sound_config_number_value<int>(dk64::configkeys::sound::sfx_volume);
}

static void add_graphics_options(recomp::config::Config &config) {
    using EnumOptionVector = const std::vector<recomp::config::ConfigOptionEnumOption>;
    // Cutscene borders
    static EnumOptionVector cutscene_border_options = {
        {dk64::CutsceneBordersMode::Off, "Off", "Off"},
        {dk64::CutsceneBordersMode::On, "On", "On"}
    };
    config.add_enum_option(
        dk64::configkeys::graphics::cutscene_borders,
        "Cutscene Borders",
        "If turned on, cutscenes will show borders on the top and the bottom of the screen as in the vanilla game.",
        cutscene_border_options,
        dk64::CutsceneBordersMode::On
    );
    config.add_percent_number_option(
        dk64::configkeys::graphics::draw_distance,
        "Draw Distance",
        "Controls the overall volume of background music.",
        0.0f
    );
}

dk64::CutsceneBordersMode dk64::get_cutscene_borders() {
    return get_graphics_config_enum_value<dk64::CutsceneBordersMode>(dk64::configkeys::graphics::cutscene_borders);
}

int dk64::get_draw_distance() {
    return get_graphics_config_number_value<int>(dk64::configkeys::graphics::draw_distance);
}

static void add_technical_options(recomp::config::Config &config) {
    using EnumOptionVector = const std::vector<recomp::config::ConfigOptionEnumOption>;
    // Multiplayer
    static EnumOptionVector multiplayer_options = {
        {dk64::MultiplayerEnabled::Off, "Off", "Off"},
        {dk64::MultiplayerEnabled::On, "On", "On"}
    };
    config.add_enum_option(
        dk64::configkeys::technical::multiplayer_enabled,
        "Enable Multiplayer",
        "Enables the ability to enter Multiplayer. Multiplayer is considered a <recomp-color secondary>Work-in-Progress feature</recomp-color> that is classed as non-functional.<br />You should not enable this feature unless you wish to use glitches such as \"Funky Weapons Glitch\" or \"Main Menu Moves\"",
        multiplayer_options,
        dk64::MultiplayerEnabled::Off
    );
}

dk64::MultiplayerEnabled dk64::get_multiplayer_enabled() {
    return get_technical_config_enum_value<dk64::MultiplayerEnabled>(dk64::configkeys::technical::multiplayer_enabled);
}

static void set_control_defaults() {
    using namespace recompinput;

    // Left shoulder -> C Down | Backwards eggs / zoom out
    set_default_mapping_for_controller(
        GameInput::C_DOWN,
        { 
            InputField::controller_analog(SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_RIGHTY, true),
            InputField::controller_digital(SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_LEFTSHOULDER)
        }
    );

    // Right shoulder -> C Up | Forwards eggs / first person
    set_default_mapping_for_controller(
        GameInput::C_UP,
        { 
            InputField::controller_analog(SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_RIGHTY, false),
            InputField::controller_digital(SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_RIGHTSHOULDER)
        }
    );

    // North button -> C Left | Talon trot / camera left
    set_default_mapping_for_controller(
        GameInput::C_LEFT,
        { 
            InputField::controller_analog(SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_RIGHTX, false),
            InputField::controller_digital(SDL_CONTROLLER_BUTTON_NORTH)
        }
    );

    // East button -> C Right | Wonderwing / camera right
    set_default_mapping_for_controller(
        GameInput::C_RIGHT,
        { 
            InputField::controller_analog(SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_RIGHTX, true),
            InputField::controller_digital(SDL_CONTROLLER_BUTTON_EAST)
        }
    );

    // R3 -> L | Unused in BK but can be used in mods
    set_default_mapping_for_controller(GameInput::L, { InputField::controller_digital(SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_RIGHTSTICK) });
}

static void set_control_descriptions() {
    recompinput::set_game_input_description(recompinput::GameInput::Y_AXIS_POS, "Used to move and for steering while swimming. Axis inversion for swimming can be configured in the General tab.");
    recompinput::set_game_input_description(recompinput::GameInput::Y_AXIS_NEG, "Used to move and for steering while swimming. Axis inversion for swimming can be configured in the General tab.");
    recompinput::set_game_input_description(recompinput::GameInput::X_AXIS_NEG, "Used to move and for steering while swimming. Axis inversion for swimming can be configured in the General tab.");
    recompinput::set_game_input_description(recompinput::GameInput::X_AXIS_POS, "Used to move and for steering while swimming. Axis inversion for swimming can be configured in the General tab.");
    recompinput::set_game_input_description(recompinput::GameInput::A, "Used to jump and select options in menus.");
    recompinput::set_game_input_description(recompinput::GameInput::B, "Used for attacks, which change depending on whether you are stationary, moving, in the air, or crouching.");
    recompinput::set_game_input_description(recompinput::GameInput::Z, "Used to crouch, which enables A, B and the C-Buttons to perform different actions.");
    recompinput::set_game_input_description(recompinput::GameInput::L, "Unused. Mods may use it for additional features.");
    recompinput::set_game_input_description(recompinput::GameInput::R, "Used to center the camera behind the player, and to perform tighter turns while in the coconut boat.");
    recompinput::set_game_input_description(recompinput::GameInput::START, "Used for pausing and for skipping certain cutscenes.");
    recompinput::set_game_input_description(recompinput::GameInput::C_UP, "Used to enter first-person mode and to play your instrument while holding Z");
    recompinput::set_game_input_description(recompinput::GameInput::C_DOWN, "Used to toggle between the different camera zoom levels, and to pull out the fairy camera while holding Z.");
    recompinput::set_game_input_description(recompinput::GameInput::C_LEFT, "Used to rotate the camera sideways. Axis inversion can be configured in the General tab. Also used to pull out your fruit weapon while holding Z.");
    recompinput::set_game_input_description(recompinput::GameInput::C_RIGHT, "Used to rotate the camera sideways. Axis inversion can be configured in the General tab). Also used to throw an orange while holding Z.");
    recompinput::set_game_input_description(recompinput::GameInput::DPAD_UP, "Unused. Mods may use it for additional features.");
    recompinput::set_game_input_description(recompinput::GameInput::DPAD_DOWN, "Unused. Mods may use it for additional features.");
    recompinput::set_game_input_description(recompinput::GameInput::DPAD_LEFT, "Unused. Mods may use it for additional features.");
    recompinput::set_game_input_description(recompinput::GameInput::DPAD_RIGHT, "Unused. Mods may use it for additional features.");
}

void dk64::init_config() {
    std::filesystem::path recomp_dir = recompui::file::get_app_folder_path();

    if (!recomp_dir.empty()) {
        std::filesystem::create_directories(recomp_dir);
    }

    recompui::config::GeneralTabOptions general_options{};
    general_options.has_rumble_strength = true;
    general_options.has_gyro_sensitivity = true;
    general_options.has_mouse_sensitivity = true;

    auto &general_config = recompui::config::create_general_tab(general_options);
    add_general_options(general_config);

    auto &graphics_config = recompui::config::create_graphics_tab();
    add_graphics_options(graphics_config);

    set_control_defaults();
    set_control_descriptions();
    recompui::config::create_controls_tab();

    auto &sound_config = recompui::config::create_sound_tab();
    add_sound_options(sound_config);

    auto &technical_config = recompui::config::create_config_tab("Technical", "technical", true);
    add_technical_options(technical_config);

    recompui::config::create_mods_tab();

    recompui::config::finalize();

}
