#include "engine.h"
#include <sstream>

int const WINDOW_WIDTH = 800;
int const WINDOW_HEIGHT = 480;

void initialize(Scenery<Scene>& scenery, TextureMap& textureMap);
Scene* createTitleScene(TextureMap& textureMap);
Scene* createGameScene(TextureMap& textureMap);
Node createBall(std::string const& name, Scene* scene, TextureMap& textureMap);
Node createPaddle(std::string const& name, Scene* scene, TextureMap& textureMap);
Node createScore(std::string const& name, float x, float y, Scene* scene);
void setPaddleControls(const std::string& name, SDL_Scancode up, SDL_Scancode down, Scene* scene);
void collideWith(Node& ball, Node& paddle);

int main()
{
  SDL_Init(SDL_INIT_EVERYTHING);
  IMG_Init(IMG_INIT_PNG);
  TTF_Init();

  SDL_Window* window = SDL_CreateWindow("Gemplates sdl2-pong example", 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
  SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);

  TextureMap textureMap(renderer);
  Scenery<Scene> scenery;
  initialize(scenery, textureMap);
  gameloop(scenery, renderer);

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);

  TTF_Quit();
  IMG_Quit();
  SDL_Quit();
  return 0;
}

void initialize(Scenery<Scene>& scenery, TextureMap& textureMap)
{
  scenery.insert("title", createTitleScene(textureMap));
  scenery.insert("game", createGameScene(textureMap));
  scenery.push("game");
}

Scene* createTitleScene(TextureMap& /*textureMap*/)
{
  return new Scene;
}

Scene* createGameScene(TextureMap& textureMap)
{
  Scene* scene = new Scene;

  createBall("ball", scene, textureMap);
  Node p1 = createPaddle("p1", scene, textureMap);
  createPaddle("p2", scene, textureMap);
  p1->components.get<Position>()->x = WINDOW_WIDTH - 44;
  setPaddleControls("p1", SDL_SCANCODE_UP, SDL_SCANCODE_DOWN, scene);
  setPaddleControls("p2", SDL_SCANCODE_W, SDL_SCANCODE_S, scene);

  createScore("p1score", WINDOW_WIDTH - 30, 10, scene);
  createScore("p2score", 10, 10, scene);

  return scene;
}

Node createBall(std::string const& name, Scene* scene, TextureMap& textureMap)
{
  Node ball = scene->add(name, { });
  scene->nodes.add(ball, Position { WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f, 0, 0, 1, 1 });
  scene->nodes.add(ball, Sprite {textureMap.get("img/ball.png")});

  ball->on<Update>(scene, [scene, name](Update const&) {
    Node ball = scene->nodesById.at(name);
    Node p1 = scene->nodesById.at("p1");
    Node p2 = scene->nodesById.at("p2");

    Position& pos = *ball->components.get<Position>();

    pos.vy = pos.y <= 0 || pos.y + pos.h >= WINDOW_HEIGHT - 1 ? -pos.vy : pos.vy;

    collideWith(ball, p1);
    collideWith(ball, p2);

    if(pos.x + pos.w < 0)
    {
      pos.x = WINDOW_WIDTH / 2;
      scene->nodesById.at("p1score")->prop<int>(PROP_VALUE) += 1;
    }
    else if(pos.x > WINDOW_WIDTH)
    {
      pos.x = WINDOW_WIDTH / 2;
      scene->nodesById.at("p2score")->prop<int>(PROP_VALUE) += 1;
    }
  });

  return ball;
}

Node createPaddle(std::string const& name, Scene* scene, TextureMap& textureMap)
{
  Node paddle = scene->add(name, {});
  scene->nodes.add(paddle, Position { 20,  WINDOW_HEIGHT / 2.0f, 0, 0, 0, 0 });
  scene->nodes.add(paddle, Sprite { textureMap.get("img/paddle.png")});

  paddle->on<Update>(scene, [scene, name](Update const&) {
    Node entity = scene->nodesById.at(name);
    Position& pos = *entity->components.get<Position>();
    pos.vy = pos.y <= 0 || pos.y + pos.h >= WINDOW_HEIGHT - 1 ? 0 : pos.vy;
  });

  return paddle;
}

Node createScore(std::string const& name, float x, float y, Scene* scene)
{
  Node score = scene->add(name, {
    {PROP_VALUE, 0},
    {PROP_OLD_VALUE, 0}
  });
  scene->nodes.add(score, Position { x, y, 0, 0, 0, 0 });
  scene->nodes.add(score, Text { "0" });

  score->on<Update>(scene, [scene, name](Update const&) {
    Node entity = scene->nodesById.at(name);
    int& value = entity->prop<int>(PROP_VALUE);
    int& oldValue = entity->prop<int>(PROP_OLD_VALUE);
    if(value != oldValue)
    {
      auto text = entity->get<Text>();
      std::ostringstream oss;
      oss << value;
      text->text = oss.str();
      text->update();
      oldValue = value;
    }
  });

  return score;
}

void setPaddleControls(std::string const& name, SDL_Scancode up, SDL_Scancode down, Scene* scene)
{
  Node entity = scene->nodesById.at(name);
  entity->on<KeyPress>(scene, [scene, name, up, down](KeyPress const& e) {
    if(e.key.scancode == up)
    {
      scene->nodesById.at(name)->components.get<Position>()->vy -= 1;
    }
    else if(e.key.scancode == down)
    {
      scene->nodesById.at(name)->components.get<Position>()->vy += 1;
    }
  });
  entity->on<KeyRelease>(scene, [scene, name, up, down](KeyRelease const& e) {
    if(e.key.scancode == up)
    {
      scene->nodesById.at(name)->components.get<Position>()->vy += 1;
    }
    else if(e.key.scancode == down)
    {
      scene->nodesById.at(name)->components.get<Position>()->vy -= 1;
    }
  });
}

void collideWith(Node& ball, Node& paddle)
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
