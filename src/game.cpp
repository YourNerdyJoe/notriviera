
enum {
	ITEM_NONE,
	ITEM_APPLE,
	ITEM_SWORD,
	ITEM_SCROLL,
};

struct Item {
	const char* name;
	b32 can_stack : 1;
};

struct Items_t {
	Item none{"---"}
		, apple{"Apple", true}
		, sword{"Sword", false}
		, scroll{"Scroll", true}
	;
}constexpr Items;

using ItemHandle = GlobalHandle<const Item, &Items.none, false>;

const char* menu_string(ItemHandle item) { return item->name; }

//struct Dialogs_t;
//extern const Dialogs_t Dialogs;
//
//struct Dialog;
//using DialogHandle = GlobalHandle<const Dialog, reinterpret_cast<const Dialog*>(&Dialogs)>;
//
//struct Dialog {
//	const char* str;
//
//	/*struct ActionNone {};
//	struct ActionItem { ItemHandle item; };
//	struct ActionChoice { DialogHandle choices[2]; };
//	std::variant<ActionNone, ActionItem, ActionChoice> action;*/
//};

//TODO: strings file en.txt
//HELLO
//Hello, World!
//TODO: textbox 2 lines at a time
//struct Dialogs_t {
//	const char* hello[2] {
//		"Hello",
//		"World",
//	};
//};

//struct Entity {
//	StrHandle name;
//	StrHandle dialog;
//	ItemHandle item;
//};

constexpr u32 inventory_size = 12;
constexpr u32 room_items_size = 3;

struct Room {
	ItemHandle items[4];
};

constexpr u32 map_max_size = 16;
using RoomLinks = std::bitset<4>;

enum {
	MOVE_LEFT,
	MOVE_RIGHT,
	MOVE_UP,
	MOVE_DOWN,
};

constexpr auto move_range = std::ranges::views::iota((u32)MOVE_LEFT, (u32)MOVE_DOWN + 1);

constexpr ivec2 move_dir[] = {
	{-1, 0}, {1, 0}, {0, -1}, {0, 1},
};

static_assert(MOVE_LEFT == B_LEFT);
static_assert(MOVE_RIGHT == B_RIGHT);
static_assert(MOVE_UP == B_UP);
static_assert(MOVE_DOWN == B_DOWN);

auto& sample_array(auto& arr, tinymt32_t* random) {
	auto begin = std::begin(arr);
	auto end = std::end(arr);
	u32 select = tinymt32_generate_uint32(random);
	select %= end - begin;
	return begin[select];
}

struct Map {
private:
	uvec2 dim;
	Room rooms[map_max_size][map_max_size];
	std::bitset<map_max_size - 1> links_hori[map_max_size];
	std::bitset<map_max_size> links_vert[map_max_size - 1];

	/*constexpr RoomLinks potential_room_links(u32 x, u32 y) const {
		RoomLinks ret;
		ret[MOVE_LEFT] = x && x < dim.x;
		ret[MOVE_RIGHT] = x < dim.x - 1;
		ret[MOVE_UP] = y && y < dim.y;
		ret[MOVE_DOWN] = y < dim.y - 1;
		return ret;
	}

	constexpr RoomLinks potential_room_links(uvec2 room_pos) const {
		return potential_room_links(room_pos.x, room_pos.y);
	}*/

	constexpr void clear() {
		dim = {};
		for(auto& row : rooms) {
			for(auto& room : row) room = {};
		}
		for(auto& row : links_hori) row = {};
		for(auto& row : links_vert) row = {};
	}

	constexpr auto neighbors(uvec2 cell) const {
		DynArr<uvec2, 4> ret = {};
		if(cell.x < dim.x && cell.y < dim.y) {
			if(cell.x) ret.add() = { cell.x - 1, cell.y };
			if(cell.x < dim.x - 1) ret.add() = { cell.x + 1, cell.y };
			if(cell.y) ret.add() = { cell.x, cell.y - 1 };
			if(cell.y < dim.y - 1) ret.add() = { cell.x, cell.y + 1 };
		}
		return ret;
	}

	constexpr auto neighbors_unlinked(uvec2 cell) const {
		DynArr<uvec2, 4> ret = {};
		auto ns = neighbors(cell);
		for(auto n : ns) {
			if(room_links(n).none()) {
				ret.add() = n;
			}
		}
		return ret;
	}

	constexpr void link(uvec2 a, uvec2 b) {
		if(b.x < a.x || b.y < a.y) std::swap(a, b);
		if(a.x != b.x) links_hori[a.y][a.x] = 1;
		if(a.y != b.y) links_vert[a.y][a.x] = 1;
	}

public:
	constexpr auto& room(this auto& self, uvec2 room_pos) {
		return self.rooms[room_pos.y][room_pos.x];
	}

	constexpr RoomLinks room_links(u32 x, u32 y) const {
		RoomLinks ret;
		ret[MOVE_LEFT] = x && links_hori[y][x - 1];
		ret[MOVE_RIGHT] = x < map_max_size - 1 && links_hori[y][x];
		ret[MOVE_UP] = y && links_vert[y - 1][x];
		ret[MOVE_DOWN] = y < map_max_size - 1 && links_vert[y][x];
		return ret;
	}

