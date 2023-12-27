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
Pen TEAL = graphics.create_pen(0, 160, 160);
Pen PINK = graphics.create_pen(255, 127, 127);
Pen GREY = graphics.create_pen(127, 127, 127);
Pen WHITE = graphics.create_pen(255, 255, 255);

#define MAX_PLAYERS 9
Player players[MAX_PLAYERS] = {
    { "MIKIE", GREEN },
    { "LIZZIE", PURPLE },
    { "BARBARA", BLUE },
    { "DAVID", RED },
    { "KATHLEEN", YELLOW },
    { "PETER", ORANGE },
    { "PAM", PINK },
    { "PAUL", GREY },
    { "CLAIRE", TEAL },
};
int num_players = MAX_PLAYERS;
int selected = 0;
std::array<int, MAX_PLAYERS> order;
bool low_to_high = false;
bool select_score = true;
bool show_bid = false;
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
        graphics.text("PicoVision ScorePad", Point((FRAME_WIDTH - len) / 2, 12), FRAME_WIDTH, 5.f);

        sprintf(str, "Increment %d, Sort %s", increment, low_to_high ? "Low to High" : "High to Low");
        graphics.text(str, Point(20, 440), FRAME_WIDTH);

        if (state.connected) graphics.text("Connected", Point(618, 440), 100);
        else graphics.text("Disconnected", Point(590, 440), 100);

        for (int i = 0; i < num_players; ++i) {
            players[order[i]].draw(graphics, i, num_players, i == selected, show_bid, select_score);
        }

        if (num_players < 8 && show_bid) {
            graphics.set_pen(WHITE);

            int y = (FRAME_HEIGHT - num_players * 40) / 2 - 22;
            int len = graphics.measure_text("Bid");
            graphics.text("Bid", Point(500 - len/2, y), 60);
            len = graphics.measure_text("Score");
            graphics.text("Score", Point(580 - len/2, y), 60);
        }

        int pause = 0;
        if (state.buttons & BUTTON_START) {
            if (state.pad & PAD_UP) num_players = MAX_PLAYERS;
            for (int i = 0; i < MAX_PLAYERS; ++i) {
                players[i].score = 0;
                players[i].bid = 0;
            }
            pause = 200;
        }
        else if ((state.pad & PAD_UP) && selected > 0) {
            --selected;
            pause = 150;
        }
        else if ((state.pad & PAD_DOWN) && selected < num_players-1) {
            ++selected;
            pause = 150;
        }
        else if (state.pad & PAD_LEFT) {
            if (select_score) players[order[selected]].score -= increment;
            else players[order[selected]].bid -= increment;
            pause = 150;
        }
        else if (state.pad & PAD_RIGHT) {
            if (select_score) players[order[selected]].score += increment;
            else players[order[selected]].bid += increment;
            pause = 150;
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
        else if ((state.buttons & BUTTON_L) && (state.buttons & BUTTON_R)) {
            show_bid = !show_bid;
            if (!show_bid) select_score = true;
            for (int i = 0; i < MAX_PLAYERS; ++i) {
                players[i].bid = 0;
            }
           pause = 200;
        }
        else if ((state.buttons & BUTTON_L) && show_bid) {
            select_score = false;
        }
        else if (state.buttons & BUTTON_R) {
            select_score = true;
        }

        display.flip();
        sleep_ms(pause);
	}
}