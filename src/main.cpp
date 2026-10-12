#include <unordered_map>
#include <ctype.h>

const char* local_strings[MAX_STR_INDEX];

static const std::unordered_map<std::string, u32> local_string_idx = {
#define LOCAL_STR(x) { #x, STR_##x },
	LOCAL_STRINGS
#undef LOCAL_STR
};

int main(int argc, char** argv[]) {
	SDL_Init(SDL_INIT_VIDEO);
	SDL_Window* window = SDL_CreateWindow("game", 1280, 720, SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, "opengl");
	{
		int driver_count = SDL_GetNumRenderDrivers();
		for(int i = 0; i < driver_count; i++) {
			printf("%s\n", SDL_GetRenderDriver(i));
		}
		printf("\n%s\n\n", SDL_GetRendererName(renderer));
	}
	{
		int w, h;
		SDL_GetWindowSize(window, &w, &h);
		printf("%dx%d\n", w, h);
		SDL_GetWindowSizeInPixels(window, &w, &h);
		printf("%dx%d\n", w, h);
	}
	SDL_SetRenderLogicalPresentation(renderer, 640, 360, SDL_LOGICAL_PRESENTATION_LETTERBOX);

	GameState gs = game_init();

	Textures textures = load_textures(renderer);

	{
		FILE* f = fopen("res/en.txt", "r");
		assert(f != nullptr);
		fseek(f, 0, SEEK_END);
		long len = ftell(f);
		fseek(f, 0, SEEK_SET);

		local_strings[0] = "<STR_NULL>";
		for(u32 i = 1; i < MAX_STR_INDEX; i++) {
			local_strings[i] = "<Missing String>";
		}

		char* buffer = new char[len + 1];
		size_t read_count = fread(buffer, 1, len, f);
		buffer[read_count] = '\0';

		char* ch = buffer;
		while(*ch && ch < buffer + read_count) {
			while(isspace(*ch)) ch++;
			if(!*ch) break;
			char* id = ch;
			while(*ch && !isspace(*ch)) ch++;
			if(!*ch) break;
			*ch++ = '\0';

			while(isspace(*ch)) ch++;
			if(!*ch) break;
			char* val = ch;
			while(*ch
			&& !(ch[0] == '\n' && ch[1] == '\n')
			&& !(ch[0] == '\n' && ch[1] == '\0')) {
				if(*ch == '\\') *ch = ' ';
				ch++;
			}
			*ch++ = '\0';

			u32 idx = local_string_idx.at(id);
			local_strings[idx] = val;
		}
	}

	SDL_Event ev;
	bool is_running = true;
	timer_init_frame_limiter();
	while(is_running) {
		while(SDL_PollEvent(&ev)) {
			switch(ev.type) {
			case SDL_EVENT_QUIT:
				is_running = false;
				break;
			}
		}

		update(gs, poll_button_state());

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

		render(gs, renderer, textures);

		SDL_RenderPresent(renderer);
		timer_end_frame();
	}
	SDL_Quit();
	return 0;
}
