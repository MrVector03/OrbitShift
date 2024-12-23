#include "rafgl.h"
#include "game_constants.h"

#ifndef UTILITY_H
#define UTILITY_H


double cosine_interpolationf(double a, double b, double s);

void cosine_map_rescale(double *dst, int dst_width, int dst_height, double *src, int src_width, int src_height);

rafgl_raster_t generate_perlin(int octaves, double persistence);

rafgl_raster_t generate_animated_perlin(int octaves, double persistence, double time);

void map_multiply_and_add(double *dst, double *src, int w, int h, double multiplier);

void draw_ellipse(rafgl_raster_t raster, int xc, int yc, int rx, int ry, rafgl_pixel_rgb_t color);

rafgl_raster_t generate_galaxy_texture(int width, int height, int octaves, double persistence, rafgl_pixel_rgb_t tint);

void update_ellipsoid_path_point(float *x, float *y, float cx, float cy, float a, float b, float *theta, float delta_time, float speed, int direction);

rafgl_raster_t generate_perlin_with_color(int octaves, double persistence);

void apply_distortion(rafgl_raster_t raster, float distortion_factor);

void apply_screen_distortion(rafgl_raster_t raster, float delta_time_elapsed, float distortion_duration);

void whiteout(rafgl_raster_t raster, float white_factor);

void apply_whiteout(rafgl_raster_t raster, float delta_time_elapsed, float whiteout_duration);

void custom_rafgl_raster_draw_spritesheet(rafgl_raster_t *raster, rafgl_spritesheet_t *spritesheet, int frame_x, int frame_y, int x, int y);

void apply_radial_blur(rafgl_raster_t raster, rafgl_raster_t *output, float blur_strength);

void apply_gaussian_blur(rafgl_raster_t raster, int radius);

void render_proximity_vignette(rafgl_raster_t raster, int cx, int cy, float vignette_factor, float rocket_sun_dist, float vignette_r, float vignette_g, float vignette_b, float r);

#endif //UTILITY_H