	constexpr RoomLinks room_links(uvec2 room_pos) const {
		return room_links(room_pos.x, room_pos.y);
	}

	//recursive backtracker
	void generate(tinymt32_t* random, uvec2 dim, uvec2 start = {}) {
		gs_assert(dim.x < map_max_size);
		gs_assert(dim.y < map_max_size);
		this->dim = dim;

		DynArr<uvec2, map_max_size* map_max_size> stack = {};
		stack.add() = start;

		while(stack.size) {
			uvec2 curr = stack.end()[-1];
			auto curr_neighbors = neighbors_unlinked(curr);
			if(!curr_neighbors.size) {
				stack.size--;
			}
			else {
				uvec2 neighbor = sample_array(curr_neighbors, random);
				link(curr, neighbor);
				stack.add() = neighbor;
			}
		}
	}
};

struct GameState {
	tinymt32_t random;
	enum class View : u32 {
		room, inventory, move,
	};
	u32 buttons_prev;
	View view;
	View prev_view_inventory;
	u32 inventory_cursor;
	ItemHandle inventory[inventory_size];
	u32 room_cursor;
	uvec2 room_pos;
	Map map;
};

GameState game_init() {
	GameState gs = {};
	tinymt32_init(&gs.random, 0);
	gs.map.generate(&gs.random, { 4, 4 });
	return gs;
}

void move_cursor(u32& cursor, VPad pad, u32 upper_bound) {
	cursor += pad.is_pressed(B_DOWN) - pad.is_pressed(B_UP);
	if(cursor == upper_bound + 1) cursor = 0;
	else if(cursor > inventory_size) cursor = upper_bound;
}

void update(GameState& gs, u32 buttons) {
	VPad pad = { .curr = buttons, .prev = gs.buttons_prev };
	gs.buttons_prev = buttons;

	if(pad.is_pressed(B_INFO)) {
		if(gs.view == GameState::View::inventory) {
			gs.view = gs.prev_view_inventory;
		}
		else {
			gs.prev_view_inventory = gs.view;
			gs.view = GameState::View::inventory;
		}
	}
	if(pad.is_pressed(B_B)) {
		if(gs.view == GameState::View::inventory) gs.view = gs.prev_view_inventory;
		else if(gs.view == GameState::View::room) gs.view = GameState::View::move;
		else if(gs.view == GameState::View::move) gs.view = GameState::View::room;
	}

	switch(gs.view) {
	case GameState::View::room:
		move_cursor(gs.room_cursor, pad, room_items_size);
		break;
	case GameState::View::inventory:
		move_cursor(gs.inventory_cursor, pad, inventory_size);
		break;
	case GameState::View::move: {
		RoomLinks room_links = gs.map.room_links(gs.room_pos);
		for(u32 i : move_range) {
			if(room_links[i] && pad.is_pressed(i)) {
				gs.room_pos += move_dir[i];
				break;
			}
		}
	} break;
	}
}

void render_text_box(SDL_Renderer* renderer, SDL_Texture* font, Rect r, const char* s) {
	int w = renderText(renderer, font, 16, r.pos() + ivec2{ 4, 4 }, s);
	r.w = std::max(r.w, w + 8);
	auto fr = toSDL_FRect(r);
	SDL_RenderRect(renderer, &fr);
}

void render_menu(SDL_Renderer* renderer, const Textures& textures, Rect r, auto& range, u32 selected) {
	int line_count = 0;
	for(auto& e : range) {
		const char* s = menu_string(e);
		renderText(renderer, textures.font, 16, r.pos() + ivec2{ 4+16, 4 + line_count * 20 }, s);
		line_count++;
	}
	r.h = std::max(r.h, line_count * 20 + 8);
	auto fr = toSDL_FRect(r);
	SDL_RenderRect(renderer, &fr);
	SDL_FRect cr = toSDL_FRect(toRect(r.pos() + ivec2{ 4, 4 + (s32)selected * 20 }, {16, 16}));
	SDL_RenderTexture(renderer, textures.cursor, nullptr, &cr);
}

void render(const GameState& gs, SDL_Renderer* renderer, const Textures& textures) {
	renderTextFmt(renderer, textures.font, 16, { 2, 2 }, "%02d,%02d", gs.room_pos.x, gs.room_pos.y);
	switch(gs.view) {
	case GameState::View::room:
		render_menu(renderer, textures, { 240, 20, 200, 0 }, gs.map.room(gs.room_pos).items, gs.room_cursor);
		break;
	case GameState::View::inventory:
		render_menu(renderer, textures, { 240, 20, 200, 0 }, gs.inventory, gs.inventory_cursor);
		break;
	case GameState::View::move: {
		auto room_links = gs.map.room_links(gs.room_pos);
		if(room_links[MOVE_LEFT]) renderText(renderer, textures.font, 32, { 8, 160 - 16 }, "<");
		if(room_links[MOVE_RIGHT]) renderText(renderer, textures.font, 32, { 640 - 8 - 32, 160 - 16 }, ">");
		if(room_links[MOVE_UP]) renderText(renderer, textures.font, 32, { 240 - 16, 8 }, "^");
		if(room_links[MOVE_DOWN]) renderText(renderer, textures.font, 32, { 240 - 16, 360 - 8 - 32 }, "V");
	} break;
	}
}
