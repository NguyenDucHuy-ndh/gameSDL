#include "Menu.h"
#include <iostream>
#include <sstream>
#include <SDL_image.h>

Menu::Menu(SDL_Renderer* renderer)
    : renderer(renderer), font(nullptr), titleFont(nullptr), selectedIndex(0),
      backgroundTexture(nullptr), currentState(MENU_STATE),
      titleTexture(nullptr), gameOverTexture(nullptr), finalScoreTexture(nullptr) {
}

Menu::~Menu() {
    clearMenuItems();

    if (backgroundTexture) {
        SDL_DestroyTexture(backgroundTexture);
    }

    if (titleTexture) {
        SDL_DestroyTexture(titleTexture);
    }

    if (gameOverTexture) {
        SDL_DestroyTexture(gameOverTexture);
    }

    if (finalScoreTexture) {
        SDL_DestroyTexture(finalScoreTexture);
    }

    if (font) {
        TTF_CloseFont(font);
    }

    if (titleFont) {
        TTF_CloseFont(titleFont);
    }
}

bool Menu::init() {
    // Load fonts
    font = TTF_OpenFont("assets/font.ttf", 24);
    if (!font) {
        std::cerr << "Failed to load menu font! SDL_ttf Error: " << TTF_GetError() << std::endl;
        return false;
    }

    titleFont = TTF_OpenFont("assets/font.ttf", 48);
    if (!titleFont) {
        std::cerr << "Failed to load title font! SDL_ttf Error: " << TTF_GetError() << std::endl;
        return false;
    }

    // Load menu background
    SDL_Surface* menuBgSurface = IMG_Load("assets/menu_background.png");
    if (!menuBgSurface) {
        std::cerr << "Failed to load menu background! SDL_image Error: " << IMG_GetError() << std::endl;
        // Create a fallback texture with plain color if image loading fails
        backgroundTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
                                             SDL_TEXTUREACCESS_TARGET, 640, 480);
        SDL_SetRenderTarget(renderer, backgroundTexture);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        SDL_SetRenderTarget(renderer, NULL);
    } else {
        backgroundTexture = SDL_CreateTextureFromSurface(renderer, menuBgSurface);
        SDL_FreeSurface(menuBgSurface);

        if (!backgroundTexture) {
            std::cerr << "Failed to create menu background texture! SDL Error: " << SDL_GetError() << std::endl;
            return false;
        }
    }

    // Create title
    SDL_Color titleColor = {255, 255, 0, 255}; // Yellow
    SDL_Surface* titleSurface = TTF_RenderText_Blended(titleFont, "Snake Game", titleColor);
    if (!titleSurface) {
        std::cerr << "Failed to render title text! SDL_ttf Error: " << TTF_GetError() << std::endl;
        return false;
    }

    titleTexture = SDL_CreateTextureFromSurface(renderer, titleSurface);
    titleRect = {320 - titleSurface->w / 2, 80, titleSurface->w, titleSurface->h};
    SDL_FreeSurface(titleSurface);

    // Initialize with main menu
    createMainMenu();

    return true;
}

void Menu::clearMenuItems() {
    for (auto& item : items) {
        if (item.texture) {
            SDL_DestroyTexture(item.texture);
            item.texture = nullptr;
        }
    }
    items.clear();
}

void Menu::createMainMenu() {
    clearMenuItems();

    std::vector<std::string> menuTexts = {"Play", "Instructions", "Quit"};
    SDL_Color normalColor = {255, 255, 255, 255}; // White
    SDL_Color selectedColor = {255, 255, 0, 255}; // Yellow

    int yPos = 200;
    const int spacing = 60;

    for (size_t i = 0; i < menuTexts.size(); i++) {
        MenuItem item;
        item.text = menuTexts[i];
        item.selected = (i == selectedIndex);

        SDL_Color textColor = item.selected ? selectedColor : normalColor;
        SDL_Surface* textSurface = TTF_RenderText_Blended(font, item.text.c_str(), textColor);

        if (!textSurface) {
            std::cerr << "Failed to render menu text! SDL_ttf Error: " << TTF_GetError() << std::endl;
            continue;
        }

        item.texture = SDL_CreateTextureFromSurface(renderer, textSurface);
        item.rect = {320 - textSurface->w / 2, yPos, textSurface->w, textSurface->h};

        SDL_FreeSurface(textSurface);
        yPos += spacing;

        items.push_back(item);
    }

    currentState = MENU_STATE;
}

