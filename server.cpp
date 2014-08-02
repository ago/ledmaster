#include <iostream>
#include <sstream>
#include <string>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <sys/time.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

#include "led.h"

#define FIFO_IN_NAME "fifo_in"
#define FIFO_OUT_NAME "fifo_out"

using namespace std;

static int create_fifo(const char *name)
{
	struct stat st;
	int ret;

	ret = ::stat(name, &st);
	if (ret == 0 && !S_ISFIFO(st.st_mode)) {
		if (::unlink(name) == -1) {
			cerr << "Failed to unlink " << name << ": " << strerror(errno) << endl;
			return -1;
		}
		ret = -1;
	}
	if (ret == -1 && ::mkfifo(name, 0666) == -1) {
		cerr << "Failed to create fifo at " << name << ": " << strerror(errno) << endl;
		return -1;
	}

	return 0;
}

static int evalcmd(Led &led, char *buf, string &result)
{
	stringstream ss(buf);
	string cmd, arg, unwanted;
	getline(ss, cmd, ' ');
	if (cmd.compare(0, 4, "set-") == 0)
		getline(ss, arg, ' ');
	getline(ss, unwanted, ' ');
	if (!unwanted.empty())
		return -1;
	if (cmd == "set-led-state") {
		if (arg == "on") {
			led.set_state(1);
			return 0;
		}
		if (arg == "off") {
			led.set_state(0);
			return 0;
		}
		return -1;
	}
	if (cmd == "get-led-state") {
		if (led.get_state())
			result = "on";
		else
			result = "off";
		return 0;
	}
	if (cmd == "set-led-color") {
		if (arg == "red") {
			led.set_color(RED);
			return 0;
		}
		if (arg == "green") {
			led.set_color(GREEN);
			return 0;
		}
		if (arg == "blue") {
			led.set_color(BLUE);
			return 0;
		}
		return -1;
	}
	if (cmd == "get-led-color") {
		led_colors col = led.get_color();
		if (col == RED)
			result = "red";
		else if (col == GREEN)
			result = "green";
		else
			result = "blue";
		return 0;
	}
	if (cmd == "set-led-rate") {
		size_t idx;
		int r;
		try {
			r = stoi(arg, &idx, 10);
		} catch (exception e) {
			return -1;
		}
		if (idx != arg.size() || r < 0 || r > 5)
			return -1;
		led.set_rate(r);
		return 0;
	}
	if (cmd == "get-led-rate") {
		result = to_string(led.get_rate());
		return 0;
	}
	return -1;
}

int main(int argc, char *argv[])
{
	if (create_fifo(FIFO_IN_NAME) < 0)
		return 1;
	if (create_fifo(FIFO_OUT_NAME) < 0)
		return 1;

	Led led;
	LedView lv(&led);
	lv.print();

	int fd;
	FILE *fin, *fout;

	fd = ::open(FIFO_IN_NAME, O_RDWR);
	if (fd == -1) {
		cerr << "Failed to open " FIFO_IN_NAME ": " << ::strerror(errno) << endl;
		return 1;
	}

	fin = ::fdopen(fd, "r+");
	if (fin == NULL) {
		cerr << "Failed to open " FIFO_IN_NAME ": " << ::strerror(errno) << endl;
		::close(fd);
		return 1;
	}

	fout = ::fopen(FIFO_OUT_NAME, "r+");
	if (fout == NULL) {
		cerr << "Failed to open " FIFO_IN_NAME ": " << ::strerror(errno) << endl;
		::fclose(fin);
		return 1;
	}

	char buf[80];

	while (1) {
		fd_set rfds;
		int retval;

		FD_ZERO(&rfds);
		FD_SET(fd, &rfds);

		retval = ::select(fd + 1, &rfds, NULL, NULL, lv.get_next_timeout());
		if (retval == -1) {
			cerr << "select() failed: " << ::strerror(errno) << endl;
			break;
		}
		else if (retval == 0) {
			lv.print();
			lv.toggle_blink_phase();
			continue;
		}

		if (!::fgets(buf, sizeof(buf), fin))
			break;
		char *p = ::strchr(buf, '\n');
		if (!p) {
			cerr << "Received data is too long, discarded\n";
			continue;
		}
		p[0] = 0;
		string result;
		int r = evalcmd(led, buf, result);
		if (r < 0)
			::fprintf(fout, "FAILED\n");
		else if (result.empty())
			::fprintf(fout, "OK\n");
		else
			::fprintf(fout, "OK %s\n", result.data());
		::fflush(fout);
	}

	::fclose(fout);
	::fclose(fin);
}
