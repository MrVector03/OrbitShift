#ifndef COSMIC_BODIES_H
#define COSMIC_BODIES_H

#include "rafgl.h"
#include "game_constants.h"
#include <math.h>

typedef struct {
    int current_x;
    int current_y;
    int radius;

    int is_center;

    rafgl_raster_t texture;

    /// ORBITAL PARAMETERS
    int orbit_center_x;
    int orbit_center_y;
    double orbit_radius_x;
    double orbit_radius_y;
} cosmic_body_t;

typedef struct {
    cosmic_body_t sun;
    cosmic_body_t planets[32];
    int num_bodies;
} solar_system_t;

extern const rafgl_pixel_rgb_t sun_color;

extern const double sun_surface_noise_factor;

extern const rafgl_pixel_rgb_t sky_color;

void draw_realistic_sun(rafgl_raster_t raster, int x, int y, int radius);

void draw_realistic_sun_with_texture(rafgl_raster_t raster, int x, int y, int radius, rafgl_raster_t sun_texture, double smooth_factor);

void scatter_stars(rafgl_raster_t raster, int num_stars);

solar_system_t generate_solar_system(int num_planets, int sun_radius, int sun_x, int sun_y, rafgl_raster_t sun_texture);

void set_background(rafgl_raster_t raster, rafgl_raster_t background, rafgl_pixel_rgb_t bg_color, int num_stars);

#endif //COSMIC_BODIES_H


