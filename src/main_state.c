#include <main_state.h>
#include <glad/glad.h>
#include <math.h>
#include <GLFW/glfw3.h>
#include <GL/gl.h>

#include <rafgl.h>

#include <game_constants.h>

static rafgl_raster_t raster;
static rafgl_texture_t texture;


static int raster_width = RASTER_WIDTH, raster_height = RASTER_HEIGHT;


// CONSTANTS

const rafgl_pixel_rgb_t sun_color = {214, 75, 15};
const rafgl_pixel_rgb_t sky_color = {3, 4, 15};

const double sun_surface_noise_factor = 0.001;


void main_state_init(GLFWwindow *window, void *args, int width, int height) {
    raster_width = width;
    raster_height = height;

    rafgl_raster_init(&raster, raster_width, raster_height);

    rafgl_texture_init(&texture);
}

void draw_realistic_sun(rafgl_raster_t raster, int x, int y, int radius) {
    for (int i = 0; i < raster_width; i++) {
        for (int j = 0; j < raster_height; j++) {
            float dx = i - x;
            float dy = j - y;
            float distance = sqrt(dx * dx + dy * dy);

            // TODO: Make surface smoother but still random
            double noise = (rand() % 100) / 100.0 * sun_surface_noise_factor;

            if (distance < radius * (1 + noise))
            {
                pixel_at_m(raster, i, j) = sun_color;
            }
            else
            {
                pixel_at_m(raster, i, j) = sky_color;
            }
        }
    }
}


void main_state_update(GLFWwindow *window, float delta_time, rafgl_game_data_t *game_data, void *args)
{


    float sun_x = raster_width / 2;
    float sun_y = raster_height / 2;
    int sun_radius = raster_height / 20;

    draw_realistic_sun(raster, sun_x, sun_y, sun_radius);

    rafgl_texture_load_from_raster(&texture, &raster);
    rafgl_texture_show(&texture, 0);
}


void main_state_render(GLFWwindow *window, void *args)
{
    rafgl_texture_load_from_raster(&texture, &raster);
    rafgl_texture_show(&texture, 0);
}


void main_state_cleanup(GLFWwindow *window, void *args)
{

}
