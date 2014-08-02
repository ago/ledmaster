#include <iostream>

#include "led.h"

#define RESET_COLOR "\e[m"
#define MAKE_RED "\e[31m"
#define MAKE_GREEN "\e[32m"
#define MAKE_BLUE "\e[34m"
#define MAKE_BOLD_RED "\e[1;31m"
#define MAKE_BOLD_GREEN "\e[1;32m"
#define MAKE_BOLD_BLUE "\e[1;34m"

using namespace std;

void Led::updated()
{
	if (lv)
		lv->print();
}

void Led::set_rate(int rate)
{
	this->rate = rate;
	if (!lv)
		return;
	lv->reset();
	updated();
	lv->toggle_blink_phase();
}

void LedView::print()
{
	if (led->get_state() && !blink_phase)
		switch (led->get_color()) {
			case RED:
				cout << "\r" MAKE_BOLD_RED ">@<" RESET_COLOR;
				break;
			case GREEN:
				cout << "\r" MAKE_BOLD_GREEN ">@<" RESET_COLOR;
				break;
			case BLUE:
				cout << "\r" MAKE_BOLD_BLUE ">@<" RESET_COLOR;
				break;
		}
	else
		switch (led->get_color()) {
			case RED:
				cout << "\r" MAKE_RED " @ " RESET_COLOR;
				break;
			case GREEN:
				cout << "\r" MAKE_GREEN " @ " RESET_COLOR;
				break;
			case BLUE:
				cout << "\r" MAKE_BLUE " @ " RESET_COLOR;
				break;
		}
	cout.flush();
}

struct timeval *LedView::get_next_timeout()
{
	int rt = led->get_rate();
	if (!rt || !led->get_state())
		return NULL;
	if (tv.tv_sec || tv.tv_usec)
		return &tv;
	if (rt == 1) {
		tv.tv_sec = 1;
		tv.tv_usec = 0;
		return &tv;
	}

	tv.tv_sec = 0;
	tv.tv_usec = 1000000 / rt;
	return &tv;
}
