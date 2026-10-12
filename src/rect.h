#pragma once

struct Rect {
	s32 x, y, w, h;

	constexpr ivec2 pos() const { return { x, y }; }
	constexpr ivec2 dim() const { return { w, h }; }

	constexpr Rect offset(ivec2 p) const { return { x + p.x, y + p.y, w, h }; }

	constexpr bool contains(ivec2 p) {
		return p.x >= x && p.x < x + w && p.y >= y && p.y < y + h;
	}
	constexpr bool overlaps(Rect r2) {
		return x < r2.x + r2.w && r2.x < x + w && y < r2.y + r2.h && r2.y < y + h;
	}

	constexpr Rect operator*(int s) const { return { x * s, y * s, w * s, h * s }; }
	constexpr Rect operator/(int s) const { return { x / s, y / s, w / s, h / s }; }
};

constexpr Rect toRect(ivec2 pos, ivec2 dim) {
	return { pos.x, pos.y, dim.x, dim.y };
}

constexpr SDL_FRect toSDL_FRect(Rect r) {
	return { (f32)r.x, (f32)r.y, (f32)r.w, (f32)r.h };
}
