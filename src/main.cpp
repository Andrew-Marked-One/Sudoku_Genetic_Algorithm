#include "GameEngine.h"

int main() {
    GameEngine game("resources/config.txt");
    game.run();

    return EXIT_SUCCESS;
}
