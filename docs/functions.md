# Documentation for cosmic_bodies.h

## Overview
The `cosmic_bodies.h` header file provides the definitions and function declarations necessary for simulating and rendering a solar system, black holes, rockets, and related cosmic elements. This file also includes utility functions for visual effects like fisheye lens distortion, starfield animations, and background rendering.

---

## Structures

### `cosmic_body_t`
Represents a celestial body in the solar system.

#### Fields:
- `float current_x, current_y`: Current position of the cosmic body.
- `int radius`: Radius of the cosmic body.
- `int is_center`: Indicates if the cosmic body is the solar system's center.
- `rafgl_raster_t texture`: Texture of the cosmic body.

#### Orbital Parameters:
- `int orbit_center_x, orbit_center_y`: Coordinates of the orbital center.
- `double orbit_radius_x, orbit_radius_y`: Radii of the orbital path.
- `int initial_x, initial_y`: Initial position of the body.
- `double orbit_speed`: Orbital speed of the body.
- `int orbit_direction`: Direction of the orbit (1 = clockwise, -1 = counterclockwise).
- `float theta`: Current angle in the orbit.

#### Black Hole Fields:
- `int is_black_hole`: Indicates if the body is a black hole.
- `int black_hole_corner`: Corner of the viewport where the black hole is located. (1 = top-left, 2 = top-right, 3 = bottom-left, 4 = bottom-right)
- `int bh_curr_frame_x, bh_curr_frame_y`: Current frame coordinates in the sprite sheet for black hole animation.

---

### `solar_system_t`
Represents the solar system containing various cosmic bodies.

#### Fields:
- `cosmic_body_t sun`: Sun of the solar system.
- `cosmic_body_t planets[32]`: Array of up to 32 planets.
- `cosmic_body_t black_hole`: The black hole.
- `int num_bodies`: Number of celestial bodies in the system.
- `rafgl_pixel_rgb_t next_system_color`: Tint color of the next solar system.

---

### `spaceship`
Represents a user-controlled spaceship entity.

#### Fields:
- `float curr_x, curr_y`: Current position of the spaceship.
- `double angle`: Direction of movement and rotation.
- `double speed`: Current speed of the spaceship.
- `int trail_timer`: Timer for the smoke trail effect.
- `int curr_particle`: Coordinates of the current smoke particle.

---

## Functions

### Celestial Body Functions

#### `void draw_realistic_sun(rafgl_raster_t raster, int x, int y, int radius)`
Draws the Sun on the given raster using a noise texture on the surface so that it looks more realistic and uneven.

#### `void draw_realistic_sun_with_texture(rafgl_raster_t raster, int x, int y, int radius, rafgl_raster_t sun_texture, double smooth_factor)`
Draws a Sun using a pre-defined texture.

- **Parameters:**
    - `rafgl_raster_t raster`: Target raster.
    - `rafgl_raster_t sun_texture`: Texture to apply.
    - `double smooth_factor`: Smoothness factor for blending effects.

#### `solar_system_t generate_solar_system(int num_planets, int sun_radius, int sun_x, int sun_y)`
Generates a solar system with a specified number of planets.

- **Returns:** A `solar_system_t` structure.

#### `solar_system_t generate_next_solar_system(rafgl_pixel_rgb_t system_color)`
Generates a new solar system with a specific color scheme. The system_color is applied to the sun color of the new system.

---

### Rendering Functions

#### `void scatter_stars(rafgl_raster_t raster, int num_stars, int layer)`
Renders scattered stars across the raster. The stars are placed randomly across the inserted layer.

- **Parameters:**
    - `int num_stars`: Number of stars to scatter.
    - `int layer`: Layer of depth for the stars (0 = closest, 2 = farthest).

#### `void render_planets(rafgl_raster_t raster, rafgl_spritesheet_t black_hole_spritesheet, solar_system_t *solar_system)`
Renders the planets and black hole for a given solar system.

- Sun is rendered using `draw_realistic_sun()`.
- Black hole is rendered using sprite sheet animation.
- Fisheye lens distortion is applied to the raster around the black hole.

