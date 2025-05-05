#ifndef MENU_H
#define MENU_H

#include <SDL.h>
#include <SDL_ttf.h>
#include <vector>
#include <string>

enum GameState {
    MENU_STATE,
    GAME_STATE,
    PAUSE_STATE,
    GAME_OVER_STATE
};

struct MenuItem {
    std::string text;
    SDL_Rect rect;
    SDL_Texture* texture;
    bool selected;
};

class Menu {
private:
    SDL_Renderer* renderer;
    TTF_Font* font;
    TTF_Font* titleFont;
    std::vector<MenuItem> items;
    int selectedIndex;
    SDL_Texture* backgroundTexture;
    GameState currentState;

    // Title texture
    SDL_Texture* titleTexture;
    SDL_Rect titleRect;

    // Game over message
    SDL_Texture* gameOverTexture;
    SDL_Rect gameOverRect;

    // Score display for game over screen
    SDL_Texture* finalScoreTexture;
    SDL_Rect finalScoreRect;

    void renderMenuItem(const MenuItem& item);
    void clearMenuItems();

public:
    Menu(SDL_Renderer* renderer);
    ~Menu();

    bool init();
    void handleEvents(SDL_Event& e, GameState& gameState);
    void render();

    void createMainMenu();
    void createPauseMenu();
    void createGameOverMenu(int score, int highScore);

    GameState getCurrentState() const { return currentState; }
    void setState(GameState state) { currentState = state; }
};

#endif // MENU_H
