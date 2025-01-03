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
    //srand(time(NULL));

    int octave_size = 2;
    double multiplier = 1.0;
    //printf("here")

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
    float rx2 = rx * rx;
    float ry2 = ry * ry;
    float two_rx2 = 2 * rx2;
    float two_ry2 = 2 * ry2;

    float p1 = ry2 - (rx2 * ry) + (0.25 * rx2);
    x = 0;
    y = ry;
    int px = 0;
    int py = two_rx2 * y;

    while (px < py) {
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

    float p2 = (ry2) * (x + 0.5) * (x + 0.5) + (rx2) * (y - 1) * (y - 1) - (rx2 * ry2);

    while (y > 0) {
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

double radial_gradient(int x, int y, int center_x, int center_y, int width, int height) {
    double dx = (double)(x - center_x) / (width / 2);
    double dy = (double)(y - center_y) / (height / 2);
    return 1.0 - sqrt(dx * dx + dy * dy);
}

rafgl_pixel_rgb_t map_to_color(double value) {
    value = fmax(0.0, fmin(1.0, value));

    double darkness_factor = 0.1;

    if (value < 0.25) return (rafgl_pixel_rgb_t){value * 255 * darkness_factor, 0, 255 * darkness_factor};              // Purple
    if (value < 0.5) return (rafgl_pixel_rgb_t){0, value * 255 * darkness_factor, 255 * darkness_factor};               // Blue
    if (value < 0.75) return (rafgl_pixel_rgb_t){value * 255 * darkness_factor, value * 255 * darkness_factor, 255 * darkness_factor};    // White

    return (rafgl_pixel_rgb_t){255 * darkness_factor, value * 255 * darkness_factor, value * 200 * darkness_factor};                      // Yellow
}

rafgl_raster_t generate_galaxy_texture(int width, int height, int octaves, double persistence, rafgl_pixel_rgb_t tint) {
    int tint_factor = rand() % 128;

    rafgl_raster_t raster;
    rafgl_raster_init(&raster, width, height);

    double *noise_map = calloc(width * height, sizeof(double));
    double *temp_map = malloc(width * height * sizeof(double));

    int center_x = width / 2;
    int center_y = height / 2;

    double max_intensity = 0.0;

    for (int octave = 0; octave < octaves; octave++) {
        int octave_size = pow(2, octave + 2);
        double amplitude = pow(persistence, octave);

        for (int y = 0; y < octave_size; y++) {
            for (int x = 0; x < octave_size; x++) {
                temp_map[y * octave_size + x] = (1.0 + randf()) * 2.0 - 1.0;
            }
        }

        cosine_map_rescale(noise_map, width, height, temp_map, octave_size, octave_size);

        for (int i = 0; i < width * height; i++) {
            noise_map[i] += temp_map[i] * amplitude;
            max_intensity = fmax(max_intensity, noise_map[i]);
        }
    }

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            double radial = radial_gradient(x, y, center_x, center_y, width, height);
            double noise_value = noise_map[y * width + x] / max_intensity;

            double final_value = noise_value * radial;

            rafgl_pixel_rgb_t pix = map_to_color(final_value);

            pix.r = (pix.r * 0.5 + tint.r * 0.5);
            pix.g = (pix.g * 0.5 + tint.g * 0.5);
            pix.b = (pix.b * 0.5 + tint.b * 0.5);

            pixel_at_m(raster, x, y) = pix;
        }
    }

    free(noise_map);
    free(temp_map);

    return raster;
}

void update_ellipsoid_path_point(float *x, float *y, float cx, float cy, float a, float b, float *theta, float delta_time, float speed, int direction) {
    *theta += delta_time * speed * direction;

    if (*theta > 2 * M_PI) {
        *theta -= 2 * M_PI;
    } else if (*theta < 0) {
        *theta += 2 * M_PI;
    }

    *x = cx + a * cos(*theta);
    *y = cy + b * sin(*theta);
}

rafgl_raster_t generate_perlin_with_color(int octaves, double persistence) {
    srand(time(NULL));

    int octave_size = 2;
    double multiplier = 1.0;
    rafgl_raster_t raster;

    int width = RASTER_WIDTH;
    int height = RASTER_HEIGHT;

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
            sample = (sample + 1.0) / 2.0;  // Normalize between 0 and 1

            // Color mapping (Blue -> Green -> Yellow -> Red)
            if (sample < 0.25) {
                pix.r = 0;
                pix.g = sample * 255 * 4;
                pix.b = 255;
            } else if (sample < 0.5) {
                pix.r = 0;
                pix.g = 255;
                pix.b = 255 - (sample - 0.25) * 255 * 2;
            } else if (sample < 0.75) {
                pix.r = (sample - 0.5) * 255 * 2;
                pix.g = 255 - (sample - 0.5) * 255 * 2;
                pix.b = 0;
            } else {
                pix.r = 255;
                pix.g = 255 - (sample - 0.75) * 255 * 4;
                pix.b = 0;
            }

            pixel_at_m(raster, x, y) = pix;
        }
    }

    free(perlin_map);
    free(tmp_map);

    return raster;
}

