// This is a comment. The filename is completely unused by C++ code, and is for organizationl purposes
// I.e. it doesn't matter if one file contains 5 states or if each of the 5 states are in its own file.

name: player_idle // the name of the GState -- this must be unique (otherwise only the first instance is loaded)
frames: 1
image: rabbit_idle.png // this needs to be in data/images folder
shader: t2 // ignore the .xx.glsl extension (it is assumed, for xxx (in this case, "textured"), that there is both xxx.fs.glsl and xxx.vs.glsl)
// I.e. there needs to be matching vs and fs files for this given name "textured"
// I.e. GUnit doesn't support automatic linking of say "salmon.vs.glsl" with "coloured.fs.glsl"


// Character 2
name: spartan_idle_spear
frames: 1
image: spartan_idle_spear.png
shader: t2

name: spartan_running_spear
frames: 4
image: spartan_running_spear.png
shader: t2

name: spartan_idle_nospear
frames: 1
image: spartan_idle_nospear.png
shader: t2

name: spartan_running_nospear
frames: 4
image: spartan_running_nospear.png
shader: t2

name: spartan_spear_throw
frames: 1
image: spartan_spear_throw.png
shader: t2

name: spartan_rotating
frames: 8
image: spartan_rotating.png
shader: t2

// Character rabbit running animation
name: rabbit_running
frames: 2
image: rabbit_running.png
shader: t2

// Character rabbit jumping
name: rabbit_jumping
frames: 1
image: rabbit_jumping.png
shader: t2

// Character rabbit falling
name: rabbit_falling
frames: 1
image: rabbit_falling.png
shader: t2

// Character 3
name: wheel_idle
frames: 1
image: wheel.png
shader: t2

// Spinning wheel
name: wheel_moving
frames: 6
image: wheel_moving.png
shader: t2

// Boosted wheel
name: boosted_wheel
frames: 1
image: boosted_wheel.png
shader: t2

// Transitions
name: to_wheel
frames: 4
image: blob.png
shader: t2

name: to_bunny
frames: 4
image: blob.png
shader: t2

name: to_spartan
frames: 4
image: blob.png
shader: t2
