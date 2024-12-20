#ifndef COSMIC_BODIES_H
#define COSMIC_BODIES_H

#include "rafgl.h"
#include "game_constants.h"
#include <math.h>



typedef struct {
    float current_x;
    float current_y;
    int radius;

    int is_center;

    rafgl_raster_t texture;

    /// ORBITAL PARAMETERS
    int orbit_center_x;
    int orbit_center_y;
    double orbit_radius_x;
    double orbit_radius_y;
    int initial_x;
    int initial_y;

    double orbit_speed;
    int orbit_direction; /// 1 - clockwise, -1 - counterclockwise
    float theta;

    /// BLACK HOLE
    int is_black_hole;
    int black_hole_corner; /// 1 - top left, 2 - top right, 3 - bottom left, 4 - bottom right
    int bh_curr_frame_x;
    int bh_curr_frame_y;
} cosmic_body_t;

typedef struct {
    cosmic_body_t sun;
    cosmic_body_t planets[32];
    cosmic_body_t black_hole;
    int num_bodies;
    rafgl_pixel_rgb_t next_system_color;
} solar_system_t;

typedef struct {
    float curr_x;
    float curr_y;
    double angle;
    double speed;
    int trail_timer;
    int curr_particle;
} spaceship;

typedef struct {
    int pos_x, pos_y;
    double smoke_lifespan;
    int frame;
} smoke_particle_t;

typedef struct {
    float x, y, z;
    float speed;
} star_t;

typedef struct {
    float x;
    float y;
    int layer; /// 0 - closest, 1 - middle, 2 - farthest
} background_star_t;

extern rafgl_pixel_rgb_t sun_color;

extern const double sun_surface_noise_factor;

extern rafgl_pixel_rgb_t sky_color;

void draw_realistic_sun(rafgl_raster_t raster, int x, int y, int radius);

void draw_realistic_sun_with_texture(rafgl_raster_t raster, int x, int y, int radius, rafgl_raster_t sun_texture, double smooth_factor);

void scatter_stars(rafgl_raster_t raster, int num_stars, int layer);

solar_system_t generate_solar_system(int num_planets, int sun_radius, int sun_x, int sun_y);

void set_background(rafgl_raster_t raster, rafgl_raster_t background, rafgl_pixel_rgb_t bg_color);

void render_planets(rafgl_raster_t raster, rafgl_spritesheet_t black_hole_spritesheet, solar_system_t *solar_system);

void draw_rocket(rafgl_raster_t raster, spaceship *ship, rafgl_spritesheet_t smoke_spritesheet, float delta_time, int moved);

void move_rocket(spaceship *ship, float thrust, float angle_control, float delta_time);

spaceship init_spaceship(cosmic_body_t black_hole, float angle, float speed, int trail_timer);

void link_rocket(spaceship* ship, int smoke_effects);

void apply_vignette_with_tint(rafgl_raster_t raster, rafgl_pixel_rgb_t tint_color);

solar_system_t generate_next_solar_system(rafgl_pixel_rgb_t system_color);

void stabilize_rocket(spaceship *ship, cosmic_body_t black_hole);

void init_stars();

void render_stars(rafgl_raster_t *raster, int width, int height);

void update_stars(float delta_time, int width, int height);

void move_background_stars();

void draw_background_stars(rafgl_raster_t raster);

void add_stars_to_background(rafgl_raster_t background_raster, int new_stars);

void render_stars_with_shaking(rafgl_raster_t *raster, int width, int height, float delta_time, rafgl_pixel_rgb_t next_system_color, int ending);

void draw_hyperspeed_rocket(rafgl_raster_t *raster, int width, int height, float delta_time);

void handle_rocket_out_of_bounds(rafgl_raster_t raster, spaceship *rocket, rafgl_spritesheet_t arrows_spritesheet, int rocket_diff_x, int rocket_diff_y);

#endif //COSMIC_BODIES_H


