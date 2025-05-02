#include "Snake.h"
#include <iostream>
#include <SDL_image.h>


Snake::Snake(SDL_Renderer* renderer, int gridSize)
    : renderer(renderer), gridSize(gridSize), direction(RIGHT),
      headTexture(nullptr), bodyTexture(nullptr) {
}

Snake::~Snake() {
    if (headTexture) {
        SDL_DestroyTexture(headTexture);
    }
    if (bodyTexture) {
        SDL_DestroyTexture(bodyTexture);
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

void Snake::move() {
    // Tạo đoạn mới cho đầu rắn
    SnakeSegment newHead = segments.front();

    // Di chuyển đầu rắn theo hướng hiện tại
    switch (direction) {
        case UP:
            newHead.y -= gridSize;
            break;
        case DOWN:
            newHead.y += gridSize;
            break;
        case LEFT:
            newHead.x -= gridSize;
            break;
        case RIGHT:
            newHead.x += gridSize;
            break;
    }

    // Thêm đầu mới
    segments.insert(segments.begin(), newHead);

    // Xóa đuôi (sẽ không xóa nếu rắn ăn mồi, được xử lý trong Game::update())
    segments.pop_back();
}

void Snake::grow() {
    // Rắn đã di chuyển, không xóa đuôi (tức là dài ra)
    segments.push_back(segments.back());
}

void Snake::render() {
    SDL_Rect destRect = {0, 0, gridSize, gridSize};

    // Render thân rắn
    for (size_t i = 1; i < segments.size(); i++) {
        destRect.x = segments[i].x;
        destRect.y = segments[i].y;
        SDL_RenderCopy(renderer, bodyTexture, nullptr, &destRect);
    }

    // Render đầu rắn với góc quay phù hợp
    destRect.x = segments[0].x;
    destRect.y = segments[0].y;

    double angle = 0;
    switch (direction) {
        case UP:
            angle = 0;
            break;
        case RIGHT:
            angle = 90;
            break;
        case DOWN:
            angle = 180;
            break;
        case LEFT:
            angle = 270;
            break;
    }

    SDL_RenderCopyEx(renderer, headTexture, nullptr, &destRect, angle, nullptr, SDL_FLIP_NONE);
}

void Snake::setDirection(Direction newDir) {
    // Ngăn chặn di chuyển ngược lại
    if ((direction == UP && newDir != DOWN) ||
        (direction == DOWN && newDir != UP) ||
        (direction == LEFT && newDir != RIGHT) ||
        (direction == RIGHT && newDir != LEFT)) {
        direction = newDir;
    }
}

bool Snake::checkSelfCollision() const {
    const SnakeSegment& head = segments.front();

    // Kiểm tra va chạm với thân (bắt đầu từ phần tử thứ 3 để tránh va chạm giả)
    for (size_t i = 3; i < segments.size(); i++) {
        if (head.x == segments[i].x && head.y == segments[i].y) {
            return true;
        }
    }

    return false;
}
