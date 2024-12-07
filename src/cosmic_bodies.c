#include <cosmic_bodies.h>
#include <rafgl.h>
#include <game_constants.h>
#include <time.h>
#include <utility.h>

// CONSTANTS
const rafgl_pixel_rgb_t sun_color = { {214, 75, 15} };
const double sun_surface_noise_factor = 0.01;
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
                pixel_at_m(raster, i, j) = sun_color;
        }
    }
}

void render_planets(rafgl_raster_t raster, solar_system_t *solar_system) {
    for (int i = 0; i < raster.width; i++) {
        for (int j = 0; j < raster.height; j++) {
            for (int k = 0; k < solar_system->num_bodies; k++) {
                cosmic_body_t *planet = &solar_system->planets[k];
                float dx = i - planet->current_x;
                float dy = j - planet->current_y;
                float distance = sqrt(dx * dx + dy * dy);
                double noise = (rand() % 100) / 100.0 * sun_surface_noise_factor;


                if (planet->is_center) {
                    if (distance < planet->radius * (1 + noise))
                        pixel_at_m(raster, i, j) = sun_color;
                } else if (distance < planet->radius) {
                    pixel_at_m(raster, i, j) = pixel_at_m(planet->texture, planet->initial_x, planet->initial_y);
                }
            }
        }
    }
    for (int i = 0; i < solar_system->num_bodies; i++) {
        if (!solar_system->planets[i].is_center) {
            update_ellipsoid_path_point(&solar_system->planets[i].current_x, &solar_system->planets[i].current_y,
                solar_system->planets[i].orbit_center_x, solar_system->planets[i].orbit_center_y,
                solar_system->planets[i].orbit_radius_x, solar_system->planets[i].orbit_radius_y,
                &solar_system->planets[i].theta, 0.01, solar_system->planets[i].orbit_speed, solar_system->planets[i].orbit_direction);
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

            double current_noise = (rand() % 100) / 100.0 * sun_surface_noise_factor;

            double smooth_noise_value = smooth_noise(current_noise, smooth_factor);

            previous_noise = smooth_noise_value;

            if (distance < radius * (1 + smooth_noise_value)) {
                rafgl_pixel_rgb_t sun_color = map_noise_to_sun_color(smooth_noise_value);

                int tex_x = (int)((dx / radius + 1) * 0.5 * texture_width);
                int tex_y = (int)((dy / radius + 1) * 0.5 * texture_height);

                tex_x = (tex_x < 0) ? 0 : (tex_x >= texture_width) ? texture_width - 1 : tex_x;
                tex_y = (tex_y < 0) ? 0 : (tex_y >= texture_height) ? texture_height - 1 : tex_y;

                rafgl_pixel_rgb_t texture_color = pixel_at_m(sun_texture, tex_x, tex_y);
                sun_color.r = (sun_color.r + texture_color.r) / 2; // Blend the colors
                sun_color.g = (sun_color.g + texture_color.g) / 2;
                sun_color.b = (sun_color.b + texture_color.b) / 2;

                pixel_at_m(raster, i, j) = sun_color;
            } else {
                pixel_at_m(raster, i, j) = sky_color;
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
    cosmic_body_t sun = {sun_x, sun_y, sun_radius, 1, sun_texture, sun_x, sun_y, curr_orbit_radius_x, curr_orbit_radius_y};
    solar_system.sun = sun;
    solar_system.num_bodies = num_planets + 1;
    solar_system.planets[0] = sun;

    for (int i = 1; i <= num_planets; i++) {
        cosmic_body_t planet;
        planet.orbit_center_x = sun_x;
        planet.orbit_center_y = sun_y;

        planet.orbit_radius_x = curr_orbit_radius_x;
        planet.orbit_radius_y = curr_orbit_radius_y;

        planet.current_x = sun_x;
        planet.current_y = sun_y + curr_orbit_radius_y;

        planet.initial_x = sun_x;
        planet.initial_y = sun_y + curr_orbit_radius_y;

        planet.radius = rand() % 20 + 10;
        planet.is_center = 0;
        planet.texture = generate_perlin_with_color(8, 0.7);

        printf("RAND: %d\n", rand());

        planet.orbit_speed = (rand() * i) % 10 + 3;
        planet.orbit_direction = ((rand() + i) % 2) ? 1 : -1;
        planet.theta = 0.0;

        solar_system.planets[i] = planet;

        curr_orbit_radius_x += rand() % 100 + planet.radius + 25;
        curr_orbit_radius_y += rand() % 50 + planet.radius + 15;
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

void fill_triangle(rafgl_raster_t raster, int x1, int y1, int x2, int y2, int x3, int y3, rafgl_pixel_rgb_t *color) {
    if (y1 > y2) {
        int tmp = y1; y1 = y2; y2 = tmp;
        tmp = x1; x1 = x2; x2 = tmp;
    }

    if (y2 > y3) {
        int tmp = y2; y2 = y3; y3 = tmp;
        tmp = x2; x2 = x3; x3 = tmp;
    }

    if (y1 > y2) {
        int tmp = y1; y1 = y2; y2 = tmp;
        tmp = x1; x1 = x2; x2 = tmp;
    }

    float m1 = (float)(x2 - x1) / (y2 - y1);
    float m2 = (float)(x3 - x1) / (y3 - y1);
    float m3 = (float)(x3 - x2) / (y3 - y2);

    float startX1 = x1, startX2 = x1;
    for (int y = y1; y <= y2; y++) {
        rafgl_raster_draw_line(&raster, (int)startX1, y, (int)startX2, y, color);
        startX1 += m1;
        startX2 += m2;
    }

    startX1 = x2;
    startX2 = x1;
    for (int y = y2; y <= y3; y++) {
        rafgl_raster_draw_line(&raster, (int)startX1, y, (int)startX2, y, color);
        startX1 += m3;
        startX2 += m2;
    }
}


void draw_rocket(rafgl_raster_t raster, spaceship *ship) {
    int width = raster.width;
    int height = raster.height;

    double size = 20.0;
    double pointiness_factor = 2.5;
    double x1, y1, x2, y2, x3, y3;

    double front_size = size * pointiness_factor;

    x2 = ship->curr_x + front_size * cos(ship->angle);
    y2 = ship->curr_y + front_size * sin(ship->angle);

    x3 = ship->curr_x + size * cos(ship->angle + M_PI / 3);
    y3 = ship->curr_y + size * sin(ship->angle + M_PI / 3);

    x1 = ship->curr_x + size * cos(ship->angle - M_PI / 3);
    y1 = ship->curr_y + size * sin(ship->angle - M_PI / 3);

    rafgl_pixel_rgb_t rgb = {255, 255, 255};

    //fill_triangle(raster, (int)x1, (int)y1, (int)x2, (int)y2, (int)x3, (int)y3, &rgb);

    rafgl_raster_draw_line(&raster, (int)x1, (int)y1, (int)x2, (int)y2, &rgb);
    rafgl_raster_draw_line(&raster, (int)x2, (int)y2, (int)x3, (int)y3, &rgb);
    rafgl_raster_draw_line(&raster, (int)x3, (int)y3, (int)x1, (int)y1, &rgb);
}

void move_rocket(spaceship *ship, float thrust, float angle_control, float delta_time) {
    ship->curr_x += delta_time * ship->speed * cos(ship->angle);
    ship->curr_y += delta_time * ship->speed * sin(ship->angle);

    if (thrust) {
        ship->speed += thrust;
    }

    if (angle_control) {
        ship->angle += angle_control;
    }
}

spaceship init_spaceship(float x, float y, float angle, float speed) {
    spaceship ship;
    ship.curr_x = x;
    ship.curr_y = y;
    ship.angle = angle;
    ship.speed = speed;

    return ship;
}
