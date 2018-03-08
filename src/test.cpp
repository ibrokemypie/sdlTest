#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

SDL_Renderer *ren;
SDL_Window *win;

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

int init() {
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

  return 0;
}

// Main function
int main() {
  if (init() != 0) {
    return 1;
  }

  SDL_SetRenderDrawColor(ren, 255, 0, 0, 255);
  SDL_RenderClear(ren);

  // Load image
  std::string imagePath = "res/test.png";
  SDL_Texture *image = loadTexture(imagePath, ren);
  std::string bgrPath = "res/background.bmp";
  SDL_Texture *bgr = loadTexture(bgrPath, ren);

  renderTexture(bgr, ren, 0, 0);
  renderTexture(bgr, ren, SCREEN_WIDTH / 2, 0);

  renderTexture(bgr, ren, 0, SCREEN_HEIGHT / 2);
  renderTexture(bgr, ren, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);

  int iW, iH;
  SDL_QueryTexture(image, NULL, NULL, &iW, &iH);
  int x = SCREEN_WIDTH / 2 - iW / 2;
  int y = SCREEN_HEIGHT / 2 - iH / 2;
  renderTexture(image, ren, x, y);

  SDL_RenderPresent(ren);
  SDL_Delay(2000);

  // Cleanup
  SDL_DestroyTexture(bgr);
  SDL_DestroyTexture(image);
  SDL_DestroyRenderer(ren);
  SDL_DestroyWindow(win);
  IMG_Quit();
  SDL_Quit();
  return 0;
}
