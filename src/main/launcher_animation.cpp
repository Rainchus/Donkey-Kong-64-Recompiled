#include "donk_launcher.h"
#include <atomic>
#include <cstdio> 

struct KeyframeRot {
    float seconds;
    float deg;
};

struct Keyframe2D {
    float seconds;
    float x;
    float y;
};

enum class InterpolationMethod {
    Linear,
    Smootherstep
};

struct AnimationData {
    uint32_t keyframe_index = 0;
    uint32_t loop_keyframe_index = UINT32_MAX;
    float seconds = 0.0f;
    InterpolationMethod interpolation_method = InterpolationMethod::Linear;
};

struct AnimatedSvg {
    recompui::Element *svg = nullptr;
    std::vector<Keyframe2D> position_keyframes;
    std::vector<Keyframe2D> scale_keyframes;
    std::vector<KeyframeRot> rotation_keyframes;
    std::vector<KeyframeRot> opacity_keyframes;
    AnimationData position_animation;
    AnimationData scale_animation;
    AnimationData rotation_animation;
    AnimationData opacity_animation;
    float width = 0;
    float height = 0;
};

struct LauncherContext {
    AnimatedSvg isles_svg;
    AnimatedSvg logo_d_svg;
    AnimatedSvg logo_k_svg;
    AnimatedSvg logo_64_svg;
    AnimatedSvg logo_recomp_svg;
    AnimatedSvg bg_svg;
    std::array<AnimatedSvg, 3> cloud_svgs;
    recompui::Element *wrapper;
    recompui::Element *water;
    recompui::Label *credits_label;
    float wrapper_phase = -1.0f;
    std::chrono::steady_clock::time_point last_update_time;
    float seconds = 0.0f;
    bool started = false;
    bool options_enabled = false;
    bool animation_skipped = false;
    std::atomic<bool> skip_animation_next_update = false;
} launcher_context;

float interpolate_value(float a, float b, float t, InterpolationMethod method) {
    switch (method) {
    case InterpolationMethod::Smootherstep:
        return a + (b - a) * (t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f));
    case InterpolationMethod::Linear:
    default:
        return a + (b - a) * t;
    }
}

void calculate_rot_from_keyframes(const std::vector<KeyframeRot> &kf, AnimationData &an, float delta_time, float &deg) {
    if (kf.empty()) {
        return;
    }

    an.seconds += delta_time;

    while ((an.keyframe_index < (kf.size() - 1) && (an.seconds >= kf[an.keyframe_index + 1].seconds))) {
        an.keyframe_index++;
    }

    if (an.keyframe_index >= (kf.size() - 1)) {
        deg = kf[an.keyframe_index].deg;
    }
    else {
        float t = (an.seconds - kf[an.keyframe_index].seconds) / (kf[an.keyframe_index + 1].seconds - kf[an.keyframe_index].seconds);
        deg = interpolate_value(kf[an.keyframe_index].deg, kf[an.keyframe_index + 1].deg, t, an.interpolation_method);
    }
}

void calculate_2d_from_keyframes(const std::vector<Keyframe2D> &kf, AnimationData &an, float delta_time, float &x, float &y) {
    if (kf.empty()) {
        return;
    }

    an.seconds += delta_time;

    while ((an.keyframe_index < (kf.size() - 1) && (an.seconds >= kf[an.keyframe_index + 1].seconds))) {
        an.keyframe_index++;
    }

    if ((an.loop_keyframe_index != UINT32_MAX) && (an.keyframe_index >= (kf.size() - 1))) {
        an.seconds = kf[an.loop_keyframe_index].seconds + (an.seconds - kf[an.keyframe_index].seconds);
        an.keyframe_index = an.loop_keyframe_index;
    }

    if (an.keyframe_index >= (kf.size() - 1)) {
        x = kf[an.keyframe_index].x;
        y = kf[an.keyframe_index].y;
    }
    else {
        float t = (an.seconds - kf[an.keyframe_index].seconds) / (kf[an.keyframe_index + 1].seconds - kf[an.keyframe_index].seconds);
        x = interpolate_value(kf[an.keyframe_index].x, kf[an.keyframe_index + 1].x, t, an.interpolation_method);
        y = interpolate_value(kf[an.keyframe_index].y, kf[an.keyframe_index + 1].y, t, an.interpolation_method);
    }
}