void apply_screen_distortion(rafgl_raster_t raster, float delta_time_elapsed, float distortion_duration) {
    float t = delta_time_elapsed / distortion_duration;
    if (t > 1.0) t = 1.0;
    float distortion_factor = sin(t * M_PI) * 100.0;
    apply_distortion(raster, distortion_factor);
}

void apply_distortion(rafgl_raster_t raster, float distortion_factor) {
    rafgl_raster_t temp_raster;
    rafgl_raster_init(&temp_raster, raster.width, raster.height);

    for (int y = 0; y < raster.height; y++) {
        for (int x = 0; x < raster.width; x++) {
            float offset_x = sin(y * 0.05f) * distortion_factor;
            float offset_y = cos(x * 0.05f) * distortion_factor;

            int src_x = (int)(x + offset_x) % raster.width;
            int src_y = (int)(y + offset_y) % raster.height;

            if (src_x < 0) src_x += raster.width;
            if (src_y < 0) src_y += raster.height;

            pixel_at_m(temp_raster, x, y) = pixel_at_m(raster, src_x, src_y);
        }
    }

    memcpy(raster.data, temp_raster.data, raster.width * raster.height * sizeof(rafgl_pixel_rgb_t));

    rafgl_raster_cleanup(&temp_raster);
}

void apply_whiteout(rafgl_raster_t raster, float delta_time_elapsed, float whiteout_duration) {
    float t = delta_time_elapsed / whiteout_duration;
    float peak = whiteout_duration * 0.5;

    if (t > 1.0) t = 1.0;

    float whiteness_factor;

    if (t <= peak) {
        whiteness_factor = sin(t * M_PI / peak);
    } else {
        whiteness_factor = cos(t * M_PI / peak);
    }

    whiteout(raster, whiteness_factor);
}

void whiteout(rafgl_raster_t raster, float white_factor) {
    for (int y = 0; y < raster.height; y++) {
        for (int x = 0; x < raster.width; x++) {
            rafgl_pixel_rgb_t pix = pixel_at_m(raster, x, y);
            pix.r = pix.r + (255 - pix.r) * white_factor;
            pix.g = pix.g + (255 - pix.g) * white_factor;
            pix.b = pix.b + (255 - pix.b) * white_factor;
            pixel_at_m(raster, x, y) = pix;
        }
    }
}

void custom_rafgl_raster_draw_spritesheet(rafgl_raster_t *raster, rafgl_spritesheet_t *spritesheet, int frame_x, int frame_y, int x, int y) {
    int frame_width = spritesheet->frame_width;
    int frame_height = spritesheet->frame_height;
    int sheet_width = spritesheet->sheet_width;

    int frame_x_pos = (frame_x % (sheet_width / frame_width)) * frame_width;
    int frame_y_pos = (frame_y / (sheet_width / frame_width)) * frame_height;

    rafgl_pixel_rgb_t background_color = {255, 0, 249, 255}; // #FF00F9

    for (int i = 0; i < frame_width; i++) {
        for (int j = 0; j < frame_height; j++) {
            rafgl_pixel_rgb_t pixel = rafgl_point_sample(&spritesheet->sheet, (float)(frame_x_pos + i) / sheet_width, (float)(frame_y_pos + j) / spritesheet->sheet_height);
            if (pixel.r != background_color.r || pixel.g != background_color.g || pixel.b != background_color.b) { // Check if the pixel is not the background color
                pixel_at_pm(raster, x + i, y + j) = pixel;
            }
        }
    }
}

