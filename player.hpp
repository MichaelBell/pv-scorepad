#pragma once

struct Player {
    const char* name;
    Pen colour;
    int score = 0;
    int bid = 0;

    Player(const char* name_, Pen colour_) : name(name_), colour(colour_) {}

    void draw(PicoGraphics_PenDV_P5& graphics, int order, int total_players, bool selected, bool show_bid, bool select_score);
};