AnimatedSvg create_animated_svg(recompui::ContextId context, recompui::Element *parent, const std::string &svg_path, float width, float height) {
    AnimatedSvg animated_svg;
    animated_svg.width = width;
    animated_svg.height = height;
    animated_svg.svg = context.create_element<recompui::Svg>(parent, svg_path);
    animated_svg.svg->set_position(recompui::Position::Absolute);
    animated_svg.svg->set_width(width, recompui::Unit::Dp);
    animated_svg.svg->set_height(height, recompui::Unit::Dp);
    return animated_svg;
}

void update_animated_svg(AnimatedSvg &animated_svg, float delta_time, float bg_width, float bg_height) {
    float position_x = 0.0f, position_y = 0.0f;
    float scale_x = 1.0f, scale_y = 1.0f;
    float rotation_degrees = 0.0f;
    float opacity = 1.0f;
    calculate_2d_from_keyframes(animated_svg.position_keyframes, animated_svg.position_animation, delta_time, position_x, position_y);
    calculate_2d_from_keyframes(animated_svg.scale_keyframes, animated_svg.scale_animation, delta_time, scale_x, scale_y);
    calculate_rot_from_keyframes(animated_svg.rotation_keyframes, animated_svg.rotation_animation, delta_time, rotation_degrees);
    calculate_rot_from_keyframes(animated_svg.opacity_keyframes, animated_svg.opacity_animation, delta_time, opacity);
    animated_svg.svg->set_translate_2D(position_x + bg_width / 2.0f - animated_svg.width / 2.0f, position_y + bg_height / 2.0f - animated_svg.height / 2.0f);
    animated_svg.svg->set_scale_2D(scale_x, scale_y);
    animated_svg.svg->set_rotation(rotation_degrees);
    animated_svg.svg->set_opacity(opacity);
}

bool check_skip_input(SDL_Event* event) {
    switch (event->type) {
    case SDL_KEYDOWN:
        return event->key.keysym.scancode == SDL_SCANCODE_ESCAPE ||
            event->key.keysym.scancode == SDL_SCANCODE_SPACE ||
            (event->key.keysym.scancode == SDL_SCANCODE_RETURN && (event->key.keysym.mod & (KMOD_LALT | KMOD_RALT)) == KMOD_NONE);
    case SDL_CONTROLLERBUTTONDOWN:
    case SDL_MOUSEBUTTONDOWN:
        return true;
    default:
        return false;
    }
}

int launcher_event_watch(void* userdata, SDL_Event* event) {
    if (!launcher_context.animation_skipped && check_skip_input(event)) {
        launcher_context.animation_skipped = true;
        launcher_context.skip_animation_next_update = true;
        return 0;
    }
    else {
        return 1;
    }
}

const float jiggy_scale_anim_start = 0.0f;
const float jiggy_scale_anim_length = 0.75f;
const float jiggy_scale_anim_end = jiggy_scale_anim_start + jiggy_scale_anim_length;
const float jiggy_move_over_start = jiggy_scale_anim_end + 0.5f;
const float jiggy_move_over_length = 0.75f;
const float jiggy_move_over_end = jiggy_move_over_start + jiggy_move_over_length;

const float animation_skip_time = 10.0f;

