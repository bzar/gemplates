#include "engine.h"
#include <sstream>
#include <iostream>

int const WINDOW_WIDTH = 800;
int const WINDOW_HEIGHT = 480;

Scene* createTitleScene();
Scene* createGameScene();
Node createBall(std::string const& name, Scene* scene);
Node createPaddle(std::string const& name, Scene* scene);
Node createScore(std::string const& name, float x, float y, Scene* scene);
Node createTitleOption(std::string const& name, std::string const& image, float x, float y, bool active, Scene* scene);
void setPaddleControls(Node paddle, SDL_Scancode up, SDL_Scancode down);
void collideWith(Node ball, Node paddle);

SDL_Window* window;
SDL_Renderer* renderer;

int main()
{
  SDL_Init(SDL_INIT_EVERYTHING);
  IMG_Init(IMG_INIT_PNG);
  TTF_Init();

  window = SDL_CreateWindow("Gemplates sdl2-pong example", 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
  renderer = SDL_CreateRenderer(window, -1, 0);

  S::Manager scenery;
  scenery.insert("title", createTitleScene());
  scenery.insert("game", createGameScene());
  scenery.push("title");

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
  scene->add("background", {},
             Position {0, 0, 0, 0, 0, 0},
             Sprite {scene->textureMap.get("img/title.png")});

  createTitleOption("start", "img/start.png", 60, 300, true, scene);
  createTitleOption("quit", "img/quit.png", 440, 300, false, scene);

  Node controller = scene->add("controller", {});
  controller->on<KeyPress>([controller](KeyPress const& e) {
    Node start = controller->scene->nodesById.at("start");
    Node quit = controller->scene->nodesById.at("quit");
    if(e.key.scancode == SDL_SCANCODE_LEFT || e.key.scancode == SDL_SCANCODE_RIGHT)
    {
      start->prop<bool>(PROP_ACTIVE) = !start->prop<bool>(PROP_ACTIVE);
      quit->prop<bool>(PROP_ACTIVE) = !quit->prop<bool>(PROP_ACTIVE);
    }
    else if(e.key.scancode == SDL_SCANCODE_RETURN)
    {
      if(start->prop<bool>(PROP_ACTIVE))
      {
        controller->scene->manager->push("game");
        Scene* s = controller->scene->manager->current();

        Node ball = s->nodesById.at("ball");
        Node paddle1 = s->nodesById.at("p1");
        Node paddle2 = s->nodesById.at("p2");
        Node p1score = s->nodesById.at("p1score");
        Node p2score = s->nodesById.at("p2score");

        ball->get<Position>()->x = WINDOW_WIDTH / 2;
        ball->get<Position>()->y = WINDOW_HEIGHT / 2;

        paddle1->get<Position>()->y = WINDOW_HEIGHT / 2;
        paddle2->get<Position>()->y = WINDOW_HEIGHT / 2;

        p1score->prop<int>(PROP_VALUE) = 0;
        p2score->prop<int>(PROP_VALUE) = 0;
      }
      else
      {
        controller->scene->manager->pop();
      }
    }
    else if(e.key.scancode == SDL_SCANCODE_ESCAPE)
    {
      controller->scene->manager->pop();
    }
  });
  return scene;
}

Scene* createGameScene()
{
  Scene* scene = new Scene(renderer);

  createBall("ball", scene);
  Node p1 = createPaddle("p1", scene);
  Node p2 = createPaddle("p2", scene);
  p1->components.get<Position>()->x = WINDOW_WIDTH - 44;
  setPaddleControls(p1, SDL_SCANCODE_UP, SDL_SCANCODE_DOWN);
  setPaddleControls(p2, SDL_SCANCODE_W, SDL_SCANCODE_S);

  createScore("p1score", WINDOW_WIDTH - 30, 10, scene);
  createScore("p2score", 10, 10, scene);

  Node controller = scene->add("controller", {});
  controller->on<KeyPress>([controller](KeyPress const& e) {
    if(e.key.scancode == SDL_SCANCODE_ESCAPE)
    {
      controller->scene->manager->pop();
    }
  });

  return scene;
}

Node createBall(std::string const& name, Scene* scene)
{
  Node ball = scene->add(name, { },
                         Position { WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f, 0, 0, 1, 1 },
                         Sprite {scene->textureMap.get("img/ball.png")});

  ball->on<Update>([ball](Update const&) {
    Node p1 = ball->scene->nodesById.at("p1");
    Node p2 = ball->scene->nodesById.at("p2");

    Position& pos = *ball->components.get<Position>();

    pos.vy = pos.y <= 0 || pos.y + pos.h >= WINDOW_HEIGHT - 1 ? -pos.vy : pos.vy;

    collideWith(ball, p1);
    collideWith(ball, p2);

    if(pos.x + pos.w < 0)
    {
      pos.x = WINDOW_WIDTH / 2;
      ball->scene->nodesById.at("p1score")->prop<int>(PROP_VALUE) += 1;
    }
    else if(pos.x > WINDOW_WIDTH)
    {
      pos.x = WINDOW_WIDTH / 2;
      ball->scene->nodesById.at("p2score")->prop<int>(PROP_VALUE) += 1;
    }
  });

  return ball;
}

Node createPaddle(std::string const& name, Scene* scene)
{
  Node paddle = scene->add(name, {},
                           Position { 20,  WINDOW_HEIGHT / 2.0f, 0, 0, 0, 0 },
                           Sprite { scene->textureMap.get("img/paddle.png")});

  paddle->on<Update>([paddle](Update const&) {
    Position& pos = *paddle->components.get<Position>();
    pos.y = pos.y < 0 ? 0 : pos.y;
    pos.y = pos.y + pos.h > WINDOW_HEIGHT - 1 ? WINDOW_HEIGHT - 1 - pos.h : pos.y;
  });

  return paddle;
}

Node createScore(std::string const& name, float x, float y, Scene* scene)
{
  Node score = scene->add(name, { {PROP_VALUE, 0}, {PROP_OLD_VALUE, 0} },
                          Position { x, y, 0, 0, 0, 0 },
                          Text { "0" });

  score->on<Update>([score](Update const&) {
    int& value = score->prop<int>(PROP_VALUE);
    int& oldValue = score->prop<int>(PROP_OLD_VALUE);
    if(value != oldValue)
    {
      auto text = score->get<Text>();
      std::ostringstream oss;
      oss << value;
      text->text = oss.str();
      text->update();
      oldValue = value;
    }
  });

  return score;
}
Node createTitleOption(std::string const& name, std::string const& image, float x, float y, bool active, Scene* scene)
{
  Node node = scene->add(name, { {PROP_ACTIVE, active} },
                         Position {x, y, 0, 0, 0, 0},
                         Sprite {scene->textureMap.get(image)});

  node->on<Update>([node](Update const&) {
    bool& active = node->prop<bool>(PROP_ACTIVE);
    Sprite& sprite = *node->get<Sprite>();
    sprite.opacity = active ? 1.0f : 0.5f;
  });

  return node;
}

void setPaddleControls(Node paddle, SDL_Scancode up, SDL_Scancode down)
{
  paddle->on<KeyPress>([paddle, up, down](KeyPress const& e) {
    if(e.key.scancode == up)
    {
      paddle->components.get<Position>()->vy -= 1;
    }
    else if(e.key.scancode == down)
    {
      paddle->components.get<Position>()->vy += 1;
    }
  });
  paddle->on<KeyRelease>([paddle, up, down](KeyRelease const& e) {
    if(e.key.scancode == up)
    {
      paddle->components.get<Position>()->vy += 1;
    }
    else if(e.key.scancode == down)
    {
      paddle->components.get<Position>()->vy -= 1;
    }
  });
}

void collideWith(Node ball, Node paddle)
{
  Position& bp = *ball->components.get<Position>();
  Position& pp = *paddle->components.get<Position>();

  SDL_Rect ballDestination {static_cast<int>(bp.x + bp.vx), static_cast<int>(bp.y + bp.vy), static_cast<int>(bp.w), static_cast<int>(bp.h)};
  SDL_Rect paddleDestination {static_cast<int>(pp.x), static_cast<int>(pp.y + pp.vy), static_cast<int>(pp.w), static_cast<int>(pp.h)};
  SDL_Rect result;
  if(SDL_IntersectRect(&ballDestination, &paddleDestination, &result))
  {
    if(result.w > result.h)
    {
      bp.vy = -bp.vy;
    }
    else
    {
      bp.vx = -bp.vx;
    }
  }
}
