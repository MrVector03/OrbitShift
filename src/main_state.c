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

static rafgl_raster_t raster, raster2, perlin_raster;
static rafgl_texture_t texture;


float sun_x = RASTER_WIDTH / 2;
float sun_y = RASTER_HEIGHT / 2;
int sun_radius = RASTER_HEIGHT / 40;

static int raster_width = RASTER_WIDTH, raster_height = RASTER_HEIGHT;

int debug_mode = 0;

solar_system_t solar_system;

void main_state_init(GLFWwindow *window, void *args, int width, int height) {
    raster_width = width;
    raster_height = height;

    printf("Initializing rasters with width: %d, height: %d\n", raster_width, raster_height);

    rafgl_raster_init(&raster, raster_width, raster_height);
    rafgl_raster_init(&raster2, raster_width, raster_height);

    perlin_raster = generate_perlin(8, 0.7);
    solar_system = generate_solar_system(8, sun_radius, sun_x, sun_y, perlin_raster);

    rafgl_texture_init(&texture);
    generate_starfield(raster, 1000);
    draw_realistic_sun_with_texture(raster, sun_x, sun_y, sun_radius, perlin_raster, 0.1  );
}

void main_state_update(GLFWwindow *window, float delta_time, rafgl_game_data_t *game_data, void *args) {
    if (game_data->keys_down[GLFW_KEY_SPACE]) {
        debug_mode = 0;
    } else {
        debug_mode = 1;
    }

    // static float time = 0.0f; // Time starts at 0
    // time += 1.0;
    //perlin_raster = generate_animated_perlin(6, 0.7, time); // Update raster


    draw_realistic_sun(raster, sun_x, sun_y, sun_radius);
    rafgl_pixel_rgb_t color_white = {255, 255, 255};
    draw_ellipse(raster, sun_x, sun_y, 100, 50, color_white);
    for (int i = 0; i < solar_system.num_bodies; i++) {
        cosmic_body_t *planet = &solar_system.planets[i];
        draw_ellipse(raster, planet->orbit_center_x, planet->orbit_center_y, planet->orbit_radius_x, planet->orbit_radius_y, color_white);
    }

    // draw_realistic_sun_with_texture(raster, sun_x, sun_y, sun_radius, perlin_raster, 0.1);

    // for (int i = 0; i < raster_width; i++) {
    //     yn = 1.0 * i / raster_height;
    //     for (int j = 0; j < raster_height; j++) {
    //         xn = 1.0 * j / raster_width;
    //         pixel_at_m(raster2, i, j) = pixel_at_m(perlin_raster, yn, xn);
    //     }
    // }
}


void main_state_render(GLFWwindow *window, void *args) {
    rafgl_texture_load_from_raster(&texture, debug_mode ? &raster : &perlin_raster);
    rafgl_texture_show(&texture, 0);
}


void main_state_cleanup(GLFWwindow *window, void *args) {

}