void dk64::launcher_animation_setup(recompui::LauncherMenu *menu) {
    auto context = recompui::get_current_context();
    recompui::Element *background_container = menu->get_background_container();
    background_container->set_background_color({ 0xFF, 0x90, 0x01, 0xFF });
    // background_container->set_property("decorator", "linear-gradient(to bottom, #FF4807FF, #FF9001FF)");

    launcher_context.wrapper = context.create_element<recompui::Element>(background_container, 0);
    launcher_context.wrapper->set_position(recompui::Position::Absolute);
    launcher_context.wrapper->set_width(100, recompui::Unit::Percent);
    launcher_context.wrapper->set_height(100, recompui::Unit::Percent);
    launcher_context.wrapper->set_top(0);

    // Disable and hide the options.
    for (auto option : menu->get_game_options_menu()->get_options()) {
        option->set_font_family("Suplexmentary Comic NC");
        option->set_enabled(false);
        option->set_opacity(0.0f);
        option->set_padding(24.0f);
        auto label = option->get_label();
        label->set_font_size(56.0f);
        label->set_letter_spacing(4.0f);
    }

    // The creation order of these is important.
    
    launcher_context.bg_svg = create_animated_svg(context, launcher_context.wrapper, "bg.svg", 1920.0f, 1920.0f);
    launcher_context.cloud_svgs[0] = create_animated_svg(context, background_container, "Cloud1.svg", 461.0f, 154.0f);
    launcher_context.cloud_svgs[1] = create_animated_svg(context, background_container, "Cloud3.svg", 295.0f, 167.0f);
    launcher_context.cloud_svgs[2] = create_animated_svg(context, background_container, "Cloud1.svg", 461.0f, 154.0f);
    launcher_context.isles_svg = create_animated_svg(context, launcher_context.wrapper, "DkIsles.svg", 1920.0f, 1920.0f);

    launcher_context.logo_64_svg = create_animated_svg(context, background_container, "logo_64.svg", 336, 283);
    launcher_context.logo_k_svg = create_animated_svg(context, background_container, "logo_k.svg", 267, 304);
    launcher_context.logo_d_svg = create_animated_svg(context, background_container, "logo_d.svg", 292, 306);
    launcher_context.logo_recomp_svg = create_animated_svg(context, background_container, "logo_subtitle.svg", 1280, 747);
    
    // Animate the clouds.
    const float cloud_scale_duration = 0.3f;
    launcher_context.cloud_svgs[0].position_keyframes = {
        { 0.0f, 600.0f, -445.0f },
        { 3.0f, 600.0f, -455.0f },
        { 6.0f, 600.0f, -445.0f },
    };

    launcher_context.cloud_svgs[0].scale_keyframes = {
        { 0.0f, 0.0f, 0.0f },
        { 2.0f, 0.0f, 0.0f },
        { 2.0f + cloud_scale_duration, 1.0f, 1.0f },
    };
    launcher_context.cloud_svgs[1].position_keyframes = {
        { 0.0f, -600.0f, -295.0f },
        { 2.0f, -600.0f, -305.0f },
        { 4.0f, -600.0f, -295.0f },
    };

    launcher_context.cloud_svgs[1].scale_keyframes = {
        { 0.0f, 0.0f, 0.0f },
        { 2.4f, 0.0f, 0.0f },
        { 2.4f + cloud_scale_duration, 1.0f, 1.0f },
    };

    launcher_context.cloud_svgs[2].position_keyframes = {
        { 0.0f, 470.0f, -200.0f },
        { 4.0f, 470.0f, -190.0f },
        { 8.0f, 470.0f, -200.0f },
    };

    launcher_context.cloud_svgs[2].scale_keyframes = {
        { 0.0f, 0.0f, 0.0f },
        { 2.6f, 0.0f, 0.0f },
        { 2.6f + cloud_scale_duration, 0.5f, 0.5f },
    };

    for (size_t i = 0; i < launcher_context.cloud_svgs.size(); i++) {
        launcher_context.cloud_svgs[i].position_animation.loop_keyframe_index = 0;
        launcher_context.cloud_svgs[i].position_animation.interpolation_method = InterpolationMethod::Smootherstep;
    }
    
    // Animate DK Isles
    launcher_context.isles_svg.position_keyframes = {
        { 0.0f, -450.0f, 125.0f },
        { 1.0f, -450.0f, 125.0f },
        { 2.0f, -450.0f, 125.0f },
    };

    launcher_context.isles_svg.scale_keyframes = {
        { 0.0f, 0.0f, 0.0f },
        { 0.1f, 0.5f, 0.5f },
    };

    launcher_context.isles_svg.rotation_keyframes = {
        { 0.0f, 0.0f },
    };

    launcher_context.isles_svg.position_animation.loop_keyframe_index = 2;
    launcher_context.isles_svg.position_animation.interpolation_method = InterpolationMethod::Smootherstep;
    launcher_context.isles_svg.rotation_animation.interpolation_method = InterpolationMethod::Smootherstep;

    const float pop_to_top_duration = 0.2f;
    const float pop_to_end_duration = 0.1f;
    const float pop_total_duration = pop_to_top_duration + pop_to_end_duration;
    const float pop_top_scale = 0.9f;
    const float pop_end_scale = 0.75f;
    const float pop_y = -405.0f;
    const float pop_d_x = -160.0f;
    const float pop_k_x = -10.0f;
    const float pop_64_x = 160.0f;
    // Animate the logo.
    // D
    launcher_context.logo_d_svg.position_keyframes = {
        { 0.0f, pop_d_x, pop_y },
        { 1.0f, pop_d_x, pop_y },
    };
    launcher_context.logo_d_svg.scale_keyframes = {
        { (0 * pop_total_duration) + 0.0f, 0.0f, 0.0f },
        { (0 * pop_total_duration) + pop_to_top_duration, pop_top_scale, pop_top_scale },
        { (0 * pop_total_duration) + pop_to_top_duration + pop_to_end_duration, pop_end_scale, pop_end_scale },
    };
    // K
    launcher_context.logo_k_svg.position_keyframes = {
        { 0.0f, pop_k_x, pop_y },
        { 1.0f, pop_k_x, pop_y },
    };
    launcher_context.logo_k_svg.scale_keyframes = {
        { 0.0f, 0.0f, 0.0f },
        { (1 * pop_total_duration) + 0.0f, 0.0f, 0.0f },
        { (1 * pop_total_duration) + pop_to_top_duration, pop_top_scale, pop_top_scale },
        { (1 * pop_total_duration) + pop_to_top_duration + pop_to_end_duration, pop_end_scale, pop_end_scale },
    };
    // 64
    launcher_context.logo_64_svg.position_keyframes = {
        { 0.0f, pop_64_x, pop_y },
        { 1.0f, pop_64_x, pop_y },
    };
    launcher_context.logo_64_svg.scale_keyframes = {
        { 0.0f, 0.0f, 0.0f },
        { (2 * pop_total_duration) + 0.0f, 0.0f, 0.0f },
        { (2 * pop_total_duration) + pop_to_top_duration, pop_top_scale, pop_top_scale },
        { (2 * pop_total_duration) + pop_to_top_duration + pop_to_end_duration, pop_end_scale, pop_end_scale },
    };
    // Subtitle
    launcher_context.logo_recomp_svg.position_keyframes = {
        { 0.0f, 0.0f, -265.0f },
        { 1.0f, 0.0f, -265.0f },
        { 2.0f, 0.0f, -365.0f },
    };
    launcher_context.logo_recomp_svg.scale_keyframes = {
        { 0.0f, 0.5f, 0.5f },
        { 2.0f, 0.5f, 0.5f },
    };
    launcher_context.logo_recomp_svg.opacity_keyframes = {
        { 0.0f, 0.0f },
        { 1.0f, 0.0f },
        { 2.0f, 1.0f },
    };
    launcher_context.logo_d_svg.position_animation.interpolation_method = InterpolationMethod::Smootherstep;
    launcher_context.logo_k_svg.position_animation.interpolation_method = InterpolationMethod::Smootherstep;
    launcher_context.logo_64_svg.position_animation.interpolation_method = InterpolationMethod::Smootherstep;
    launcher_context.logo_recomp_svg.position_animation.interpolation_method = InterpolationMethod::Smootherstep;
    launcher_context.logo_recomp_svg.opacity_animation.interpolation_method = InterpolationMethod::Smootherstep;


    launcher_context.water = context.create_element<recompui::Element>(background_container, 0);
    launcher_context.water->set_position(recompui::Position::Absolute);
    launcher_context.water->set_width(100, recompui::Unit::Percent);
    launcher_context.water->set_height(100, recompui::Unit::Percent);
    launcher_context.water->set_background_color({ 0x00, 0x66, 0xCC, 0x7F});

    launcher_context.credits_label = context.create_element<recompui::Label>(
        background_container, 
        "DK64 Rekongpiled by Rainchus, Ballaam, KillKlli, GreenBean and UmedMuzl", 
        recompui::LabelStyle::Small
    );
    launcher_context.credits_label->set_position(recompui::Position::Absolute);
    launcher_context.credits_label->set_bottom(4.0f, recompui::Unit::Dp);
    launcher_context.credits_label->set_right(4.0f, recompui::Unit::Dp);
    launcher_context.credits_label->set_font_family("Suplexmentary Comic NC");

    // Install an event watch to skip the launcher animation if a keyboard, mouse or controller input is detected.
    SDL_AddEventWatch(&launcher_event_watch, nullptr);
}

