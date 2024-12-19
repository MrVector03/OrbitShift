#include <cosmic_bodies.h>
#include <rafgl.h>
#include <game_constants.h>
#include <time.h>
#include <utility.h>

// CONSTANTS
rafgl_pixel_rgb_t sun_color = { {214, 75, 15} };
const double sun_surface_noise_factor = 0.01;
rafgl_pixel_rgb_t sky_color = { {3, 4, 15} };

static spaceship* rocket;

/// BACKGROUND STARS
background_star_t closest_stars[1000];
int closest_stars_count = 0;
background_star_t middle_stars[1000];
int middle_stars_count = 0;
background_star_t farthest_stars[1000];
int farthest_stars_count = 0;

int show_smoke;

smoke_particle_t smoke_particles[MAX_SMOKE_PARTICLES];
int active_smoke_particles = 0;

star_t hyperdrive_stars[MAX_STARS];


void init_stars() {
    for (int i = 0; i < MAX_STARS; i++) {
        hyperdrive_stars[i].x = (randf() - 0.5f) * 2.0f * RASTER_WIDTH;
        hyperdrive_stars[i].y = (randf() - 0.5f) * 2.0f * RASTER_HEIGHT;
        hyperdrive_stars[i].z = randf() * MAX_STARS_Z;
    }
}

void update_stars(int speed) {
    for (int i = 0; i < MAX_STARS; i++) {
        hyperdrive_stars[i].z -= speed;
        if (hyperdrive_stars[i].z <= 1) {
            hyperdrive_stars[i].z += MAX_STARS_Z;
            hyperdrive_stars[i].x = randf() * MAX_STARS_Z - MAX_STARS_Z / 2;
            hyperdrive_stars[i].y = randf() * MAX_STARS_Z - MAX_STARS_Z / 2;
        }
    }
}

void render_stars(rafgl_raster_t raster, int speed) {
    int brightness;
    float sx0, sy0, sx1, sy1;

    for (int i = 0; i < MAX_STARS; i++) {
        sx1 = RASTER_WIDTH / 2 + hyperdrive_stars[i].x * (0.5 * RASTER_WIDTH / hyperdrive_stars[i].z);
        sy1 = RASTER_HEIGHT / 2 + hyperdrive_stars[i].y * (0.5 * RASTER_HEIGHT / hyperdrive_stars[i].z);

        sx0 = RASTER_WIDTH / 2 + hyperdrive_stars[i].x * (0.5 * RASTER_WIDTH / (hyperdrive_stars[i].z + speed));
        sy0 = RASTER_HEIGHT / 2 + hyperdrive_stars[i].y * (0.5 * RASTER_HEIGHT / (hyperdrive_stars[i].z + speed));

        brightness = 255 - ((float)hyperdrive_stars[i].z / MAX_STARS_Z) * 255.0f;

        rafgl_raster_draw_line(&raster, sx1, sy1, sx0, sy0, rafgl_RGB(brightness, brightness, brightness));
    }
}

void move_background_stars() {
    //printf("COUNT CLOSEST: %d\n", closest_stars_count);
    //printf("COUNT MIDDLE: %d\n", middle_stars_count);
    //printf("COUNT FARTHEST: %d\n", farthest_stars_count);
    for (int i = 0; i < closest_stars_count; i++) {
        closest_stars[i].y += CLOSEST_STAR_SPEED;
        closest_stars[i].x += CLOSEST_STAR_SPEED / 2;
        if (closest_stars[i].y >= RASTER_HEIGHT) {
            closest_stars[i].y = 0;
        }
        if (closest_stars[i].x >= RASTER_WIDTH) {
            closest_stars[i].x = 0;
        }
    }
    //("HERE 1\n");
    for (int i = 0; i < middle_stars_count; i++) {
        middle_stars[i].y += MIDDLE_STAR_SPEED;
        middle_stars[i].x += CLOSEST_STAR_SPEED / 2;
        if (middle_stars[i].y >= RASTER_HEIGHT) {
            middle_stars[i].y = 0;
        }
        if (middle_stars[i].x >= RASTER_WIDTH) {
            middle_stars[i].x = 0;
        }
    }
    //printf("HERE 2\n");
    for (int i = 0; i < farthest_stars_count; i++) {
        farthest_stars[i].y += FARTHEST_STAR_SPEED;
        farthest_stars[i].x += CLOSEST_STAR_SPEED / 2;
        if (farthest_stars[i].y >= RASTER_HEIGHT) {
            farthest_stars[i].y = 0;
        }
        if (farthest_stars[i].x >= RASTER_WIDTH) {
            farthest_stars[i].x = 0;
        }
    }
    //printf("HERE 3\n");
}

