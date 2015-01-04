#include "engine.h"
#include "bullets.h"
#include <sstream>
#include <iostream>

int const WINDOW_WIDTH = 800;
int const WINDOW_HEIGHT = 480;

Scene* createTitleScene();
Scene* createGameScene();

SDL_Window* window;
SDL_Renderer* renderer;

Node createEnemy(Scene* scene);

int main()
{
  SDL_Init(SDL_INIT_EVERYTHING);
  IMG_Init(IMG_INIT_PNG);
  TTF_Init();

  window = SDL_CreateWindow("Gemplates sdl2-danmaku example", 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
  renderer = SDL_CreateRenderer(window, -1, 0);
  //SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

  S::Manager scenery;
  scenery.insert("title", createTitleScene());
  scenery.insert("game", createGameScene());
  scenery.push("game");

  gameloop(scenery, renderer);

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);

  TTF_Quit();
  IMG_Quit();
  SDL_Quit();
  return 0;
}

Scene* createTitleScene()
{
  Scene* scene = new Scene(renderer);
  return scene;
}

Scene* createGameScene()
{
  Scene* scene = new Scene(renderer);

  Node enemy = createEnemy(scene);
  enemy->get<Position>()->x = WINDOW_WIDTH - 64;
  enemy->get<Position>()->y = WINDOW_HEIGHT / 2;
  enemy->get<Position>()->vy = -1;
  return scene;
}

Node createEnemy(Scene* scene)
{
  Node enemy = scene->addAnonymous({{"t", 0}},
                                   Position {},
                                   Sprite { scene->textureMap.get("img/enemy.png")});

  enemy->on<Update>([enemy](Update const&) {
    Position& p = *enemy->get<Position>();
    int& t = enemy->prop<int>("t");
    t += 1;

    if(t % 64 == 0)
    {
      createBullet32_0(enemy->scene, p.x, p.y, -3, 0);
    }

    if(p.y < 64 || p.y > WINDOW_HEIGHT - 64)
    {
      p.vy = -p.vy;
    }
  });

  return enemy;
}
