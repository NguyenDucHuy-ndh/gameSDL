#ifndef FOOD_H
#define FOOD_H

#include <SDL.h>
#include <vector>
#include "Snake.h"

using namespace std;

class Food {
private:
    SDL_Point position;
    SDL_Texture* texture;
    SDL_Renderer* renderer;
    int gridSize;
    int screenWidth;
    int screenHeight;

public:
    Food(SDL_Renderer* renderer, int gridSize, int screenWidth, int screenHeight);
    ~Food();

    bool loadTexture();
    void generate(vector<SnakeSegment>& snakeSegment);
    void render();

    SDL_Point getPosition() {return position;}
};

#endif // FOOD_H
