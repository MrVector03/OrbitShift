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

static rafgl_raster_t raster, raster2, perlin_raster, galaxy_texture, background_raster;
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
    srand(time(NULL));
    printf("Initializing rasters with width: %d, height: %d\n", raster_width, raster_height);

    rafgl_raster_init(&raster, raster_width, raster_height);
    rafgl_raster_init(&raster2, raster_width, raster_height);
    rafgl_raster_init(&background_raster, raster_width, raster_height);

    /// generating
    perlin_raster = generate_perlin(8, 0.7);
    galaxy_texture = generate_galaxy_texture(raster_width, raster_height, 4, 0.05);
    solar_system = generate_solar_system(1, sun_radius, sun_x, sun_y, perlin_raster);

    rafgl_texture_init(&texture);

    set_background(background_raster, galaxy_texture, sky_color, 1000);

}

void main_state_update(GLFWwindow *window, float delta_time, rafgl_game_data_t *game_data, void *args) {
    if (game_data->keys_down[GLFW_KEY_SPACE]) {
        debug_mode = 0;
    } else {
        debug_mode = 1;
    }

    //draw_ellipse(raster, sun_x, sun_y, 100, 50, color_white);

    memcpy(raster.data, background_raster.data, raster.width * raster.height * sizeof(rafgl_pixel_rgb_t));

    //draw_realistic_sun(raster, sun_x, sun_y, sun_radius);
    //scatter_stars(raster, 1000);
    rafgl_pixel_rgb_t color_white = {255, 255, 255};
    for (int i = 0; i < solar_system.num_bodies; i++) {
        draw_ellipse(raster, solar_system.planets[i].orbit_center_x,
            solar_system.planets[i].orbit_center_y,
            solar_system.planets[i].orbit_radius_x,
            solar_system.planets[i].orbit_radius_y,
            color_white);
    }
    render_planets(raster, &solar_system);


}


void main_state_render(GLFWwindow *window, void *args) {
    rafgl_texture_load_from_raster(&texture, debug_mode ? &raster : &perlin_raster);
    rafgl_texture_show(&texture, 0);
}


void main_state_cleanup(GLFWwindow *window, void *args) {

}