void dk64::launcher_animation_update(recompui::LauncherMenu *menu) {
    std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
    float delta_time = launcher_context.started ? std::chrono::duration_cast<std::chrono::milliseconds>(now - launcher_context.last_update_time).count() / 1000.0f : 0.0f;
    if (launcher_context.skip_animation_next_update) {
        delta_time = std::max(animation_skip_time - launcher_context.seconds, 0.0f);
        launcher_context.skip_animation_next_update = false;
    }

    launcher_context.seconds += delta_time;
    launcher_context.last_update_time = now;
    launcher_context.started = true;

    recompui::Element *background_container = menu->get_background_container();
    float dp_to_pixel_ratio = background_container->get_dp_to_pixel_ratio();
    float bg_width = background_container->get_client_width() / dp_to_pixel_ratio;
    float bg_height = background_container->get_client_height() / dp_to_pixel_ratio;
    for (size_t i = 0; i < launcher_context.cloud_svgs.size(); i++) {
        update_animated_svg(launcher_context.cloud_svgs[i], delta_time, bg_width, bg_height);
    }
    update_animated_svg(launcher_context.isles_svg, delta_time, bg_width, bg_height);
    update_animated_svg(launcher_context.logo_64_svg, delta_time, bg_width, bg_height);
    update_animated_svg(launcher_context.logo_k_svg, delta_time, bg_width, bg_height);
    update_animated_svg(launcher_context.logo_d_svg, delta_time, bg_width, bg_height);
    update_animated_svg(launcher_context.logo_recomp_svg, delta_time, bg_width, bg_height);
    launcher_context.water->set_top(bg_height - 70);

    launcher_context.bg_svg.svg->set_top(0);
    launcher_context.bg_svg.svg->set_width(100, recompui::Unit::Percent);
    launcher_context.bg_svg.svg->set_width(100, recompui::Unit::Percent);
    

    float wrapper_phase = std::clamp((launcher_context.seconds - jiggy_move_over_start) / (jiggy_move_over_end - jiggy_move_over_start), 0.0f, 1.0f);
    if (wrapper_phase != launcher_context.wrapper_phase) {
        float game_option_menu_opacity = interpolate_value(0, 1.0f, wrapper_phase, InterpolationMethod::Smootherstep);
        for (auto option : menu->get_game_options_menu()->get_options()) {
            option->set_opacity(game_option_menu_opacity);
        }

        float game_option_menu_right = interpolate_value(launcher_options_right_position_start, launcher_options_right_position_end, wrapper_phase, InterpolationMethod::Smootherstep);
        menu->get_game_options_menu()->set_right(game_option_menu_right);

        launcher_context.wrapper_phase = wrapper_phase;
    }

    if (!launcher_context.options_enabled && launcher_context.seconds >= jiggy_move_over_end) {
        SDL_DelEventWatch(&launcher_event_watch, nullptr);

        for (auto option : menu->get_game_options_menu()->get_options()) {
            option->set_enabled(true);
            option->set_opacity(1.0f);
        }

        launcher_context.options_enabled = true;
    }
}