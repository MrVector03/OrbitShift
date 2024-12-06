#ifndef COSMIC_BODIES_H
#define COSMIC_BODIES_H

#include "rafgl.h"
#include "game_constants.h"
#include <math.h>

extern const rafgl_pixel_rgb_t sun_color;

extern const double sun_surface_noise_factor;

extern const rafgl_pixel_rgb_t sky_color;

void draw_realistic_sun(rafgl_raster_t raster, int x, int y, int radius);

void draw_realistic_sun_with_texture(rafgl_raster_t raster, int x, int y, int radius, rafgl_raster_t sun_texture);


void generate_starfield(rafgl_raster_t raster, int num_stars);

void draw_a_star(rafgl_raster_t raster, int x, int y, int radius);

#endif //COSMIC_BODIES_H


