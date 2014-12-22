#include "engine.h"
#include <sstream>

int const WINDOW_WIDTH = 800;
int const WINDOW_HEIGHT = 480;

void initialize(Scenery<Scene>& scenery, TextureMap& textureMap);
Scene* createTitleScene(TextureMap& textureMap);
Scene* createGameScene(TextureMap& textureMap);
Node createBall(std::string const& name, Scene* scene, TextureMap& textureMap);
Node createPaddle(std::string const& name, Scene* scene, TextureMap& textureMap);
Node createScore(std::string const& name, int x, int y, Scene* scene);
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
  p1->prop<float>(PROP_X) = WINDOW_WIDTH - 20 - scene->nodesById.at("p1")->prop<int>(PROP_W);
  setPaddleControls("p1", SDL_SCANCODE_UP, SDL_SCANCODE_DOWN, scene);
  setPaddleControls("p2", SDL_SCANCODE_W, SDL_SCANCODE_S, scene);

  createScore("p1score", WINDOW_WIDTH - 30, 10, scene);
  createScore("p2score", 10, 10, scene);

  return scene;
}

Node createBall(std::string const& name, Scene* scene, TextureMap& textureMap)
{
  Node ball = scene->nodes.add({
    {PROP_X, static_cast<float>(WINDOW_WIDTH / 2.0f)},
    {PROP_Y, static_cast<float>(WINDOW_HEIGHT / 2.0f)},
    {PROP_VX, 1.0f},
    {PROP_VY, 1.0f}
  }, Sprite {textureMap.get("img/ball.png"), 0, 0});
  ball->prop<int>(PROP_W, ball->get<Sprite>()->rect.w);
  ball->prop<int>(PROP_H, ball->get<Sprite>()->rect.h);
  scene->nodesById.insert(std::make_pair(name, ball));

  ball->on<Update>(scene, [scene, name](Update const&) {
    Node ball = scene->nodesById.at(name);
    Node p1 = scene->nodesById.at("p1");
    Node p2 = scene->nodesById.at("p2");

    float& x = ball->prop<float>(PROP_X);
    float& y = ball->prop<float>(PROP_Y);
    int& w = ball->prop<int>(PROP_W);
    int& h = ball->prop<int>(PROP_H);
    float& vx = ball->prop<float>(PROP_VX);
    float& vy = ball->prop<float>(PROP_VY);

    vy = y <= 0 || y + h >= WINDOW_HEIGHT - 1 ? -vy : vy;

    collideWith(ball, p1);
    collideWith(ball, p2);

    x += vx;
    y += vy;

    if(x + w < 0)
    {
      x = WINDOW_WIDTH / 2;
      scene->nodesById.at("p1score")->prop<int>(PROP_VALUE) += 1;
    }
    else if(x > WINDOW_WIDTH)
    {
      x = WINDOW_WIDTH / 2;
      scene->nodesById.at("p2score")->prop<int>(PROP_VALUE) += 1;
    }
    auto sprite = ball->get<Sprite>();
    sprite->rect.x = x;
    sprite->rect.y = y;
  });

  return ball;
}

Node createPaddle(std::string const& name, Scene* scene, TextureMap& textureMap)
{
  float y = WINDOW_HEIGHT / 2.0f;
  Node paddle = scene->nodes.add({
    {PROP_X, 20.0f},
    {PROP_Y, y},
    {PROP_VY, 0.0f}
  }, Sprite { textureMap.get("img/paddle.png"), 0, 0});
  paddle->prop<int>(PROP_W, paddle->get<Sprite>()->rect.w);
  paddle->prop<int>(PROP_H, paddle->get<Sprite>()->rect.h);
  scene->nodesById.insert(std::make_pair(name, paddle));

  paddle->on<Update>(scene, [scene, name](Update const&) {
    Node entity = scene->nodesById.at(name);
    float& x = entity->prop<float>(PROP_X);
    float& y = entity->prop<float>(PROP_Y);
    int& h = entity->prop<int>(PROP_H);
    float& vy = entity->prop<float>(PROP_VY);

    y += y + vy < 0 || y + h + vy >= WINDOW_HEIGHT ? 0 : vy;

    auto sprite = entity->get<Sprite>();
    sprite->rect.x = x;
    sprite->rect.y = y;
  });

  return paddle;
}

Node createScore(std::string const& name, int x, int y, Scene* scene)
{
  Node score = scene->nodes.add({
    {PROP_VALUE, 0},
    {PROP_OLD_VALUE, 0}
  }, Text { "0", x, y });
  scene->nodesById.insert(std::make_pair(name, score));

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
      scene->nodesById.at(name)->prop<float>(PROP_VY) -= 1;
    }
    else if(e.key.scancode == down)
    {
      scene->nodesById.at(name)->prop<float>(PROP_VY) += 1;
    }
  });
  entity->on<KeyRelease>(scene, [scene, name, up, down](KeyRelease const& e) {
    if(e.key.scancode == up)
    {
      scene->nodesById.at(name)->prop<float>(PROP_VY) += 1;
    }
    else if(e.key.scancode == down)
    {
      scene->nodesById.at(name)->prop<float>(PROP_VY) -= 1;
    }
  });
}

void collideWith(Node& ball, Node& paddle)
{
  float& x = ball->prop<float>(PROP_X);
  float& y = ball->prop<float>(PROP_Y);
  int& w = ball->prop<int>(PROP_W);
  int& h = ball->prop<int>(PROP_H);
  float& vx = ball->prop<float>(PROP_VX);
  float& vy = ball->prop<float>(PROP_VY);

  float& px = paddle->prop<float>(PROP_X);
  float& py = paddle->prop<float>(PROP_Y);
  int& pw = paddle->prop<int>(PROP_W);
  int& ph = paddle->prop<int>(PROP_H);
  float& pvy = paddle->prop<float>(PROP_VY);

  SDL_Rect ballDestination {static_cast<int>(x + vx), static_cast<int>(y + vy), w, h};
  SDL_Rect paddleDestination {static_cast<int>(px), static_cast<int>(py + pvy), pw, ph};
  SDL_Rect result;
  if(SDL_IntersectRect(&ballDestination, &paddleDestination, &result))
  {
    if(result.w > result.h)
    {
      vy = -vy;
    }
    else
    {
      vx = -vx;
    }
  }
}
