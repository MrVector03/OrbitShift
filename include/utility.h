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

#endif //UTILITY_H
