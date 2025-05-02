#include <SDL.h>
#include "Game.h"

int main(int argc, char* args[]) {
    Game game;

    if (!game.init()) {
        return 1;
    }

    game.run();

    return 0;
}
