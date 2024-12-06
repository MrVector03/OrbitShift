#include <cosmic_bodies.h>
#include <rafgl.h>
#include <game_constants.h>

// CONSTANTS

const rafgl_pixel_rgb_t sun_color = { {214, 75, 15} };
const double sun_surface_noise_factor = 0.001;
const rafgl_pixel_rgb_t sky_color = { {3, 4, 15} };


void draw_realistic_sun(rafgl_raster_t raster, int x, int y, int radius) {
    for (int i = 0; i < RASTER_WIDTH; i++) {
        for (int j = 0; j < RASTER_HEIGHT; j++) {
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

void draw_realistic_sun_with_texture(rafgl_raster_t raster, int x, int y, int radius, rafgl_raster_t sun_texture) {
    for (int i = 0; i < RASTER_WIDTH; i++) {
        for (int j = 0; j < RASTER_HEIGHT; j++) {
            float dx = i - x;
            float dy = j - y;
            float distance = sqrt(dx * dx + dy * dy);

            // TODO: Make surface smoother but still random
            double noise = (rand() % 100) / 100.0 * sun_surface_noise_factor;


            int yn = 1.0 * i / RASTER_HEIGHT;
            int xn = 1.0 * j / RASTER_WIDTH;

            if (distance < radius * (1 + noise))
            {
                pixel_at_m(raster, i, j) = pixel_at_m(sun_texture, i, j);
            }
            else
            {
                pixel_at_m(raster, i, j) = sky_color;
            }
        }
    }
}


void draw_a_star(rafgl_raster_t raster, int x, int y, int radius) {
    for (int i = 0; i < RASTER_WIDTH; i++) {
        for (int j = 0; j < RASTER_HEIGHT; j++) {
            float dx = i - x;
            float dy = j - y;
            float distance = sqrt(dx * dx + dy * dy);

            if (distance < radius)
            {
                pixel_at_m(raster, i, j).rgba = rafgl_RGB(255, 255, 255);
            }
        }
    }
}

void generate_starfield(rafgl_raster_t raster, int num_stars) {
    for (int i = 0; i < num_stars; i++) {
        int x = rand() % RASTER_WIDTH;
        int y = rand() % RASTER_HEIGHT;
        pixel_at_m(raster, x, y).rgba = rafgl_RGB(255, 255, 255);
    }
}
