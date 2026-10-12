#pragma once

#define KEY_BINDINGS \
KEY_BINDING(B_LEFT,SDL_SCANCODE_LEFT) \
KEY_BINDING(B_RIGHT, SDL_SCANCODE_RIGHT) \
KEY_BINDING(B_UP, SDL_SCANCODE_UP) \
KEY_BINDING(B_DOWN, SDL_SCANCODE_DOWN) \
KEY_BINDING(B_A, SDL_SCANCODE_Z) \
KEY_BINDING(B_B, SDL_SCANCODE_X) \
KEY_BINDING(B_ESC, SDL_SCANCODE_ESCAPE) \
KEY_BINDING(B_INFO, SDL_SCANCODE_TAB)

enum {
#define KEY_BINDING(x, _) x,
KEY_BINDINGS
#undef KEY_BINDING
	KEY_BINDING_COUNT,
};

u32 poll_button_state() {
	constexpr u32 key_bindings[] = {
#define KEY_BINDING(_, x) x,
KEY_BINDINGS
#undef KEY_BINDING
	};

	const bool* keys = SDL_GetKeyboardState(NULL);
	u32 buttons = 0;
	for(u32 i = 0; i < KEY_BINDING_COUNT; i++) {
		buttons |= !!keys[key_bindings[i]] << i;
	}
	return buttons;
}

constexpr bool button_is_down(u32 buttons, int button) {
	return (buttons >> button) & 1;
}

struct VPad {
	u32 curr, prev;
	constexpr bool is_down(int button) const { return button_is_down(curr, button); }
	constexpr bool is_pressed(int button) const { return is_down(button) && !button_is_down(prev, button); }
	constexpr bool is_released(int button) const { return !is_down(button) && button_is_down(prev, button); }
};

//enum {
//	KB_NONE,
//	KB_ESCAPE,
//	KB_BACKSPACE,
//	KB_ENTER = '\n',
//};
//
//struct Input {
//	const bool* keys;
//	const char* text;
//	s32 mouse_x, mouse_y;
//	u32 mouse_buttons;
//};
//
//Input inputPollOrQuit() {
//	static char text_buffer[1024];
//	Input input = { 0 };
//	char* text_ptr = text_buffer;
//	SDL_Event ev;
//	while(SDL_PollEvent(&ev)) {
//		switch(ev.type) {
//		case SDL_EVENT_QUIT:
//			return input;
//		case SDL_EVENT_KEY_DOWN:
//			if(ev.key.key >= SDLK_SPACE && ev.key.key <= SDLK_TILDE) {
//				*text_ptr++ = ev.key.key;
//			}
//			else if(ev.key.key == SDLK_RETURN) {
//				*text_ptr++ = '\n';
//			}
//			else if(ev.key.key == SDLK_BACKSPACE) {
//				if(text_ptr != text_buffer) text_ptr--;
//			}
//		}
//	}
//	*text_ptr = '\0';
//	input.keys = SDL_GetKeyboardState(NULL);
//	input.text = text_buffer;
//	float mouse_x, mouse_y;
//	input.mouse_buttons = SDL_GetMouseState(&mouse_x, &mouse_y);
//	input.mouse_x = (s32)mouse_x;
//	input.mouse_y = (s32)mouse_y;
//	return input;
//}
