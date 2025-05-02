#include "Food.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <SDL_image.h>
#include <SDL.h>



Food::Food(SDL_Renderer* renderer, int gridSize, int screenWidth, int screenHeight)
    : renderer(renderer), gridSize(gridSize),
      screenWidth(screenWidth), screenHeight(screenHeight),
      texture(nullptr) {
    srand(static_cast<unsigned int>(time(nullptr)));

    // Khởi tạo vị trí ban đầu
    position.x = 0;
    position.y = 0;
}

Food::~Food() {
    if (texture) {
        SDL_DestroyTexture(texture);
    }
}


bool Food::loadTexture() {
    SDL_Surface* foodSurface = IMG_Load("assets/food.png");
    if (!foodSurface) {
        std::cerr << "Không thể tải hình ảnh thức ăn! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    texture = SDL_CreateTextureFromSurface(renderer, foodSurface);
    SDL_FreeSurface(foodSurface);

    if (!texture) {
        std::cerr << "Không thể tạo texture thức ăn! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    return true;
}

void Food::generate(std::vector<SnakeSegment>& snakeSegments) {
    bool validPosition = false;

    while (!validPosition) {
        position.x = (rand() % (screenWidth / gridSize)) * gridSize;
        position.y = (rand() % (screenHeight / gridSize)) * gridSize;

        // Kiểm tra xem mồi có trùng với vị trí rắn không
        validPosition = true;
        for (const auto& segment : snakeSegments) {
            if (segment.x == position.x && segment.y == position.y) {
                validPosition = false;
                break;
            }
        }
    }
}

void Food::render() {
    SDL_Rect destRect = {position.x, position.y, gridSize, gridSize};
    SDL_RenderCopy(renderer, texture, nullptr, &destRect);
}
