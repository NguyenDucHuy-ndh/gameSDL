#include <Snake.h>
#include <iostream>

Snake::Snake(SDL_Renderer* renderer, int gridSize)
    : renderer(renderer), gridSize(gridSize), direction(RIGHT),
        headTexture(nullptr), bodyTexture(nullptr) {
}

Snake::~Snake() {
    if (headTexture) {
        SDL_DestroyTexture(headTexture);
    }
    if (bodyTexture) {
        SDL_DestroyTexture(bodyTexture;)
    }
}

bool Snake::loadTextures() {
    // Tải hình ảnh đầu rắn
    SDL_Surface* headSurface = IMG_Load("assets/snake_head.png");
    if (!headSurface) {
        std::cerr << "Không thể tải hình ảnh đầu rắn! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    headTexture = SDL_CreateTextureFromSurface(renderer, headSurface);
    SDL_FreeSurface(headSurface);

    if (!headTexture) {
        std::cerr << "Không thể tạo texture đầu rắn! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    // Tải hình ảnh thân rắn
    SDL_Surface* bodySurface = IMG_Load("assets/snake_body.png");
    if (!bodySurface) {
        std::cerr << "Không thể tải hình ảnh thân rắn! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    bodyTexture = SDL_CreateTextureFromSurface(renderer, bodySurface);
    SDL_FreeSurface(bodySurface);

    if (!bodyTexture) {
        std::cerr << "Không thể tạo texture thân rắn! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    return true;
}

void Snake::init(int startX, int startY) {
    segments.clear();

    // Tạo rắn ban đầu với 3 đoạn
    for (int i = 0; i < 3; i++) {
        SnakeSegment segment;
        segment.x = startX - i * gridSize;
        segment.y = startY;
        segments.push_back(segment);
    }

    direction = RIGHT;
}

