#include "Game.h"
#include <iostream>
#include <sstream>
#include <SDL_ttf.h>

Game::Game()
    : window(nullptr), renderer(nullptr), backgroundTexture(nullptr),
      eatSound(nullptr), crashSound(nullptr),
      snake(nullptr, GRID_SIZE), food(nullptr, GRID_SIZE, SCREEN_WIDTH, SCREEN_HEIGHT),
      running(false), score(0), highScore(0),
      lastUpdateTime(0), gameSpeed(150), speedIncrement(5),
      scoreTexture(nullptr) {
}

Game::~Game() {
    // Giải phóng tài nguyên âm thanh
    if (eatSound) {
        Mix_FreeChunk(eatSound);
    }
    if (crashSound) {
        Mix_FreeChunk(crashSound);
    }

    // Giải phóng tài nguyên SDL
    if (scoreTexture) {
        SDL_DestroyTexture(scoreTexture);
    }
    if (backgroundTexture) {
        SDL_DestroyTexture(backgroundTexture);
    }
    if (renderer) {
        SDL_DestroyRenderer(renderer);
    }
    if (window) {
        SDL_DestroyWindow(window);
    }

    // Đóng các hệ thống
    TTF_Quit();
    Mix_Quit();
    IMG_Quit();
    SDL_Quit();
}

