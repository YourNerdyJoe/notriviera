#include <chrono>
#include <thread>

#define FPS 60

static std::chrono::steady_clock::time_point timer;

void timer_init_frame_limiter(void) {
	timer = std::chrono::steady_clock::now();
}

void timer_end_frame(void) {
	timer += std::chrono::microseconds(1000000 / FPS);
	auto now = std::chrono::steady_clock::now();
	if(now < timer)	//avoid running a million frames after a breakpoint
		std::this_thread::sleep_until(timer);
	else
		timer = now;
}
