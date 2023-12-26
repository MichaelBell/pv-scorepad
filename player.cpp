#include "display.hpp"

#include "player.hpp"

#define PLAYER_HEIGHT 40

#define NAME_X 100
#define SCORE_WIDTH 98
#define NAME_WIDTH (FRAME_WIDTH - 202 - SCORE_WIDTH)
#define SCORE_X (NAME_X + NAME_WIDTH + 2)

#define BID_WIDTH 78
#define NAME_WIDTH2 (FRAME_WIDTH - 200 - 2*(BID_WIDTH + 2))
#define BID_X    (NAME_X + NAME_WIDTH2 + 2)
#define SCORE_X2 (NAME_X + NAME_WIDTH2 + 4 + BID_WIDTH)

void Player::draw(PicoGraphics_PenDV_P5& graphics, int order, int total_players, bool selected, bool show_bid, bool select_score)
{
    int top = (FRAME_HEIGHT / 2) - (PLAYER_HEIGHT / 2) * total_players;
    int y = top + order * PLAYER_HEIGHT;

    if (show_bid) {
        graphics.set_pen(colour);
        graphics.rectangle({NAME_X, y + 1, NAME_WIDTH2, PLAYER_HEIGHT - 2});
        graphics.rectangle({BID_X, y + 1, BID_WIDTH, PLAYER_HEIGHT - 2});
        graphics.rectangle({SCORE_X2, y + 1, BID_WIDTH, PLAYER_HEIGHT - 2});
        graphics.set_pen(selected ? WHITE : BLACK);
        graphics.text(name, {NAME_X + 10, y + 5}, NAME_WIDTH - 15, 4.f);

        char str[16];
        graphics.set_pen((selected && !select_score) ? WHITE : BLACK);
        sprintf(str, "%d", bid);
        int len = graphics.measure_text(str, 4.f);
        graphics.text(str, {BID_X + BID_WIDTH - 10 - len, y + 5}, BID_WIDTH - 10, 4.f);

        graphics.set_pen((selected && select_score) ? WHITE : BLACK);
        sprintf(str, "%d", score);
        len = graphics.measure_text(str, 4.f);
        graphics.text(str, {SCORE_X2 + BID_WIDTH - 10 - len, y + 5}, BID_WIDTH - 10, 4.f);
    }
    else {
        graphics.set_pen(colour);
        graphics.rectangle({NAME_X, y + 1, NAME_WIDTH, PLAYER_HEIGHT - 2});
        graphics.rectangle({SCORE_X, y + 1, SCORE_WIDTH, PLAYER_HEIGHT - 2});
        graphics.set_pen(selected ? WHITE : BLACK);
        graphics.text(name, {NAME_X + 10, y + 5}, NAME_WIDTH - 15, 4.f);
        char str[16];
        sprintf(str, "%d", score);
        int len = graphics.measure_text(str, 4.f);
        graphics.text(str, {SCORE_X + SCORE_WIDTH - 10 - len, y + 5}, SCORE_WIDTH - 10, 4.f);
    }
}