bool Game::init() {
    // Khởi tạo SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        std::cerr << "SDL không thể khởi tạo! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    // Khởi tạo SDL_image
    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags)) {
        std::cerr << "SDL_image không thể khởi tạo! SDL_image Error: " << IMG_GetError() << std::endl;
        return false;
    }

    // Khởi tạo SDL_ttf
    if (TTF_Init() == -1) {
        std::cerr << "SDL_ttf không thể khởi tạo! SDL_ttf Error: " << TTF_GetError() << std::endl;
        return false;
    }

    // Khởi tạo SDL_mixer
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cerr << "SDL_mixer không thể khởi tạo! SDL_mixer Error: " << Mix_GetError() << std::endl;
        return false;
    }

    // Tạo cửa sổ
    window = SDL_CreateWindow("Game Rắn Săn Mồi", SDL_WINDOWPOS_UNDEFINED,
                             SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH,
                             SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == nullptr) {
        std::cerr << "Không thể tạo cửa sổ! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    // Tạo renderer
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr) {
        std::cerr << "Không thể tạo renderer! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    // Truyền renderer vào đối tượng rắn và thức ăn
    snake = Snake(renderer, GRID_SIZE);
    food = Food(renderer, GRID_SIZE, SCREEN_WIDTH, SCREEN_HEIGHT);

    // Tải các tài nguyên
    if (!loadMedia()) {
        return false;
    }

    // Khởi tạo game
    snake.init(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
    food.generate(snake.getSegments());

    running = true;
    score = 0;
    updateScore();

    return true;
}

bool Game::loadMedia() {
    // Tải hình ảnh nền
    SDL_Surface* backgroundSurface = IMG_Load("assets/background.png");
    if (!backgroundSurface) {
        std::cerr << "Không thể tải hình ảnh nền! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    backgroundTexture = SDL_CreateTextureFromSurface(renderer, backgroundSurface);
    SDL_FreeSurface(backgroundSurface);

    if (!backgroundTexture) {
        std::cerr << "Không thể tạo texture nền! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    // Tải âm thanh
    eatSound = Mix_LoadWAV("assets/eat.wav");
    if (!eatSound) {
        std::cerr << "Không thể tải âm thanh ăn mồi! SDL_mixer Error: " << Mix_GetError() << std::endl;
        return false;
    }

    crashSound = Mix_LoadWAV("assets/crash.wav");
    if (!crashSound) {
        std::cerr << "Không thể tải âm thanh va chạm! SDL_mixer Error: " << Mix_GetError() << std::endl;
        return false;
    }

    // Tải các tài nguyên khác
    if (!snake.loadTextures()) {
        return false;
    }

    if (!food.loadTexture()) {
        return false;
    }

    return true;
}

void Game::handleEvents() {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            running = false;
        }
        else if (e.type == SDL_KEYDOWN) {
            switch (e.key.keysym.sym) {
                case SDLK_UP:
                    snake.setDirection(UP);
                    break;
                case SDLK_DOWN:
                    snake.setDirection(DOWN);
                    break;
                case SDLK_LEFT:
                    snake.setDirection(LEFT);
                    break;
                case SDLK_RIGHT:
                    snake.setDirection(RIGHT);
                    break;
                case SDLK_ESCAPE:
                    running = false;
                    break;
                case SDLK_r:
                    reset();
                    break;
            }
        }
    }
}

void Game::update() {
    Uint32 currentTime = SDL_GetTicks();
    if (currentTime - lastUpdateTime < gameSpeed) {
        return; // Chưa đến thời gian cập nhật
    }
    lastUpdateTime = currentTime;

    // Di chuyển rắn
    snake.move();

    // Kiểm tra va chạm
    checkCollision();
}

void Game::checkCollision() {
    const SnakeSegment& head = snake.getHead();

    // Kiểm tra va chạm với tường
    if (head.x < 0 || head.x >= SCREEN_WIDTH ||
        head.y < 0 || head.y >= SCREEN_HEIGHT) {
        // Game over - va chạm với tường
        Mix_PlayChannel(-1, crashSound, 0);
        if (score > highScore) {
            highScore = score;
        }
        reset();
        return;
    }

    // Kiểm tra va chạm với thân rắn
    if (snake.checkSelfCollision()) {
        // Game over - rắn cắn chính nó
        Mix_PlayChannel(-1, crashSound, 0);
        if (score > highScore) {
            highScore = score;
        }
        reset();
        return;
    }

    // Kiểm tra xem rắn có ăn được mồi không
    const SDL_Point& foodPos = food.getPosition();
    if (head.x == foodPos.x && head.y == foodPos.y) {
        // Rắn ăn mồi
        Mix_PlayChannel(-1, eatSound, 0);
        snake.grow();
        food.generate(snake.getSegments());
        score += 10;
        updateScore();

        // Tăng tốc độ di chuyển (giảm thời gian đợi)
        if (gameSpeed > 50) {  // Giới hạn tốc độ tối đa
            gameSpeed -= speedIncrement;
        }
    }
}

void Game::updateScore() {
    // Cập nhật texture hiển thị điểm
    TTF_Font* font = TTF_OpenFont("assets/font.ttf", 24);
    if (!font) {
        std::cerr << "Không thể tải font! SDL_ttf Error: " << TTF_GetError() << std::endl;
        return;
    }

    // Tạo text điểm số
    std::stringstream scoreText;
    scoreText << "Score: " << score << "  High Score: " << highScore;

    SDL_Color textColor = {255, 255, 255, 255};
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, scoreText.str().c_str(), textColor);

    if (!textSurface) {
        std::cerr << "Không thể render text! SDL_ttf Error: " << TTF_GetError() << std::endl;
        TTF_CloseFont(font);
        return;
    }

    // Tạo texture từ surface
    if (scoreTexture) {
        SDL_DestroyTexture(scoreTexture);
    }

    scoreTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

    if (!scoreTexture) {
        std::cerr << "Không thể tạo texture từ text! SDL_Error: " << SDL_GetError() << std::endl;
    }

    // Lưu kích thước rect để render
    scoreRect.x = 10;
    scoreRect.y = 10;
    scoreRect.w = textSurface->w;
    scoreRect.h = textSurface->h;

    // Giải phóng bộ nhớ
    SDL_FreeSurface(textSurface);
    TTF_CloseFont(font);
}

void Game::render() {
    // Xóa màn hình
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // Vẽ nền
    SDL_RenderCopy(renderer, backgroundTexture, nullptr, nullptr);

    // Vẽ thức ăn
    food.render();

    // Vẽ rắn
    snake.render();

    // Vẽ điểm số
    renderScore();

    // Hiển thị những gì đã vẽ
    SDL_RenderPresent(renderer);
}

void Game::renderScore() {
    if (scoreTexture) {
        SDL_RenderCopy(renderer, scoreTexture, nullptr, &scoreRect);
    }
}

bool Game::isRunning() const {
    return running;
}

void Game::run() {
    while (isRunning()) {
        handleEvents();
        update();
        render();

        // Giới hạn FPS
        SDL_Delay(16); // Khoảng 60 FPS
    }
}

void Game::reset() {
    snake.init(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
    food.generate(snake.getSegments());
    gameSpeed = 150;
    score = 0;
    updateScore();
}