void apply_radial_blur(rafgl_raster_t raster, rafgl_raster_t *output, float blur_strength) {
    int x, y, sx, sy;
    float dx, dy, distance, weight, total_weight;
    rafgl_pixel_rgb_t original_color, sample_color, final_color;

    int width = raster.width;
    int height = raster.height;
    int center_x = width / 2;
    int center_y = height / 2;

    for (y = 0; y < height; y++) {
        for (x = 0; x < width; x++) {
            dx = x - center_x;
            dy = y - center_y;
            distance = sqrt(dx * dx + dy * dy);

            final_color = (rafgl_pixel_rgb_t){0, 0, 0};
            total_weight = 0.0f;

            for (float t = 0.0f; t <= 1.0f; t += 1.0f / blur_strength) {
                sx = center_x + (int)(dx * t);
                sy = center_y + (int)(dy * t);

                if (sx >= 0 && sx < width && sy >= 0 && sy < height) {
                    sample_color = pixel_at_m(raster, sx, sy);
                    weight = 1.0f - t;
                    final_color.r += sample_color.r * weight;
                    final_color.g += sample_color.g * weight;
                    final_color.b += sample_color.b * weight;
                    total_weight += weight;
                }
            }

            final_color.r = (int)(final_color.r / total_weight);
            final_color.g = (int)(final_color.g / total_weight);
            final_color.b = (int)(final_color.b / total_weight);

            pixel_at_m(raster, x, y) = final_color;
        }
    }
}

void apply_gaussian_blur(rafgl_raster_t raster, int radius) {
    int width = raster.width;
    int height = raster.height;
    rafgl_raster_t temp_raster;
    rafgl_raster_init(&temp_raster, width, height);

    float sigma = radius / 2.0f;
    int kernel_size = 2 * radius + 1;
    float *kernel = (float *)malloc(kernel_size * sizeof(float));

    float sum = 0.0f;
    for (int i = -radius; i <= radius; i++) {
        kernel[i + radius] = exp(-(i * i) / (2 * sigma * sigma)) / (sqrt(2 * M_PI) * sigma);
        sum += kernel[i + radius];
    }

    for (int i = 0; i < kernel_size; i++) {
        kernel[i] /= sum;
    }

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            float r = 0.0f, g = 0.0f, b = 0.0f;
            for (int k = -radius; k <= radius; k++) {
                int pixel_x = fmin(width - 1, fmax(0, x + k));
                rafgl_pixel_rgb_t pixel = pixel_at_m(raster, pixel_x, y);
                r += pixel.r * kernel[k + radius];
                g += pixel.g * kernel[k + radius];
                b += pixel.b * kernel[k + radius];
            }
            pixel_at_m(temp_raster, x, y) = (rafgl_pixel_rgb_t){(unsigned char)r, (unsigned char)g, (unsigned char)b};
        }
    }

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            float r = 0.0f, g = 0.0f, b = 0.0f;
            for (int k = -radius; k <= radius; k++) {
                int pixel_y = fmin(height - 1, fmax(0, y + k));
                rafgl_pixel_rgb_t pixel = pixel_at_m(temp_raster, x, pixel_y);
                r += pixel.r * kernel[k + radius];
                g += pixel.g * kernel[k + radius];
                b += pixel.b * kernel[k + radius];
            }
            pixel_at_m(raster, x, y) = (rafgl_pixel_rgb_t){(unsigned char)r, (unsigned char)g, (unsigned char)b};
        }
    }

    free(kernel);
    rafgl_raster_cleanup(&temp_raster);
}

void render_proximity_vignette(rafgl_raster_t raster, int cx, int cy, float vignette_factor, float rocket_sun_dist, float vignette_r, float vignette_g, float vignette_b, float r) {
    for (int i = 0; i < raster.width; i++) {
        for (int j = 0; j < raster.height; j++) {
            float dist = rafgl_distance2D(i, j, cx, cy) / r;

            dist = powf(dist, 1.8f);

            rafgl_pixel_rgb_t sampled = pixel_at_m(raster, i, j);
            rafgl_pixel_rgb_t result;

            float tint_factor = dist * vignette_factor;

            if (rocket_sun_dist < 100.0) {
                float proximity_factor = 1.0 - (rocket_sun_dist / 100.0);
                tint_factor *= proximity_factor;
                result.r = rafgl_saturatei(sampled.r * (1.0f - tint_factor) + vignette_r * tint_factor * 255);
                result.g = rafgl_saturatei(sampled.g * (1.0f - tint_factor) + vignette_g * tint_factor * 255);
                result.b = rafgl_saturatei(sampled.b * (1.0f - tint_factor) + vignette_b * tint_factor * 255);
            } else {
                result.r = rafgl_saturatei(sampled.r * (1.0f - tint_factor));
                result.g = rafgl_saturatei(sampled.g * (1.0f - tint_factor));
                result.b = rafgl_saturatei(sampled.b * (1.0f - tint_factor));
            }
            pixel_at_m(raster, i, j) = result;
        }
    }
}