void link_rocket(spaceship* ship, int smoke_effects) {
    rocket = ship;
    show_smoke = smoke_effects;
}

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

float vignette_strength(float distance_to_sun, float max_effect_distance, float max_intensity) {
    if (distance_to_sun > max_effect_distance) {
        return 0.0f; // No vignette effect outside the range
    }
    return max_intensity * (1.0f - (distance_to_sun / max_effect_distance));
}

void render_planets(rafgl_raster_t raster, rafgl_spritesheet_t black_hole_spritesheet,solar_system_t *solar_system) {
    for (int planet_id = 0; planet_id < solar_system->num_bodies; planet_id++) {
        cosmic_body_t *planet = &solar_system->planets[planet_id];
        if (planet->is_center) {
            draw_realistic_sun(raster, (int)planet->current_x, (int)planet->current_y, planet->radius);
        } else {
            int top_left_x = planet->current_x - planet->radius;
            int top_left_y = planet->current_y - planet->radius;
            for (int i = 0; i < solar_system->planets[planet_id].radius * 2 + 20; i++) {
                for (int j = 0; j < solar_system->planets[planet_id].radius * 2 + 20; j++) {
                    int dx = top_left_x + i;
                    int dy = top_left_y + j;
                    if (dx >= 0 && dx < raster.width && dy >= 0 && dy < raster.height && rafgl_distance2D((float)dx, (float)dy, planet->current_x, planet->current_y) < planet->radius) {
                        pixel_at_m(raster, dx, dy) = pixel_at_m(planet->texture, i, j);
                    }
                }
            }
        }
    }

    rafgl_raster_draw_spritesheet(&raster, &black_hole_spritesheet,
        solar_system->black_hole.bh_curr_frame_x,
        solar_system->black_hole.bh_curr_frame_y,
            (int) solar_system->black_hole.current_x,
            (int) solar_system->black_hole.current_y);

    solar_system->black_hole.bh_curr_frame_x = (solar_system->black_hole.bh_curr_frame_x + 1) % 8;
    solar_system->black_hole.bh_curr_frame_y = (solar_system->black_hole.bh_curr_frame_y + 1) % 8;

    for (int i = 0; i < solar_system->num_bodies; i++) {
        if (!solar_system->planets[i].is_center) {
            update_ellipsoid_path_point(&solar_system->planets[i].current_x, &solar_system->planets[i].current_y,
                solar_system->planets[i].orbit_center_x, solar_system->planets[i].orbit_center_y,
                solar_system->planets[i].orbit_radius_x, solar_system->planets[i].orbit_radius_y,
                &solar_system->planets[i].theta, (rand() % i+1) * 0.1, solar_system->planets[i].orbit_speed, solar_system->planets[i].orbit_direction);
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

void render_background_star(rafgl_raster_t raster, background_star_t star) {

    //printf("PRINTING STAR\n");

    /// TODO: DECIDE ON STAR COLORS
    rafgl_pixel_rgb_t star_color = {255, 255, 255};

    if (star.layer == 0) {
        star_color = (rafgl_pixel_rgb_t){255, 255, 255};
    } else if (star.layer == 1) {
        star_color = (rafgl_pixel_rgb_t){200, 200, 200};
    } else {
        star_color = (rafgl_pixel_rgb_t){150, 150, 150};
    }

    if (star.layer == 0) {
        for (int xi = star.x; xi < star.x + CLOSEST_STAR_SIZE; xi++) {
            for (int yi = star.y; yi < star.y + CLOSEST_STAR_SIZE; yi++) {
                if (xi >= RASTER_WIDTH || yi >= RASTER_HEIGHT) {
                    continue;
                }
                pixel_at_m(raster, xi, yi) = star_color;
            }
        }
    } else if (star.layer == 1) {
        for (int xi = star.x; xi < star.x + MIDDLE_STAR_SIZE; xi++) {
            for (int yi = star.y; yi < star.y + MIDDLE_STAR_SIZE; yi++) {
                if (xi >= RASTER_WIDTH || yi >= RASTER_HEIGHT) {
                    continue;
                }
                pixel_at_m(raster, xi, yi) = star_color;
            }
        }
    } else {
        for (int xi = star.x; xi < star.x + FARTHEST_STAR_SIZE; xi++) {
            for (int yi = star.y; yi < star.y + FARTHEST_STAR_SIZE; yi++) {
                if (xi >= RASTER_WIDTH || yi >= RASTER_HEIGHT) {
                    continue;
                }
                pixel_at_m(raster, xi, yi) = star_color;
            }
        }
    }
    //printf("FINISHED PRINTING STAR\n");
}

void draw_background_stars(rafgl_raster_t raster) {
    for (int i = 0; i < closest_stars_count; i++) {
        render_background_star(raster, closest_stars[i]);
    }
    for (int i = 0; i < middle_stars_count; i++) {
        render_background_star(raster, middle_stars[i]);
    }
    for (int i = 0; i < farthest_stars_count; i++) {
        render_background_star(raster, farthest_stars[i]);
    }
}


void scatter_stars(rafgl_raster_t raster, int num_stars, int layer) {
    for (int i = 0; i < num_stars; i++) {
        int x = rand() % RASTER_WIDTH;
        int y = rand() % RASTER_HEIGHT;
        rafgl_pixel_rgb_t star_color = {255, 255, 255};

        background_star_t star = {x, y, layer};
        //printf("BEFORE\n");
        if (layer == 0) {
            closest_stars[closest_stars_count++] = star;
        } else if (layer == 1) {
            middle_stars[middle_stars_count++] = star;
        } else {
            farthest_stars[farthest_stars_count++] = star;
        }
        //printf("AFTER\n");

        render_background_star(raster, star);
    }
}

void set_corner_coords(int corner_type, cosmic_body_t *body) {

    int general_diff = RASTER_HEIGHT / 8;

    switch (corner_type) {
        case 1:
            body->current_x = general_diff;
            body->current_y = general_diff;
            break;
        case 2:
            body->current_x = RASTER_WIDTH - general_diff;
            body->current_y = general_diff;
            break;
        case 3:
            body->current_x = general_diff;
            body->current_y = RASTER_HEIGHT - general_diff;
            break;
        case 4:
            body->current_x = RASTER_WIDTH - general_diff;
            body->current_y = RASTER_HEIGHT - general_diff;
            break;
    }
}

solar_system_t generate_solar_system(int num_planets, int sun_radius, int sun_x, int sun_y) {
    int curr_orbit_radius_x = 200;
    int curr_orbit_radius_y = 100;

    solar_system_t solar_system;
    cosmic_body_t sun = {sun_x, sun_y, sun_radius, 1,
        0, 0, 0,
        sun_x, sun_y, curr_orbit_radius_x, curr_orbit_radius_y};
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
        planet.is_black_hole = 0;
        planet.texture = generate_perlin_with_color(3, 0.7);

        planet.orbit_speed = ((rand() % 100) / 1000.0) * (1.0 / i);
        planet.orbit_direction = ((rand() + i) % 2) ? 1 : -1;
        planet.theta = 0.0;

        solar_system.planets[i] = planet;

        curr_orbit_radius_x += rand() % 100 + planet.radius + 25;
        curr_orbit_radius_y += rand() % 50 + planet.radius + 15;
    }

    cosmic_body_t black_hole;
    black_hole.is_black_hole = 1;
    black_hole.radius = 64;
    black_hole.bh_curr_frame_x = 0;
    black_hole.bh_curr_frame_y = 0;

    int corner_type = rand() % 4 + 1;
    black_hole.black_hole_corner = corner_type;

    set_corner_coords(corner_type, &black_hole);
    solar_system.black_hole = black_hole;

    solar_system.next_system_color = (rafgl_pixel_rgb_t){rand() % 255, rand() % 255, rand() % 255};

    for (int i = 0; i < active_smoke_particles; i++) {
        smoke_particles[i] = (smoke_particle_t){0, 0, 0, 0};
    }

    return solar_system;
}

void set_background(rafgl_raster_t raster, rafgl_raster_t background, rafgl_pixel_rgb_t bg_color) {
    for (int i = 0; i < RASTER_WIDTH; i++) {
        for (int j = 0; j < RASTER_HEIGHT; j++) {
            rafgl_pixel_rgb_t sampled = pixel_at_m(background, i, j);
            rafgl_pixel_rgb_t result;
            result.r = rafgl_saturatei(sampled.r + bg_color.r);
            result.g = rafgl_saturatei(sampled.g + bg_color.g);
            result.b = rafgl_saturatei(sampled.b + bg_color.b);
            pixel_at_m(raster, i, j) = pixel_at_m(background, i, j);
        }
    }
}

void add_stars_to_background(rafgl_raster_t background_raster, int new_stars) {
    if (new_stars) {
        closest_stars_count = 0;
        middle_stars_count = 0;
        farthest_stars_count = 0;

        scatter_stars(background_raster, CLOSEST_STAR_COUNT, 0);
        scatter_stars(background_raster, MIDDLE_STAR_COUNT, 1);
        scatter_stars(background_raster, FARTHEST_STAR_COUNT, 2);
    } else {
        for (int i = 0; i < closest_stars_count; i++) {
            render_background_star(background_raster, closest_stars[i]);
        }
        for (int i = 0; i < middle_stars_count; i++) {
            render_background_star(background_raster, middle_stars[i]);
        }
        for (int i = 0; i < farthest_stars_count; i++) {
            render_background_star(background_raster, farthest_stars[i]);
        }
    }
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

void spawn_smoke_particle(int x, int y, float lifespan, int frame) {
    if (active_smoke_particles < MAX_SMOKE_PARTICLES) {
        smoke_particles[active_smoke_particles++] = (smoke_particle_t){x, y, lifespan, frame};
    }
}

void update_smoke_particles(float delta_time) {
    for (int i = 0; i < active_smoke_particles; i++) {
        smoke_particles[i].smoke_lifespan -= delta_time;
        if (smoke_particles[i].smoke_lifespan <= 0) {
            smoke_particles[i] = smoke_particles[--active_smoke_particles];
            i--;
        }

        smoke_particles[i].pos_x += rand() % 5 - 2;
        smoke_particles[i].pos_y += rand() % 5 - 2;
    }
}

void draw_particles(rafgl_raster_t raster, rafgl_spritesheet_t spritesheet) {
    for (int i = 0; i < active_smoke_particles; i++) {
        smoke_particle_t *particle = &smoke_particles[i];
        rafgl_raster_draw_spritesheet(&raster, &spritesheet, particle->frame, 1, particle->pos_x, particle->pos_y);
    }
}

void draw_rocket(rafgl_raster_t raster, spaceship *ship, rafgl_spritesheet_t smoke_spritesheet, float delta_time, int moved) {
    int width = raster.width;
    int height = raster.height;

    double size = 10.0;
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

    if (show_smoke) {
        /// Smoke trail
        int exhaust_x = ship->curr_x - size * cos(ship->angle) + rand() % 10 - 5;
        int exhaust_y = ship->curr_y - size * sin(ship->angle) + rand() % 10 - 5;


        //printf("SHEET WIDTH %d\n", smoke_spritesheet.sheet_width);
        //int frame = (ship->trail_timer * 10) % (smoke_spritesheet.sheet_width / SMOKE_SPRITE_WIDTH);
        //rafgl_raster_draw_spritesheet(&raster, &smoke_spritesheet, ship->curr_particle, 1, exhaust_x, exhaust_y);
        //printf("AFTER PRINT\n");

        if (moved) {
            int frame = rand() % 6;
            spawn_smoke_particle(exhaust_x, exhaust_y, 5.0, frame);
        }

        update_smoke_particles(delta_time);
        draw_particles(raster, smoke_spritesheet);
    }
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

    if (show_smoke) {
        ship->trail_timer += delta_time;
        ship->curr_particle = (ship->curr_particle + 1) % 6;
    }
}

spaceship init_spaceship(cosmic_body_t black_hole, float angle, float speed, int trail_timer) {
    spaceship ship;

    int black_hole_x = black_hole.current_x;
    int black_hole_y = black_hole.current_y;

    /// get sum other corner
    if (black_hole.black_hole_corner == 1) {
        black_hole_x = RASTER_WIDTH - black_hole_x;
        black_hole_y = RASTER_HEIGHT - black_hole_y;
    } else if (black_hole.black_hole_corner == 2) {
        black_hole_y = RASTER_HEIGHT - black_hole_y;
    } else if (black_hole.black_hole_corner == 3) {
        black_hole_x = RASTER_WIDTH - black_hole_x;
    }

    ship.curr_x = black_hole_x;
    ship.curr_y = black_hole_y;
    ship.angle = angle;
    ship.speed = speed;
    ship.trail_timer = trail_timer;
    ship.curr_particle = 0;
    return ship;
}

solar_system_t generate_next_solar_system(rafgl_pixel_rgb_t system_color) {
    int num_planets = rand() % 3;
    int sun_radius = RASTER_HEIGHT / (40 + rand() % 10);
    sun_color = system_color;
    return generate_solar_system(num_planets, sun_radius, RASTER_WIDTH / 2, RASTER_HEIGHT / 2);
}

void stabilize_rocket(spaceship *rocket, cosmic_body_t black_hole) {
    // int black_hole_x = black_hole.current_x;
    // int black_hole_y = black_hole.current_y;
    //
    // if (black_hole.black_hole_corner == 1) {
    //     black_hole_x = RASTER_WIDTH - black_hole_x;
    //     black_hole_y = RASTER_HEIGHT - black_hole_y;
    // } else if (black_hole.black_hole_corner == 2) {
    //     black_hole_y = RASTER_HEIGHT - black_hole_y;
    // } else if (black_hole.black_hole_corner == 3) {
    //     black_hole_x = RASTER_WIDTH - black_hole_x;
    // }
    //
    // rocket->curr_x = black_hole_x;
    // rocket->curr_y = black_hole_y;

    rocket->speed = 0;
}
