#include <main_state.h>
#include <glad/glad.h>
#include <math.h>
#include <GLFW/glfw3.h>
#include <GL/gl.h>
#include <cosmic_bodies.h>
#include <time.h>

#include <rafgl.h>

#include <game_constants.h>
#include <utility.h>

static rafgl_raster_t raster, raster2, perlin_raster, galaxy_texture, background_raster, handbrake_raster;
static rafgl_spritesheet_t smoke_spritesheet, black_hole_spritesheet;

static rafgl_raster_t test_raster;

static rafgl_texture_t texture;


int hole_x = 0;
int hole_y = 0;

float sun_x = RASTER_WIDTH / 2;
float sun_y = RASTER_HEIGHT / 2;
int sun_radius = RASTER_HEIGHT / 40;

float orange_r = 1.0;
float orange_g = 0.5;
float orange_b = 0.0;

float black_hole_r = 0.0;
float black_hole_g = 0.0;
float black_hole_b = 0.0;

static int raster_width = RASTER_WIDTH, raster_height = RASTER_HEIGHT;

int debug_mode = 0;

solar_system_t solar_system;
spaceship rocket;

rafgl_pixel_rgb_t color_white = {255, 255, 255};
rafgl_pixel_rgb_t color_sun = {255, 255, 0};

rafgl_raster_t vignetted_raster;

/// FPS CONTROL CENTER
int hot_vignette = 1;   /// TURN ON/OFF SUN PROXIMITY VIGNETTE
int smoke_effects = 1;  /// 0 - NO SMOKE; 1 - SMOKE
int num_planets = 0;    /// 0,1,2 - OK;   3,4... - SHITS THE BED


void main_state_init(GLFWwindow *window, void *args, int width, int height) {
    raster_width = width;
    raster_height = height;
    srand(time(NULL));

    /// RASTER INITS
    rafgl_raster_init(&raster, raster_width, raster_height);
    rafgl_raster_init(&raster2, raster_width, raster_height);
    rafgl_raster_init(&vignetted_raster, raster_width, raster_height);
    rafgl_raster_init(&background_raster, raster_width, raster_height);
    rafgl_raster_init(&test_raster, raster_width, raster_height);
    rafgl_raster_load_from_image(&handbrake_raster, "res/images/handbrake.jpeg");

    rafgl_spritesheet_init(&smoke_spritesheet, "res/images/plumeplume.png", 6, 5);
    rafgl_spritesheet_init(&black_hole_spritesheet, "res/images/black_hole_spritesheet.png", 8, 8);

    /// GALAXY TEXTURE
    perlin_raster = generate_perlin(8, 0.7);
    galaxy_texture = generate_galaxy_texture(raster_width, raster_height, 4, 0.05);

    solar_system = generate_solar_system(num_planets, sun_radius, sun_x, sun_y, perlin_raster);

    /// export color quotients from next_system_color
    black_hole_r = solar_system.next_system_color.r / 255.0;
    black_hole_g = solar_system.next_system_color.g / 255.0;
    black_hole_b = solar_system.next_system_color.b / 255.0;

    glfwSwapInterval(1);
    rafgl_texture_init(&texture);

    /// ROCKET
    rocket = init_spaceship(solar_system.black_hole, 0.0, 0., 10);
    link_rocket(&rocket, smoke_effects);

    set_background(background_raster, galaxy_texture, sky_color, 1000);


    for (int i = 0; i < solar_system.num_bodies; i++) {
        draw_ellipse(raster, solar_system.planets[i].orbit_center_x,
            solar_system.planets[i].orbit_center_y,
            solar_system.planets[i].orbit_radius_x,
            solar_system.planets[i].orbit_radius_y,
            color_white);
    }
}

