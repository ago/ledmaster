#ifndef _LED_H
#define _LED_H

#include <string>

enum led_colors { RED, GREEN, BLUE };

class LedView;

class Led {
	private:
		int state;
		led_colors color;
		int rate;
		LedView *lv;
	protected:
		void updated();
	public:
		Led() { state = 0; color = RED; rate = 0; }
		void set_state(int state) { this->state = state; updated(); }
		int get_state() { return state; }
		void set_color(led_colors color) { this->color = color; updated(); }
		led_colors get_color() { return this->color; }
		void set_rate(int rate);
		int get_rate() { return this->rate; }
		void set_view(LedView *lv) { this->lv = lv; }
};

class LedView {
	private:
		Led *led;
		int blink_phase;
		struct timeval tv;
	public:
		LedView(Led *led) { this->led = led; led->set_view(this); reset(); }
		void print();
		struct timeval *get_next_timeout();
		void toggle_blink_phase() { blink_phase = 1 - blink_phase; }
		void reset() { blink_phase = 0; tv.tv_sec = 0; tv.tv_usec = 0; }
};

#endif /* _LED_H */
