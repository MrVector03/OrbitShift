#include <cosmic_bodies.h>
#include <rafgl.h>
#include <game_constants.h>
#include <time.h>
#include <utility.h>

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
            // else
            // {
            //     pixel_at_m(raster, i, j) = sky_color;
            // }
        }
    }
}

double previous_noise = 0.0;

// Function to smoothly transition noise over time
double smooth_noise(double current_noise, double smooth_factor) {
    return previous_noise * (1 - smooth_factor) + current_noise * smooth_factor;
}

// Function to map noise to a color (Yellow to Red)
rafgl_pixel_rgb_t map_noise_to_sun_color(double noise_value) {
    // Make sure the noise value is between 0.0 and 1.0
    noise_value = fmin(fmax(noise_value, 0.0), 1.0);

    // Map noise_value to a sun-like color (Yellow to Red)
    // 1.0 -> Red, 0.0 -> Yellow

    unsigned char b = (unsigned char)(255 * noise_value);  // Red increases as noise increases
    unsigned char r = (unsigned char)(255 * (1 - noise_value));  // Green decreases as noise increases
    unsigned char g = 0;  // Blue is 0 to keep the color yellow to red

    return (rafgl_pixel_rgb_t){r, g, b};
}

void draw_realistic_sun_with_texture(rafgl_raster_t raster, int x, int y, int radius, rafgl_raster_t sun_texture, double smooth_factor) {
    int texture_width = sun_texture.width;
    int texture_height = sun_texture.height;

    for (int i = 0; i < RASTER_WIDTH; i++) {
        for (int j = 0; j < RASTER_HEIGHT; j++) {
            float dx = i - x;
            float dy = j - y;
            float distance = sqrt(dx * dx + dy * dy);

            // Generate new random noise for the sun's surface
            double current_noise = (rand() % 100) / 100.0 * sun_surface_noise_factor;

            // Apply smoothing to the noise
            double smooth_noise_value = smooth_noise(current_noise, smooth_factor);

            // Update the previous noise value for the next frame
            previous_noise = smooth_noise_value;

            // Apply the noise to simulate the sun's surface
            if (distance < radius * (1 + smooth_noise_value)) {
                // Map the noise value to a color
                rafgl_pixel_rgb_t sun_color = map_noise_to_sun_color(smooth_noise_value);

                int tex_x = (int)((dx / radius + 1) * 0.5 * texture_width);
                int tex_y = (int)((dy / radius + 1) * 0.5 * texture_height);

                tex_x = (tex_x < 0) ? 0 : (tex_x >= texture_width) ? texture_width - 1 : tex_x;
                tex_y = (tex_y < 0) ? 0 : (tex_y >= texture_height) ? texture_height - 1 : tex_y;

                // Add the texture color to the sun's color
                rafgl_pixel_rgb_t texture_color = pixel_at_m(sun_texture, tex_x, tex_y);
                sun_color.r = (sun_color.r + texture_color.r) / 2; // Blend the colors
                sun_color.g = (sun_color.g + texture_color.g) / 2;
                sun_color.b = (sun_color.b + texture_color.b) / 2;

                pixel_at_m(raster, i, j) = sun_color;
            } else {
                pixel_at_m(raster, i, j) = sky_color;  // Sky color
            }
        }
    }
}

void scatter_stars(rafgl_raster_t raster, int num_stars) {
    for (int i = 0; i < num_stars; i++) {
        int x = rand() % RASTER_WIDTH;
        int y = rand() % RASTER_HEIGHT;
        rafgl_pixel_rgb_t star_color = {255, 255, 255};
        pixel_at_m(raster, x, y) = star_color;
    }
}

solar_system_t generate_solar_system(int num_planets, int sun_radius, int sun_x, int sun_y, rafgl_raster_t sun_texture) {
    int curr_orbit_radius_x = 200;
    int curr_orbit_radius_y = 100;

    solar_system_t solar_system;
    solar_system.sun = (cosmic_body_t){sun_x, sun_y, sun_radius, 1, sun_texture, sun_x, sun_y, curr_orbit_radius_x, curr_orbit_radius_y};
    solar_system.num_bodies = num_planets;
    srand(time(NULL));

    for (int i = 0; i < num_planets; i++) {
        cosmic_body_t planet;
        planet.orbit_center_x = sun_x;
        planet.orbit_center_y = sun_y;
        planet.orbit_radius_x = curr_orbit_radius_x;
        planet.orbit_radius_y = curr_orbit_radius_y;

        solar_system.planets[i] = planet;

        curr_orbit_radius_x += rand() % 100 + 50;
        curr_orbit_radius_y += rand() % 50 + 25;
    }

    return solar_system;
}

void set_background(rafgl_raster_t raster, rafgl_raster_t background, rafgl_pixel_rgb_t bg_color, int num_stars) {
    for (int i = 0; i < RASTER_WIDTH; i++) {
        for (int j = 0; j < RASTER_HEIGHT; j++) {
            pixel_at_m(raster, i, j) = pixel_at_m(background, i, j);
        }
    }

    scatter_stars(raster, num_stars);
}


