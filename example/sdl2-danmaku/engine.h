#ifndef ENGINE_H
#define ENGINE_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include <boost/variant.hpp>
#include <queue>
#include "scenery.h"
#include "eventful.h"
#include "nodedon.h"

struct KeyPress
{
  SDL_Keysym key;
};

struct KeyRelease
{
  SDL_Keysym key;
};

struct Update
{

};


class TextureMap
{
public:
  TextureMap() = default;
  TextureMap(SDL_Renderer* renderer);
  ~TextureMap();
  SDL_Texture* get(std::string const& name);
private:
  SDL_Renderer* renderer;
  std::unordered_map<std::string, SDL_Texture*> map;
};

struct Scene;
struct Entity;
struct Position;
struct Sprite;
struct Text;
struct Bullet;

using EventBus = Eventful::QueuedBus<KeyPress, KeyRelease, Update>;
using N = Nodedon<Entity, Position, Sprite, Text, Bullet>;
using Node = N::Pointer<N::Node>;

struct Entity
{
  using PropertyKey = std::string;
  using PropertyValue = boost::variant<float, int, bool>;
  using PropertyMap = std::unordered_map<PropertyKey, PropertyValue>;

  Entity() = default;
  Entity(std::initializer_list<PropertyMap::value_type>&& init);
  Entity(PropertyMap&& properties);

  template<typename T>
  T& prop(PropertyKey const& key)
  {
    return boost::get<T>(properties.at(key));
  }

  template<typename T, typename F>
  void on(F&& f);

  Scene* scene;
  PropertyMap properties;
  ContainerTuple<Eventful::Sub, KeyPress, KeyRelease, Update> subs;
};

struct Position
{
  float x, y, w, h, vx, vy;
};

struct Sprite : public N::NodeAware
{
  Sprite() = default;
  Sprite(SDL_Texture* texture, SDL_Rect&& src = {0,0,0,0}, SDL_Rect&& rect = {0,0,0,0});

  SDL_Texture* texture;
  SDL_Rect src;
  SDL_Rect rect;
  float opacity = 1.0f;
  float angle = 0;
  SDL_Point center = {0, 0};
  SDL_RendererFlip flip = SDL_FLIP_NONE;
};

struct Text : public N::NodeAware
{
  Text();
  Text(std::string const& text, SDL_Color color = {255,255,255,255});

  void update();

  std::string text;
  SDL_Color color;
  SDL_Texture* texture;
  SDL_Rect rect;

  static TTF_Font* font;
};

struct Bullet : public N::NodeAware
{
  using Behavior = std::function<void(Bullet&)>;
  Bullet() = default;
  Bullet(int life, Behavior&& behavior) : life(life), time(0), behavior(behavior) {}
  int life;
  int time;
  Behavior behavior;
};

using S = Scenery<Scene>;

struct Scene : public S::ManagerAware
{
  Scene(SDL_Renderer* renderer) : textureMap(renderer) {}
  using NodeKey = std::string;
  using Deferred = std::function<void()>;
  Node add(NodeKey const& key, N::Node&& node);
  Node add(NodeKey const& key, N::Node&& node, Node parent);

  template<typename Component, typename... Components>
  Node add(NodeKey const& key, N::Node&& node, Component&& c, Components&&... cs)
  {
    Node n = add(key, std::forward<N::Node>(node));
    nodes.add(n, std::forward<Component>(c), std::forward<Components...>(cs...));
    return n;
  }
  template<typename Component, typename... Components>
  Node add(NodeKey const& key, Node parent, N::Node&& node, Component&& c, Components&&... cs)
  {
    Node n = add(key, std::forward<N::Node>(node), parent);
    nodes.add(n, std::forward<Component>(c), std::forward<Components...>(cs...));
    return n;
  }

  Node addAnonymous(N::Node&& node);
  Node addAnonymous(Node parent, N::Node&& node);

  template<typename Component, typename... Components>
  Node addAnonymous(N::Node&& node, Component&& c, Components&&... cs)
  {
    Node n = addAnonymous(std::forward<N::Node>(node));
    nodes.add(n, std::forward<Component>(c), std::forward<Components>(cs)...);
    return n;
  }
  template<typename Component, typename... Components>
  Node addAnonymous(Node parent, N::Node&& node, Component&& c, Components&&... cs)
  {
    Node n = addAnonymous(parent, std::forward<N::Node>(node));
    nodes.add(n, std::forward<Component>(c), std::forward<Components>(cs)...);
    return n;
  }

  void remove(NodeKey const& key);

  void defer(Deferred&& d)
  {
    deferred.push(d);
  }

  N::Context nodes;
  EventBus bus;
  std::unordered_map<NodeKey, Node> nodesById;
  TextureMap textureMap;

  std::queue<Deferred> deferred;
};


void gameloop(S::Manager& scenery, SDL_Renderer* renderer);
bool input(Scene* scene);
bool update(Scene* scene);
void render(Scene* scene, SDL_Renderer* renderer);

template<typename T, typename F>
void Entity::on(F&& f)
{
  subs.template get<T>() = scene->bus.sub<T>(std::forward<F>(f));
}

#endif
