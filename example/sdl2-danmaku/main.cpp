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
Node createPlayer(Scene* scene);
Node createBeam(Scene* scene);

void setPlayerControls(Node player, SDL_Scancode up, SDL_Scancode down, SDL_Scancode left, SDL_Scancode right, SDL_Scancode fire);

int main()
{
  SDL_Init(SDL_INIT_EVERYTHING);
  IMG_Init(IMG_INIT_PNG);
  TTF_Init();

  window = SDL_CreateWindow("Gemplates sdl2-danmaku example", 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
  renderer = SDL_CreateRenderer(window, -1, 0);

  {
    S::Manager scenery;
    scenery.insert("title", createTitleScene());
    scenery.insert("game", createGameScene());
    scenery.push("game");

    gameloop(scenery, renderer);
  }

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

  Node player = createPlayer(scene);
  player->get<Position>()->x = 64;
  player->get<Position>()->y = WINDOW_HEIGHT / 2;
  setPlayerControls(player, SDL_SCANCODE_UP, SDL_SCANCODE_DOWN, SDL_SCANCODE_LEFT, SDL_SCANCODE_RIGHT, SDL_SCANCODE_SPACE);

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

Node createPlayer(Scene* scene)
{
  Node player = scene->addAnonymous({{"t", 0}},
                                   Position {},
                                   Sprite { scene->textureMap.get("img/player.png")});
  return player;
}

Node createBeam(Scene* scene)
{
  float x1, y1, x2, y2 = 0.0f;
  Node beam = scene->add("beam", {{"x1", x1}, {"y1", y1}, {"x2", x2}, {"y2", y2}});
  Node base = scene->addAnonymous(beam, {},
                                   Position {},
                                   Sprite { scene->textureMap.get("img/beam.png"), {0, 0, 32, 48}, {0, 0, 32, 48} });
  Node body = scene->addAnonymous(beam, {},
                                   Position {},
                                   Sprite { scene->textureMap.get("img/beam.png"), {32, 0, 32, 48}, {0, 0, 32, 48} });
  Node tip = scene->addAnonymous(beam, {},
                                   Position {},
                                   Sprite { scene->textureMap.get("img/beam.png"), {64, 0, 32, 48}, {0, 0, 32, 48} });
  beam->on<Update>([beam, base, body, tip](Update const&) {
    float& x1 = beam->prop<float>("x1");
    float& y1 = beam->prop<float>("y1");
    float& x2 = beam->prop<float>("x2");
    float& y2 = beam->prop<float>("y2");

    Position& basePos = *base->get<Position>();
    Position& bodyPos = *body->get<Position>();
    Position& tipPos = *tip->get<Position>();

    basePos.x = x1 + basePos.w/2;
    basePos.y = y1;
    tipPos.x = x2 - tipPos.w/2;
    tipPos.y = y2;
    bodyPos.x = (x1 + x2) / 2;
    bodyPos.y = (y1 + y2) / 2;

    Sprite& bodySpr = *body->get<Sprite>();

    bodySpr.rect.w = static_cast<int>(sqrt((x1-x2)*(x1-x2) + (y1-y2)*(y1-y2))) - 64;
  });

  return beam;

}

void setPlayerControls(Node player, SDL_Scancode up, SDL_Scancode down, SDL_Scancode left, SDL_Scancode right, SDL_Scancode fire)
{
  player->on<KeyPress>([player, up, down, left, right, fire](KeyPress const& e) {
    if(e.key.scancode == up)
    {
      player->components.get<Position>()->vy -= 1;
    }
    else if(e.key.scancode == down)
    {
      player->components.get<Position>()->vy += 1;
    }
    else if(e.key.scancode == left)
    {
      player->components.get<Position>()->vx -= 1;
    }
    else if(e.key.scancode == right)
    {
      player->components.get<Position>()->vx += 1;
    }
    else if(e.key.scancode == fire)
    {
      player->scene->defer([player] {
        Position& pos = *player->components.get<Position>();
        Node beam = createBeam(player->scene);
        beam->parent = player;
        beam->prop<float>("x1") = pos.x;
        beam->prop<float>("y1") = pos.y;
        beam->prop<float>("x2") = pos.x + WINDOW_WIDTH/2;
        beam->prop<float>("y2") = pos.y;
      });
    }
  });
  player->on<KeyRelease>([player, up, down, left, right, fire](KeyRelease const& e) {
    if(e.key.scancode == up)
    {
      player->components.get<Position>()->vy += 1;
    }
    else if(e.key.scancode == down)
    {
      player->components.get<Position>()->vy -= 1;
    }
    else if(e.key.scancode == left)
    {
      player->components.get<Position>()->vx += 1;
    }
    else if(e.key.scancode == right)
    {
      player->components.get<Position>()->vx -= 1;
    }
    else if(e.key.scancode == fire)
    {
      player->scene->defer([player] {
        player->scene->remove("beam");
      });
    }
  });
}
