// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2023 Brian Starkey <stark3y@gmail.com>

// Setup and run the bluetooth stack, will never return
// i.e. start this on Core 1 with multicore_launch_core1()
void bt_main(void);

#define BUTTON_X 0x08
#define BUTTON_Y 0x10
#define BUTTON_A 0x01
#define BUTTON_B 0x02
#define BUTTON_L 0x40
#define BUTTON_R 0x80
#define BUTTON_START  0x800
#define BUTTON_SELECT 0x400

#define PAD_UP    0x01
#define PAD_RIGHT 0x02
#define PAD_DOWN  0x04
#define PAD_LEFT  0x08

struct bt_hid_state {
	uint16_t buttons;
	uint8_t pad;
};

// Get the latest controller state
void bt_hid_get_latest(struct bt_hid_state *dst);
