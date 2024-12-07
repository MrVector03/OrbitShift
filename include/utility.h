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

rafgl_raster_t generate_galaxy_texture(int width, int height, int octaves, double persistence);

void update_ellipsoid_path_point(float *x, float *y, float cx, float cy, float a, float b, float *theta, float delta_time, float speed, int direction);

rafgl_raster_t generate_perlin_with_color(int octaves, double persistence);

#endif //UTILITY_H
