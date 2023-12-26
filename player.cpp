#include "display.hpp"

#include "player.hpp"

#define PLAYER_HEIGHT 40

#define NAME_X 100
#define SCORE_WIDTH 98
#define NAME_WIDTH (FRAME_WIDTH - 202 - SCORE_WIDTH)
#define SCORE_X (NAME_X + NAME_WIDTH + 2)

void Player::draw(PicoGraphics_PenDV_P5& graphics, int order, int total_players, bool selected)
{
    int top = (FRAME_HEIGHT / 2) - (PLAYER_HEIGHT / 2) * total_players;
    int y = top + order * PLAYER_HEIGHT;

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