void Menu::createPauseMenu() {
    clearMenuItems();

    std::vector<std::string> menuTexts = {"Resume", "Restart", "Main Menu", "Quit"};
    SDL_Color normalColor = {255, 255, 255, 255}; // White
    SDL_Color selectedColor = {255, 255, 0, 255}; // Yellow

    int yPos = 180;
    const int spacing = 50;

    for (size_t i = 0; i < menuTexts.size(); i++) {
        MenuItem item;
        item.text = menuTexts[i];
        item.selected = (i == selectedIndex);

        SDL_Color textColor = item.selected ? selectedColor : normalColor;
        SDL_Surface* textSurface = TTF_RenderText_Blended(font, item.text.c_str(), textColor);

        if (!textSurface) {
            std::cerr << "Failed to render menu text! SDL_ttf Error: " << TTF_GetError() << std::endl;
            continue;
        }

        item.texture = SDL_CreateTextureFromSurface(renderer, textSurface);
        item.rect = {320 - textSurface->w / 2, yPos, textSurface->w, textSurface->h};

        SDL_FreeSurface(textSurface);
        yPos += spacing;

        items.push_back(item);
    }

    currentState = PAUSE_STATE;
}

void Menu::createGameOverMenu(int score, int highScore) {
    clearMenuItems();
    selectedIndex = 0;

    // Create "Game Over" text
    SDL_Color gameOverColor = {255, 0, 0, 255}; // Red
    SDL_Surface* gameOverSurface = TTF_RenderText_Blended(titleFont, "Game Over", gameOverColor);
    if (!gameOverSurface) {
        std::cerr << "Failed to render game over text! SDL_ttf Error: " << TTF_GetError() << std::endl;
    } else {
        gameOverTexture = SDL_CreateTextureFromSurface(renderer, gameOverSurface);
        gameOverRect = {320 - gameOverSurface->w / 2, 80, gameOverSurface->w, gameOverSurface->h};
        SDL_FreeSurface(gameOverSurface);
    }

    // Create score text
    SDL_Color scoreColor = {255, 255, 255, 255}; // White
    std::stringstream scoreText;
    scoreText << "Score: " << score << "  High Score: " << highScore;

    SDL_Surface* scoreSurface = TTF_RenderText_Blended(font, scoreText.str().c_str(), scoreColor);
    if (!scoreSurface) {
        std::cerr << "Failed to render score text! SDL_ttf Error: " << TTF_GetError() << std::endl;
    } else {
        finalScoreTexture = SDL_CreateTextureFromSurface(renderer, scoreSurface);
        finalScoreRect = {320 - scoreSurface->w / 2, 150, scoreSurface->w, scoreSurface->h};
        SDL_FreeSurface(scoreSurface);
    }

    // Create menu options
    std::vector<std::string> menuTexts = {"Play Again", "Main Menu", "Quit"};
    SDL_Color normalColor = {255, 255, 255, 255}; // White
    SDL_Color selectedColor = {255, 255, 0, 255}; // Yellow

    int yPos = 220;
    const int spacing = 50;

    for (size_t i = 0; i < menuTexts.size(); i++) {
        MenuItem item;
        item.text = menuTexts[i];
        item.selected = (i == selectedIndex);

        SDL_Color textColor = item.selected ? selectedColor : normalColor;
        SDL_Surface* textSurface = TTF_RenderText_Blended(font, item.text.c_str(), textColor);

        if (!textSurface) {
            std::cerr << "Failed to render menu text! SDL_ttf Error: " << TTF_GetError() << std::endl;
            continue;
        }

        item.texture = SDL_CreateTextureFromSurface(renderer, textSurface);
        item.rect = {320 - textSurface->w / 2, yPos, textSurface->w, textSurface->h};

        SDL_FreeSurface(textSurface);
        yPos += spacing;

        items.push_back(item);
    }

    currentState = GAME_OVER_STATE;
}

