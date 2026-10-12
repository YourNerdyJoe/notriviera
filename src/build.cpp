
#include <SDL3/SDL.h>
#include <bitset>
#include <ranges>
#include <variant>
#include <vector>

#include "gs_assert.h"
#include "types.h"
#include "vec2.h"
#include "rect.h"
#include "container.h"
#include "input.h"
#include "local_strings.h"

#define CUTE_SOUND_IMPLEMENTATION
#include "third_party/cute_sound.h"
#define STB_IMAGE_IMPLEMENTATION
#include "third_party/stb_image.h"

#include "third_party/bitset_iter.h"
#include "third_party/tinymt32.h"

#include "render_text.cpp"
#include "res.cpp"
#include "game.cpp"
#include "timer.cpp"
#include "main.cpp"
#include "third_party/tinymt32.c"
