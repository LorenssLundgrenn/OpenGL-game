#include <iostream>
#include <SDL.h>
#include <plog/Log.h>
#include <plog/Initializers/RollingFileInitializer.h>

#include "GameState.h"

int main(int argv, char* argc[]) {
    plog::init(plog::none, "log.txt");

    GameState game = GameState(960, 540);
    game.run();

    return 0;
}