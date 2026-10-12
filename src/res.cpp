
SDL_Texture* load_texture(SDL_Renderer* renderer, const char* filename) {
	int width = 0, height = 0, chan = 0;
	void* buffer = stbi_load(filename, &width, &height, &chan, 4);
	if(buffer) {
		SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STATIC, width, height);
		SDL_UpdateTexture(texture, NULL, buffer, width * 4);
		SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
		SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);
		stbi_image_free(buffer);
		return texture;
	}
	else {
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Missing file", filename, NULL);
		return NULL;
	}
}

struct Textures {
	SDL_Texture* font;
	SDL_Texture* cursor;
};

Textures load_textures(SDL_Renderer* renderer) {
	return Textures{
		.font = load_texture(renderer, "res/font_dark.png"),
		.cursor = load_texture(renderer, "res/cursor.png"),
	};
}
