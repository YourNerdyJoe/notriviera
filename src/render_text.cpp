#include <stdio.h>

int renderTextBounded(SDL_Renderer* renderer, SDL_Texture* font, int font_size, ivec2 pos, int w, const char* text) {
	int curr_x = pos.x;
	int curr_y = pos.y;
	int max_width = 0;
	while(*text) {
		if(*text == '\n') {
			curr_x = pos.x;
			curr_y += font_size;
			text++;
		}
		else if(w > 0 && *text == ' ') {
			while(*text && *text == ' ') {
				curr_x += font_size;
				text++;
			}
			const char* word = text;
			int word_width = 0;
			while(*word && *word != ' ' && *word != '\n') {
				word_width += font_size;
				word++;
			}
			if(word_width + curr_x - pos.x > w) {
				curr_x = pos.x;
				curr_y += font_size;
			}
		}
		else {
			SDL_FRect ch_rect = {(f32)((*text%16)*8), (f32)((*text/16)*8), 8.f, 8.f};
			SDL_FRect dst_rect = {(f32)curr_x, (f32)curr_y, (f32)font_size, (f32)font_size};
			SDL_RenderTexture(renderer, font, &ch_rect, &dst_rect);
			curr_x += font_size;
			if(curr_x > max_width) max_width = curr_x;
			text++;
		}
	}
	return max_width - pos.x;
}

int renderText(SDL_Renderer* renderer, SDL_Texture* font, int font_size, ivec2 pos, const char* text) {
	return renderTextBounded(renderer, font, font_size, pos, 0, text);
}

int renderTextFmtBounded(SDL_Renderer* renderer, SDL_Texture* font, int font_size, ivec2 pos, int w, const char* fmt, ...) {
	va_list ap;
	char buffer[512];
	va_start(ap, fmt);
	vsnprintf(buffer, 512, fmt, ap);
	int max_width = renderTextBounded(renderer, font, font_size, pos, w, buffer);
	va_end(ap);
	return max_width;
}

int renderTextFmt(SDL_Renderer* renderer, SDL_Texture* font, int font_size, ivec2 pos, const char* fmt, ...) {
	va_list ap;
	char buffer[512];
	va_start(ap, fmt);
	vsnprintf(buffer, 512, fmt, ap);
	int max_width = renderText(renderer, font, font_size, pos, buffer);
	va_end(ap);
	return max_width;
}
