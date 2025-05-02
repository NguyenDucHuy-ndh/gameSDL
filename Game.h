#ifndef GAME_H
#define GAME_H

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <vector>
#include <string>

#include "Snake.h"
#include "Food.h"

class Game {
private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* backgroundTexture;


    // Am thanh
    Mix_Chunk* eatSound;
    Mix_Chunk* crashSound;

    // Doi tuong game
    Snake snake;
    Food food;

    // TRang tthai game
    bool running;
    int score;
    int highScore;

    Uint32 lastUpdateTime;
    int gameSpeed;
    int speedIncrement; // Tăng tốc sau mỗi lần ăn mồi

    // Fonthiển thị điểm số
    SDL_Texture* scoreTexture;
    SDL_Rect scoreRect;

    // Hàm hỗ trợ
    bool loadMedia();
    void updateScore();
    void renderScore();
    void checkCollision();
    void generateFood();

public:
    Game();
    ~Game();

    bool init();
    void handleEvents();
    void update();
    void render();
    bool isRunning() const;
    void run();
    void reset();

    // Hằng số
    static const int SCREEN_WIDTH = 640;
    static const int SCREEN_HEIGHT = 480;
    static const int GRID_SIZE = 20;
};

#endif // GAME_H
