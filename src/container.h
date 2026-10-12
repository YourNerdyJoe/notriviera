#pragma once
#include <ranges>
#include <span>

template<typename T, bool no_null_obj = true>
class Handle {
protected:
	u32 handle;
public:
	constexpr Handle() = default;
	constexpr Handle(T* ptr, T* arr) : handle((u32)(ptr ? ptr - arr + no_null_obj : 0)) {}
	constexpr Handle(u32 idx) : handle(idx + no_null_obj) {}
	constexpr operator bool() const { return handle; }
	constexpr T* get(T* arr) const {
		return (!no_null_obj || handle) ? (arr + handle - no_null_obj) : nullptr;
	}
	constexpr auto operator++(this auto& self) {
		auto ret = self;
		ret.handle++;
		return ret;
	}
};

template<typename T, T* array, bool no_null_obj = true>
class GlobalHandle : public Handle<T, no_null_obj> {
public:
	constexpr GlobalHandle() = default;
	constexpr GlobalHandle(T* ptr) : Handle<T, no_null_obj>(ptr, array) {}
	constexpr GlobalHandle(u32 idx) : Handle<T, no_null_obj>(idx) {}
	/*constexpr bool operator==(const GlobalHandle<T, array, no_null_obj>& other) {
		return handle == other.handle;
	}*/
	constexpr T* get() const {
		return Handle<T, no_null_obj>::get(array);
	}
	constexpr T* operator->() const {
		return get();
	}
	constexpr T& operator*() const {
		return *get();
	}
};

template <typename T, size_t N> requires std::is_trivial_v<T>
struct DynArr {
	u32 size;
	T data[N];

	constexpr auto begin(this auto& self) { return self.data; }
	constexpr auto end(this auto& self) { return self.data + self.size; }

	constexpr bool full() const { return size >= N; }
	constexpr bool contains_ptr(const T* ptr) const { return ptr >= begin() && ptr < end(); }

	constexpr T& add() { gs_assert(!full()); return data[size++]; }
	constexpr void remove(u32 idx) { gs_assert(idx < size); data[idx] = data[--size]; }
	constexpr void remove(T* ptr) { gs_assert(contains_ptr(ptr)); remove((u32)(ptr - data)); }
	constexpr void clear() { size = 0; }
};

template <typename T, size_t N> requires std::is_trivial_v<T>
struct DynArrFreeList {
	Handle<T> next_free;
	u32 size;
	T data[N];
	bool is_active[N];

	constexpr auto active(this auto& self) {
		return std::span(self.data, self.size) | std::views::filter([&](auto& v){ return self.is_active[&v - self.data]; });
	}

	constexpr bool full() const { return size >= N && !next_free; }
	constexpr bool contains_ptr(const T* ptr) const { return ptr >= data && ptr < data + size; }
	constexpr bool contains_active_ptr(const T* ptr) const { return contains_ptr(ptr) && is_active[ptr - data]; }

	constexpr T& add() {
		gs_assert(!full());
		T* ret;
		if(next_free) {
			ret = next_free.get(data);
			next_free = ret->next;
		}
		else {
			ret = data + size++;
		}
		is_active[ret - data] = true;
		return *ret;
	}

	constexpr void remove(u32 idx) {
		gs_assert(idx < size);
		gs_assert(is_active[idx]);
		data[idx].next = next_free;
		is_active[idx] = false;
		next_free = idx;
	}

	constexpr void remove(T* ptr) { gs_assert(contains_ptr(ptr)); remove((u32)(ptr - data)); }
	constexpr void remove(Handle<T> handle) { remove(handle.get(data)); }
	constexpr void clear() { next_free = {}; size = 0; }
};

template<typename T, size_t N>
constexpr T* handle_get_active(Handle<T> handle, DynArrFreeList<T, N>& list) {
	if(T* ptr = handle.get(list.data)) {
		if(list.contains_active_ptr(ptr)) {
			return ptr;
		}
	}
	return nullptr;
}

template<typename T, size_t N>
constexpr const T* handle_get_active(Handle<T> handle, const DynArrFreeList<T, N>& list) {
	if(const T* ptr = handle.get(list.data)) {
		if(list.contains_active_ptr(ptr)) {
			return ptr;
		}
	}
	return nullptr;
}
