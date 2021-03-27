#include "marslanding/game.h"

int main(int argc, char** argv)
{
    if (init_game()) return -1;

    loop_game();

    quit_game();

    return 0;
}