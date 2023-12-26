#include "pico/stdlib.h"
#include <cstdio>
#include <iterator>
#include <sstream>

#include "pico/flash.h"
#include "pico/multicore.h"

extern "C" {
#include "bt_hid.h"
}

#include "display.hpp"

#include "player.hpp"

DVDisplay display;
PicoGraphics_PenDV_P5 graphics(FRAME_WIDTH, FRAME_HEIGHT, display);

Pen BLACK = graphics.create_pen(0, 0, 0);
Pen PURPLE = graphics.create_pen(140, 0, 200);
Pen BLUE = graphics.create_pen(0, 0, 200);
Pen GREEN = graphics.create_pen(0, 180, 0);
Pen RED = graphics.create_pen(180, 0, 0);
Pen ORANGE = graphics.create_pen(200, 120, 0);
Pen YELLOW = graphics.create_pen(200, 200, 0);
Pen WHITE = graphics.create_pen(255, 255, 255);

#define MAX_PLAYERS 6
Player players[MAX_PLAYERS] = {
    { "MIKIE", GREEN },
    { "LIZZIE", PURPLE },
    { "BARBARA", BLUE },
    { "DAVID", RED },
    { "KATHLEEN", YELLOW },
    { "PETER", ORANGE }
};
int num_players = MAX_PLAYERS;
int selected = 0;
std::array<int, MAX_PLAYERS> order;
bool low_to_high = false;
int increment = 1;

void reorder() {
    if (low_to_high) {
        std::sort(order.begin(), order.begin() + num_players, [](int a, int b) {
            return players[a].score < players[b].score;
        });
    }
    else {
        std::sort(order.begin(), order.begin() + num_players, [](int a, int b) {
            return players[a].score > players[b].score;
        });
    }

    for (int i = 0; i < MAX_PLAYERS; ++i) {
        printf("%d ", order[i]);
    }
    printf("\n");
}

int selected_entry() {
    for (int i = 0; i < num_players; ++i) {
        if (order[i] == selected) return i;
    }
    return 0;
}

int main() {
    for (int i = 0; i < MAX_PLAYERS; ++i) { order[i] = i; }

    stdio_init_all();

    // Wait for USB console connection
    //while (!stdio_usb_connected());
    printf("Hello World\n");

    printf("Init Bluetooth\n");
	flash_safe_execute_core_init();

	multicore_launch_core1(bt_main);

    printf("Init Video...\n");
    display.preinit();
    display.init(FRAME_WIDTH, FRAME_HEIGHT, DVDisplay::MODE_PALETTE, FRAME_WIDTH, FRAME_HEIGHT, true);
    printf("Done!\n");

    graphics.set_font("bitmap8");

	struct bt_hid_state state;
    char str[80];
    while(true) {
        graphics.set_pen(BLACK);
        graphics.clear();
        graphics.set_pen(WHITE);
		bt_hid_get_latest(&state);
        //sprintf(str, "buttons: %04x, pad: %02x",
        //        state.buttons, state.pad);

        int len = graphics.measure_text("PicoVision ScorePad", 5.f);
        graphics.text("PicoVision ScorePad", Point((FRAME_WIDTH - len) / 2, 15), FRAME_WIDTH, 5.f);

        sprintf(str, "Increment %d, Sort %s", increment, low_to_high ? "Low to High" : "High to Low");
        graphics.text(str, Point(20, 440), FRAME_WIDTH);

        for (int i = 0; i < num_players; ++i) {
            players[order[i]].draw(graphics, i, num_players, i == selected);
        }

        int pause = 0;
        if (state.buttons & BUTTON_START) {
            if (state.pad & PAD_UP) num_players = MAX_PLAYERS;
            for (int i = 0; i < num_players; ++i) {
                players[i].score = 0;
            }
            pause = 200;
        }
        else if ((state.pad & PAD_UP) && selected > 0) {
            --selected;
            pause = 200;
        }
        else if ((state.pad & PAD_DOWN) && selected < num_players-1) {
            ++selected;
            pause = 200;
        }
        else if (state.pad & PAD_LEFT) {
            players[order[selected]].score -= increment;
            pause = 100;
        }
        else if (state.pad & PAD_RIGHT) {
            players[order[selected]].score += increment;
            pause = 100;
        }
        else if (state.buttons & BUTTON_A) {
            reorder();
            pause = 200;
        }
        else if (state.buttons & BUTTON_B) {
            if (increment == 1) increment = 5;
            else if (increment == 5) increment = 10;
            else increment = 1;
            pause = 200;
        }
        else if (state.buttons & BUTTON_Y) {
            std::swap(order[selected], order[num_players-1]);
            --num_players;
            if (selected >= num_players) selected = num_players - 1;
            reorder();
            pause = 200;
        }
        else if (state.buttons & BUTTON_X) {
            if (num_players < MAX_PLAYERS) ++num_players;
            reorder();
            pause = 200;
        }
        else if (state.buttons & BUTTON_SELECT) {
            low_to_high = !low_to_high;
            pause = 200;
        }

        display.flip();
        sleep_ms(pause);
	}
}