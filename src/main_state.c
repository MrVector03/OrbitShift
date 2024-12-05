//4hero
//set sprajtova je kljucan u animaciji
//to se naziva spritesheet
//ukoliko idemo kroz neki red i vrtimo ga u krug imamo animaciju kretanja
//spite je neka slicica karaktera ili efekta i sl.
#include <main_state.h>
#include <glad/glad.h>
#include <math.h>
#include <GLFW/glfw3.h>
#include <GL/gl.h>

#include <rafgl.h>

#include <game_constants.h>

static rafgl_raster_t doge;
static rafgl_raster_t upscaled_doge;
static rafgl_raster_t raster, raster2;
static rafgl_raster_t checker;

static rafgl_texture_t texture;

static rafgl_spritesheet_t hero;

int corner_size = 128;

static int raster_width = RASTER_WIDTH, raster_height = RASTER_HEIGHT;

static char save_file[256];
int save_file_no = 0;

// CONSTANTS

const rafgl_pixel_rgb_t sun_color = {214, 75, 15};
const rafgl_pixel_rgb_t sky_color = {3, 4, 15};

const double sun_surface_noise_factor = 0.001;


void main_state_init(GLFWwindow *window, void *args, int width, int height)
{
    /* inicijalizacija */
    /* raster init nam nije potreban ako radimo load from image */
    rafgl_raster_load_from_image(&doge, "res/images/doge.png");
    rafgl_raster_load_from_image(&checker, "res/images/checker32.png");

    raster_width = width;
    raster_height = height;


    rafgl_raster_init(&raster, raster_width, raster_height);
    rafgl_raster_init(&raster2, raster_width, raster_height);

    //corner_size = ((raster_width - raster_width / 8) + (raster_height - raster_height / 8)) / 2;

    //spritesheet je inicijalizovan tako sto prosledimo
    //koju sliku ucitavamo
    //i koliko ima slicica po x, a koliko po y
    //i to smo samo zapamtili u sprite sheet hero
    rafgl_spritesheet_init(&hero, "res/images/character.png", 10, 4);

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
    //rafgl_raster_draw_circle(&raster, sun_x, sun_y, sun_radius, &sun_color);



    rafgl_texture_load_from_raster(&texture, &raster);
    rafgl_texture_show(&texture, 0);
}


void main_state_render(GLFWwindow *window, void *args)
{
    /* prikazi teksturu */
    rafgl_texture_load_from_raster(&texture, &raster);
    rafgl_texture_show(&texture, 0);
}


void main_state_cleanup(GLFWwindow *window, void *args)
{
    rafgl_raster_cleanup(&raster);
    rafgl_raster_cleanup(&raster2);
    rafgl_texture_cleanup(&texture);

}