After rendering the cosmic bodies, the function updates the positions of the planets, and finds the next sprite frame for the black hole animation.

#### `void set_background(rafgl_raster_t raster, rafgl_raster_t background, rafgl_pixel_rgb_t bg_color)`
Applies the background raster to the main raster with a specific color tint applied to the background.

Tint is applied using the `rafgl_saturatei()` function.

---

### Spaceship Functions

#### `spaceship init_spaceship(cosmic_body_t black_hole, float angle, float speed, int trail_timer)`
Initializes a spaceship entity.

- Initializes the spawn position based on the black hole's position.
- Sets the initial angle and speed of the spaceship.
- Initializes the smoke trail timer.

#### `void move_rocket(spaceship *ship, float thrust, float angle_control, float delta_time)`
Moves the spaceship based on thrust and angle control.

#### `void draw_rocket(rafgl_raster_t raster, spaceship *ship, rafgl_spritesheet_t smoke_spritesheet, float delta_time, int moved)`
Draws the spaceship on the raster represented as a triangle that's being rotated based on the spaceship's angle.

- Function `rafgl_raster_draw_line()` is used to connect the points of the triangle.
- Smoke trail effect is applied using a sprite sheet. Smoke particles are drawn behind the spaceship in chaotic patterns.

#### `void handle_rocket_out_of_bounds(rafgl_raster_t raster, spaceship *rocket, rafgl_spritesheet_t arrows_spritesheet, int rocket_diff_x, int rocket_diff_y)`
Handles scenarios where the rocket moves out of the viewport bounds.

- Draws arrows on the screen to indicate the edge of the viewport.
- Uses the `rafgl_distance2D()` function to calculate the distance between the rocket and the viewport center.
- If the rocket is going away from the center, the arrow is colored red; if it's moving towards the center, the arrow is colored green; otherwise, it's colored white.
- Uses the `rafgl_raster_draw_spritesheet_color_insteadof()` function to draw the arrows with the appropriate color.

---

### Visual Effects

#### `void render_proximity_vignette(rafgl_raster_t raster, int cx, int cy, float vignette_factor, float rocket_sun_dist, float vignette_r, float vignette_g, float vignette_b, float r)`
Renders a vignette effect based on the proximity of the spaceship to a cosmic body.

- Uses the `rafgl_saturatei()` function to appropriately saturate the vignette color using the vignette factor and the distance between the spaceship and the cosmic body.

#### `void apply_fisheye_lens(rafgl_raster_t *raster, int cx, int cy, int radius)`
Applies a fisheye lens distortion to the raster at the specified center and radius.

- This effect makes objects on the upper part of the raster appear larger and objects on the lower part appear smaller. (Simulates a black hole's gravitational lensing effect)
- Uses the `rafgl_raster_init()` function to initialize a helper raster for the distortion effect.

---

### Starfield Animation

#### `void init_stars()`
Initializes the starfield.

#### `void render_stars(rafgl_raster_t *raster, int width, int height)`
Renders the starfield on the raster.

#### `void update_stars(float delta_time, int width, int height)`
Updates the star positions based on delta time.

#### `void move_background_stars()`
Moves the background stars for a parallax effect.

#### `void add_stars_to_background(rafgl_raster_t background_raster, int new_stars)`
Adds new stars to the background raster.

---

### Hyper speed Effects

#### `void render_stars_with_shaking(rafgl_raster_t *raster, int width, int height, float delta_time, rafgl_pixel_rgb_t next_system_color, int ending)`
Renders stars with shaking for a hyper speed effect.

#### `void draw_hyperspeed_rocket(rafgl_raster_t *raster, int width, int height, float delta_time)`
Draws the rocket with hyper speed visuals.

---

## Global Variables

### Colors
- `rafgl_pixel_rgb_t sun_color`: Default color of the Sun.
- `rafgl_pixel_rgb_t sky_color`: Default color of the sky.

### Constants
- `const double sun_surface_noise_factor`: Noise factor for Sun rendering.

---

