#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <thread>
#include <unistd.h>

struct gameSettings {
  int SCREEN_WIDTH = 640;
  int SCREEN_HEIGHT = 480;
  int FPS = 60;
};

SDL_Renderer *ren;
SDL_Window *win;

bool quit = false;
int x, y, iW, iH, frames;

struct player {
  int x;
  int y;
  int h;
  int w;

  int speed;
  std::string path;
};

// Error log function
void logSDLError(std::ostream &os, const std::string &msg) {
  os << msg << " error " << SDL_GetError() << std::endl;
}

// Texture load function, takes path and renderer, returns texture
SDL_Texture *loadTexture(const std::string &file, SDL_Renderer *ren) {
  // Load file as texture
  SDL_Texture *texture = IMG_LoadTexture(ren, file.c_str());

  if (texture == nullptr) {
    logSDLError(std::cout, "LoadTexture");
  }
  return texture;
}

// Render texture function
// Draws texture to renderer at given x and y
void renderTexture(SDL_Texture *tex, SDL_Renderer *ren, int x, int y, int w,
                   int h) {
  SDL_Rect dest;
  dest.x = x;
  dest.y = y;
  dest.w = w;
  dest.h = h;

  SDL_RenderCopy(ren, tex, NULL, &dest);
}

void renderTexture(SDL_Texture *tex, SDL_Renderer *ren, int x, int y) {
  int w, h;
  SDL_QueryTexture(tex, NULL, NULL, &w, &h);
  renderTexture(tex, ren, x, y, w, h);
}

int init(int SCREEN_WIDTH, int SCREEN_HEIGHT) {
  // Initialise SDL
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    logSDLError(std::cout, "SDL_Init");
    return 1;
  }

  // Create window
  win = SDL_CreateWindow("SDLTest", SDL_WINDOWPOS_UNDEFINED,
                         SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT,
                         SDL_WINDOW_SHOWN);
  if (win == nullptr) {
    logSDLError(std::cout, "SDL_CreateWindow");
    SDL_Quit();
    return 1;
  }

  // Create renderer
  ren = SDL_CreateRenderer(
      win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if (ren == nullptr) {
    SDL_DestroyWindow(win);
    logSDLError(std::cout, "SDL_CreateRenderer");
    SDL_Quit();
    return 1;
  }

  if ((IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG) != IMG_INIT_PNG) {
    logSDLError(std::cout, "IMG_Init");
    SDL_Quit();
    return 1;
  }

  SDL_SetRenderDrawColor(ren, 255, 0, 0, 255);
  SDL_RenderClear(ren);

  return 0;
}

void movement(int SCREEN_WIDTH, int SCREEN_HEIGHT) {
  while (!quit) {
    const Uint8 *keyState = SDL_GetKeyboardState(NULL);
    if (keyState[SDL_SCANCODE_W]) {
      if (y > 0) {
        y -= 5;
      }
    }
    if (keyState[SDL_SCANCODE_S]) {
      if (y < SCREEN_HEIGHT - iH) {
        y += 5;
      }
    }
    if (keyState[SDL_SCANCODE_D]) {
      if (x < SCREEN_WIDTH - iW) {
        x += 5;
      }
    }
    if (keyState[SDL_SCANCODE_A]) {
      if (x > 0) {
        x -= 5;
      }
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
}

void frameRate() {
  while (!quit) {
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    std::cout << "Framerate: " << frames << std::endl;
    frames = 0;
  }
}

void render(SDL_Texture *bgr, SDL_Texture *image, int SCREEN_WIDTH,
            int SCREEN_HEIGHT) {
  renderTexture(bgr, ren, 0, 0);
  renderTexture(bgr, ren, SCREEN_WIDTH / 2, 0);

  renderTexture(bgr, ren, 0, SCREEN_HEIGHT / 2);
  renderTexture(bgr, ren, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);

  renderTexture(image, ren, x, y);
  SDL_RenderPresent(ren);
}

void cleanup(std::thread &move, std::thread &frame, SDL_Texture *bgr,
             SDL_Texture *image) {
  move.join();
  frame.join();
  SDL_DestroyTexture(bgr);
  SDL_DestroyTexture(image);
  SDL_DestroyRenderer(ren);
  SDL_DestroyWindow(win);
  IMG_Quit();
  SDL_Quit();
}

// Main function
int main() {
  gameSettings *game = new gameSettings();

  if (init(game->SCREEN_WIDTH, game->SCREEN_HEIGHT) != 0) {
    return 1;
  }

  // Load image
  std::string imagePath = "res/test.png";
  SDL_Texture *image = loadTexture(imagePath, ren);
  std::string bgrPath = "res/background.bmp";
  SDL_Texture *bgr = loadTexture(bgrPath, ren);

  SDL_QueryTexture(image, NULL, NULL, &iW, &iH);
  x = game->SCREEN_WIDTH / 2 - iW / 2;
  y = game->SCREEN_HEIGHT / 2 - iH / 2;

  Uint32 waittime = 1000.0f / game->FPS;
  Uint32 framestarttime = 0;
  Sint32 delaytime;

  SDL_Event e;

  std::thread move(movement, game->SCREEN_WIDTH, game->SCREEN_HEIGHT);
  std::thread frame(frameRate);

  while (!quit) {
    while (SDL_PollEvent(&e)) {
      if (e.type == SDL_QUIT) {
        quit = true;
      }
    }

    render(bgr, image, game->SCREEN_WIDTH, game->SCREEN_HEIGHT);

    delaytime = waittime - (SDL_GetTicks() - framestarttime);
    if (delaytime > 0) {
      SDL_Delay((Uint32)delaytime);
    }
    framestarttime = SDL_GetTicks();
    frames++;
  }

  cleanup(move, frame, bgr, image);

  return 0;
}
