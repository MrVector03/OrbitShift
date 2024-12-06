#include <utility.h>
#include <rafgl.h>
#include <game_constants.h>
#include <time.h>
#include <stdlib.h>

double cosine_interpolation(double a, double b, double s) {
    double f = (1 - cos(s * M_PI)) * 0.5;
    return a * (1 - f) + b * f;
}

void cosine_map_rescale(double *dst, int dst_width, int dst_height, double *src, int src_width, int src_height) {
    int x, y;
    double xn, yn;
    double fxs, fys;

    int ixs0, iys0, ixs1, iys1;

    double upper_middle, lower_middle;
    double sample_left, sample_right;

    double result;

    for (y = 0; y < dst_height; y++) {
        yn = 1.0 * y / dst_height;
        fys = yn * src_height;
        iys0 = (int)fys;
        iys1 = iys0 + 1;
        fys -= iys0;

        if (iys1 >= src_height) {
            iys1 = src_height - 1;
        }

        for (x = 0; x < dst_width; x++) {
            xn = 1.0 * x / dst_width;
            fxs = xn * src_width;
            ixs0 = (int)fxs;
            ixs1 = ixs0 + 1;
            fxs -= ixs0;
            if (ixs1 >= src_width) {
                ixs1 = src_width - 1;
            }

            sample_left = src[iys0 * src_width + ixs0];
            sample_right = src[iys0 * src_width + ixs1];
            upper_middle = cosine_interpolation(sample_left, sample_right, fxs);

            sample_left = src[iys1 * src_width + ixs0];
            sample_right = src[iys1 * src_width + ixs1];
            lower_middle = cosine_interpolation(sample_left, sample_right, fxs);

            result = cosine_interpolation(upper_middle, lower_middle, fys);

            dst[y * dst_width + x] = result;
        }
    }
}

void map_multiply_and_add(double *dst, double *src, int w, int h, double multiplier) {
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            dst[y * w + x] += src[y * w + x] * multiplier;
        }
    }
}

rafgl_raster_t generate_perlin(int octaves, double persistence) {
    srand(time(NULL));

    int octave_size = 2;
    double multiplier = 1.0;

    rafgl_raster_t raster;

    int width = pow(2, octaves);
    int height = width;

    int i, octave, x, y;
    double *tmp_map = malloc(height * width * sizeof(double));
    double *perlin_map = calloc(height * width, sizeof(double));
    double *octave_map;
    rafgl_pixel_rgb_t pix;
    rafgl_raster_init(&raster, width, height);

    for (octave = 0; octave < octaves; octave++) {
        octave_map = malloc(octave_size * octave_size * sizeof(double));
        for (y = 0; y < octave_size; y++) {
            for (x = 0; x < octave_size; x++) {
                octave_map[y * octave_size + x] = (1.0 + randf()) * 2.0 - 1.0;
            }
        }

        cosine_map_rescale(tmp_map, width, height, octave_map, octave_size, octave_size);
        map_multiply_and_add(perlin_map, tmp_map, width, height, multiplier);

        octave_size *= 2;
        multiplier *= persistence;
        memset(tmp_map, 0, height * width * sizeof(double));
        free(octave_map);
    }

    float sample;

    for (y = 0; y < height; y++) {
        for (x = 0; x < width; x++) {
            sample = perlin_map[y * width + x];
            sample = (sample + 1.0) / 2.0;
            if (sample < 0.0) sample = 0.0;
            if (sample > 1.0) sample = 1.0;
            pix.r = sample * 255;
            pix.g = sample * 255;
            pix.b = sample * 255;

            pixel_at_m(raster, x, y) = pix;
        }
    }

    free(perlin_map);
    free(tmp_map);

    return raster;
}

rafgl_raster_t generate_animated_perlin(int octaves, double persistence, double p_time) {
    srand(time(NULL));

    int octave_size = 2;
    double multiplier = 1.0;
    //printf("jhere")

    rafgl_raster_t raster;

    int width = pow(2, octaves);
    int height = width;

    int x, y, octave;
    double *tmp_map = malloc(height * width * sizeof(double));
    double *perlin_map = calloc(height * width, sizeof(double));
    double *octave_map;
    rafgl_pixel_rgb_t pix;
    rafgl_raster_init(&raster, width, height);

    for (octave = 0; octave < octaves; octave++) {
        octave_map = malloc(octave_size * octave_size * sizeof(double));
        for (y = 0; y < octave_size; y++) {
            for (x = 0; x < octave_size; x++) {
                // Use time to influence octave values for smooth movement
                octave_map[y * octave_size + x] = (1.0 + randf()) * 2.0 - 1.0 + sin(p_time * 0.1 + x + y);
            }
        }

        cosine_map_rescale(tmp_map, width, height, octave_map, octave_size, octave_size);
        map_multiply_and_add(perlin_map, tmp_map, width, height, multiplier);

        octave_size *= 2;
        multiplier *= persistence;
        memset(tmp_map, 0, height * width * sizeof(double));
        free(octave_map);
    }

    float sample;

    for (y = 0; y < height; y++) {
        for (x = 0; x < width; x++) {
            // Modify sample value to add smooth changes
            sample = perlin_map[y * width + x] + sin(p_time * 0.05);
            sample = (sample + 1.0) / 2.0;
            if (sample < 0.0) sample = 0.0;
            if (sample > 1.0) sample = 1.0;
            pix.r = sample * 255;
            pix.g = sample * 255;
            pix.b = sample * 255;

            pixel_at_m(raster, x, y) = pix;
        }
    }

    free(perlin_map);
    free(tmp_map);

    return raster;
}

void draw_ellipse(rafgl_raster_t raster, int xc, int yc, int rx, int ry, rafgl_pixel_rgb_t color) {
    int x, y;
    float rx2 = rx * rx;  // Square of radius along x-axis
    float ry2 = ry * ry;  // Square of radius along y-axis
    float two_rx2 = 2 * rx2;
    float two_ry2 = 2 * ry2;

    // Region 1
    float p1 = ry2 - (rx2 * ry) + (0.25 * rx2);
    x = 0;
    y = ry;
    int px = 0;
    int py = two_rx2 * y;

    while (px < py) {
        // Draw pixels for this point
        pixel_at_m(raster, xc + x, yc + y) = color;
        pixel_at_m(raster, xc - x, yc + y) = color;
        pixel_at_m(raster, xc + x, yc - y) = color;
        pixel_at_m(raster, xc - x, yc - y) = color;

        x++;
        px += two_ry2;
        if (p1 < 0) {
            p1 += ry2 + px;
        } else {
            y--;
            py -= two_rx2;
            p1 += ry2 + px - py;
        }
    }

    // Region 2
    float p2 = (ry2) * (x + 0.5) * (x + 0.5) + (rx2) * (y - 1) * (y - 1) - (rx2 * ry2);

    while (y > 0) {
        // Draw pixels for this point
        pixel_at_m(raster, xc + x, yc + y) = color;
        pixel_at_m(raster, xc - x, yc + y) = color;
        pixel_at_m(raster, xc + x, yc - y) = color;
        pixel_at_m(raster, xc - x, yc - y) = color;

        y--;
        py -= two_rx2;
        if (p2 > 0) {
            p2 += rx2 - py;
        } else {
            x++;
            px += two_ry2;
            p2 += rx2 - py + px;
        }
    }
}
