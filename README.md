# OrbitShift

## Overview

This project is a simulation of cosmic bodies and their interactions.
It includes features such as rendering stars, planets, and handling
spaceship movements.


## Features

- Rendering of stars and planets
- Spaceship movement and smoke trail effects
- Spaceship collision detection
- Hyperspace jump effect

## Installation

Instructions on how to install the project.

## Usage

Instructions on how to use the simulation and interact with the spaceship.

## Core Functions

### Rendering
- `draw_background_stars()`: Renders stars with different speeds and sizes in the background
- `render_planets()`: Renders planets with different sizes and speeds that orbit around the sun elliptically
- `draw_rocket()`: Renders the spaceship with a smoke trail effect and appropriate rotation
- `render_stars_with_shaking()`: Renders hyperspace jump effect with shaking stars

### Physics
- `update_ellipsoid_path_point()`: Updates the position of a planet in an elliptical path
- `move_stars()`: Moves stars in the background with different speeds
- `move_rocket()`: Moves the spaceship with building up speed and smoke trail effect

### Utilities
- `generate_galaxy_texture()`: Generates a perlin noise texture with give color tint for the galaxy background
- `render_proximity_vignette()`: Renders a vignette effect depending on the proximity of the spaceship to a sun or a black hole
- `custom_rafgl_raster_draw_spritesheet()`: Renders a sprite sheet by exchanging a chosen color of the sprite with the given color
- `apply_distortion()`: Applies a distortion effect to the screen
- `apply_whiteout()`: Applies a gradual whiteout effect to the screen

For detailed descriptions of all functions, see [Function Documentation](docs/functions.md).
