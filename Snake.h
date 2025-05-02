#ifndef SNAKE_H
#define SNAKE_H

#include <SDL.h>
#include <vector>

enum Direction {
    UP, DOWN, LEFT, RIGHT
};

struct SnakeSegment {
    int x,y;
};

class Snake {
private:
    std::vector<SnakeSegment> segments;
    Direction direction;
    SDL_Texture* headTexture;
    SDL_Texture* bodyTexture;
    SDL_Renderer* renderer;
    int gridSize;

public:
    Snake(SDL_Renderer* renderer, int gridSize);
    ~Snake();

    bool loadTextures();
    void init(int startX, int startY);
    void move();
    void grow();
    void render();
    void setDirection(Direction newDir);

    Direction getDirection() const {return direction;}
    SnakeSegment getHead() const {return segments.front();}
    std::vector<SnakeSegment>& getSegments() {return segments;}

    bool checkSelfCollision() const;
};

#endif // SNAKE_H