void Menu::handleEvents(SDL_Event& e, GameState& gameState) {
    if (e.type == SDL_KEYDOWN) {
        switch (e.key.keysym.sym) {
            case SDLK_UP:
                // Move selection up
                selectedIndex = (selectedIndex > 0) ? selectedIndex - 1 : items.size() - 1;

                // Update selected status
                for (size_t i = 0; i < items.size(); i++) {
                    items[i].selected = (i == selectedIndex);

                    // Recreate texture with appropriate color
                    if (items[i].texture) {
                        SDL_DestroyTexture(items[i].texture);
                    }

                    SDL_Color textColor = items[i].selected ? SDL_Color{255, 255, 0, 255} : SDL_Color{255, 255, 255, 255};
                    SDL_Surface* textSurface = TTF_RenderText_Blended(font, items[i].text.c_str(), textColor);
                    items[i].texture = SDL_CreateTextureFromSurface(renderer, textSurface);
                    SDL_FreeSurface(textSurface);
                }
                break;

            case SDLK_DOWN:
                // Move selection down
                selectedIndex = (selectedIndex < items.size() - 1) ? selectedIndex + 1 : 0;

                // Update selected status
                for (size_t i = 0; i < items.size(); i++) {
                    items[i].selected = (i == selectedIndex);

                    // Recreate texture with appropriate color
                    if (items[i].texture) {
                        SDL_DestroyTexture(items[i].texture);
                    }

                    SDL_Color textColor = items[i].selected ? SDL_Color{255, 255, 0, 255} : SDL_Color{255, 255, 255, 255};
                    SDL_Surface* textSurface = TTF_RenderText_Blended(font, items[i].text.c_str(), textColor);
                    items[i].texture = SDL_CreateTextureFromSurface(renderer, textSurface);
                    SDL_FreeSurface(textSurface);
                }
                break;

            case SDLK_RETURN:
            case SDLK_SPACE:
                // Select the current item
                if (currentState == MENU_STATE) {
                    // Main menu selection
                    if (items[selectedIndex].text == "Play") {
                        gameState = GAME_STATE;
                    } else if (items[selectedIndex].text == "Instructions") {
                        // TODO: Show instructions screen
                    } else if (items[selectedIndex].text == "Quit") {
                        gameState = MENU_STATE;
                        SDL_Event quitEvent;
                        quitEvent.type = SDL_QUIT;
                        SDL_PushEvent(&quitEvent);
                    }
                } else if (currentState == PAUSE_STATE) {
                    // Pause menu selection
                    if (items[selectedIndex].text == "Resume") {
                        gameState = GAME_STATE;
                    } else if (items[selectedIndex].text == "Restart") {
                        gameState = GAME_STATE;
                        // Game will handle the reset
                    } else if (items[selectedIndex].text == "Main Menu") {
                        createMainMenu();
                        gameState = MENU_STATE;
                    } else if (items[selectedIndex].text == "Quit") {
                        gameState = MENU_STATE;
                        SDL_Event quitEvent;
                        quitEvent.type = SDL_QUIT;
                        SDL_PushEvent(&quitEvent);
                    }
                } else if (currentState == GAME_OVER_STATE) {
                    // Game over menu selection
                    if (items[selectedIndex].text == "Play Again") {
                        gameState = GAME_STATE;
                        // Game will handle the reset
                    } else if (items[selectedIndex].text == "Main Menu") {
                        createMainMenu();
                        gameState = MENU_STATE;
                    } else if (items[selectedIndex].text == "Quit") {
                        gameState = MENU_STATE;
                        SDL_Event quitEvent;
                        quitEvent.type = SDL_QUIT;
                        SDL_PushEvent(&quitEvent);
                    }
                }
                break;

            case SDLK_ESCAPE:
                if (currentState == GAME_STATE) {
                    createPauseMenu();
                    gameState = PAUSE_STATE;
                } else if (currentState == PAUSE_STATE) {
                    gameState = GAME_STATE;
                }
                break;
        }
    }
}

void Menu::renderMenuItem(const MenuItem& item) {
    if (item.texture) {
        SDL_RenderCopy(renderer, item.texture, nullptr, &item.rect);
    }
}

void Menu::render() {
    // Render background
    if (backgroundTexture) {
        SDL_RenderCopy(renderer, backgroundTexture, nullptr, nullptr);
    }

    // Render title or game over message
    if (currentState == MENU_STATE) {
        if (titleTexture) {
            SDL_RenderCopy(renderer, titleTexture, nullptr, &titleRect);
        }
    } else if (currentState == GAME_OVER_STATE) {
        if (gameOverTexture) {
            SDL_RenderCopy(renderer, gameOverTexture, nullptr, &gameOverRect);
        }

        if (finalScoreTexture) {
            SDL_RenderCopy(renderer, finalScoreTexture, nullptr, &finalScoreRect);
        }
    } else if (currentState == PAUSE_STATE) {
        // Render "PAUSED" text at the top
        SDL_Color pauseColor = {255, 255, 0, 255}; // Yellow
        SDL_Surface* pauseSurface = TTF_RenderText_Blended(titleFont, "PAUSED", pauseColor);
        if (pauseSurface) {
            SDL_Texture* pauseTexture = SDL_CreateTextureFromSurface(renderer, pauseSurface);
            SDL_Rect pauseRect = {320 - pauseSurface->w / 2, 80, pauseSurface->w, pauseSurface->h};
            SDL_RenderCopy(renderer, pauseTexture, nullptr, &pauseRect);
            SDL_DestroyTexture(pauseTexture);
            SDL_FreeSurface(pauseSurface);
        }
    }

    // Render menu items
    for (const auto& item : items) {
        renderMenuItem(item);
    }
}