void main_state_update(GLFWwindow *window, float delta_time, rafgl_game_data_t *game_data, void *args) {
    if (game_data->keys_down[GLFW_KEY_SPACE]) {
        debug_mode = 0;
        rocket.speed = 0.0;
    } else {
        debug_mode = 1;
    }

    draw_ellipse(raster, sun_x, sun_y, 100, 50, color_white);


    float dist, vignette_factor = 1.5, vignette_scale_factor = 0.5;
    rafgl_pixel_rgb_t sampled, result;
    float cx = raster.width / 2;
    float cy = raster.height / 2;

    float rocket_sun_dist = rafgl_distance2D(solar_system.sun.current_x, solar_system.sun.current_y, rocket.curr_x, rocket.curr_y);
    float rocket_black_hole_dist = rafgl_distance2D(solar_system.black_hole.current_x, solar_system.black_hole.current_y, rocket.curr_x, rocket.curr_y);
    int closer_to_sun = 1;

    if (rocket_black_hole_dist < rocket_sun_dist) {
        rocket_sun_dist = rocket_black_hole_dist;
        closer_to_sun = 0;
    }

    float r = 750.0 + rocket_sun_dist * vignette_scale_factor;

    float vignette_r, vignette_g, vignette_b;

    if (closer_to_sun) {
        vignette_r = orange_r;
        vignette_g = orange_g;
        vignette_b = orange_b;
    } else {
        vignette_r = black_hole_r;
        vignette_g = black_hole_g;
        vignette_b = black_hole_b;
    }

    memcpy(raster.data, background_raster.data, raster.width * raster.height * sizeof(rafgl_pixel_rgb_t));

    render_planets(raster, black_hole_spritesheet, &solar_system);

    int moved = 0;
    if (game_data->keys_down[GLFW_KEY_W]) {
        move_rocket(&rocket, 0.7, 0.0, delta_time);
        moved = 1;
    }
    if (game_data->keys_down[GLFW_KEY_A]) {
        move_rocket(&rocket, 0.0, -0.1, delta_time);
    }
    if (game_data->keys_down[GLFW_KEY_S]) {
        move_rocket(&rocket, -0.7, 0.0, delta_time);
        moved = 1;
    }
    if (game_data->keys_down[GLFW_KEY_D]) {
        move_rocket(&rocket, 0.0, 0.1, delta_time);
    }

    move_rocket(&rocket, 0.0, 0.0, delta_time);
    draw_rocket(raster, &rocket, smoke_spritesheet, delta_time, moved);

    // TODO: Smoothly blend hot and normal vignettes
    for (int i = 0; i < raster.width; i++) {
        for (int j = 0; j < raster.height; j++) {
            dist = rafgl_distance2D(i, j, cx, cy) / r;

            // Apply a power function to make the vignette effect more pronounced on the edges
            dist = powf(dist, 1.8f); // You can adjust this exponent to control the softness of the vignette

            // Sample the current pixel color (e.g., from the background or previous frame)
            sampled = pixel_at_m(raster, i, j);

            float tint_factor = dist * vignette_factor;

            if (rocket_sun_dist < 150.0) {
                // Closer to the sun, shrink and turn the vignette more orange
                float proximity_factor = 1.0 - (rocket_sun_dist / 500.0);
                tint_factor *= proximity_factor;
                result.r = rafgl_saturatei(sampled.r * (1.0f - tint_factor) + vignette_r * tint_factor * 255);
                result.g = rafgl_saturatei(sampled.g * (1.0f - tint_factor) + vignette_g * tint_factor * 255);
                result.b = rafgl_saturatei(sampled.b * (1.0f - tint_factor) + vignette_b * tint_factor * 255);
            } else {
                // Far from the sun, keep the vignette black in the corners
                result.r = rafgl_saturatei(sampled.r * (1.0f - tint_factor));
                result.g = rafgl_saturatei(sampled.g * (1.0f - tint_factor));
                result.b = rafgl_saturatei(sampled.b * (1.0f - tint_factor));
            }

                pixel_at_m(raster, i, j) = result;
        }
    }
}


void main_state_render(GLFWwindow *window, void *args) {
    rafgl_texture_load_from_raster(&texture, debug_mode ? &raster : &test_raster);
    rafgl_texture_show(&texture, 0);
}


void main_state_cleanup(GLFWwindow *window, void *args) {

}
