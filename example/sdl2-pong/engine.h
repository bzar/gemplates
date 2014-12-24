#ifndef ENGINE_H
#define ENGINE_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include <boost/variant.hpp>
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

struct Entity;
struct Position;
struct Sprite;
struct Text;
using EventBus = Eventful::QueuedBus<KeyPress, KeyRelease, Update>;
using N = Nodedon<Entity, Position, Sprite, Text>;
using Node = N::Pointer<N::Node>;
struct Scene
{
  using NodeKey = std::string;
  Node add(NodeKey const& key, N::Node&& node);

  N::Context nodes;
  EventBus bus;
  std::unordered_map<NodeKey, Node> nodesById;
};

enum PropertyName { PROP_VALUE, PROP_OLD_VALUE };
struct Entity
{
  using PropertyKey = int;
  using PropertyValue = boost::variant<float, int>;
  using PropertyMap = std::unordered_map<PropertyKey, PropertyValue>;

  Entity() = default;
  Entity(std::initializer_list<PropertyMap::value_type>&& init);
  Entity(PropertyMap&& properties);

  template<typename T>
  T& prop(PropertyKey const& key)
  {
    return boost::get<T>(properties.at(key));
  }
  template<typename T>
  void prop(PropertyKey const& key, PropertyValue&& value)
  {
    properties.insert(std::make_pair(key, std::forward<PropertyValue>(value)));
  }

  template<typename T, typename F>
  void on(Scene* scene, F&& f)
  {
    subs.template get<T>() = scene->bus.sub<T>(std::forward<F>(f));
  }
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
  Sprite(SDL_Texture* texture);

  SDL_Texture* texture;
  SDL_Rect rect;
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

class TextureMap
{
public:
  TextureMap(SDL_Renderer* renderer);
  ~TextureMap();
  SDL_Texture* get(std::string const& name);
private:
  SDL_Renderer* renderer;
  std::unordered_map<std::string, SDL_Texture*> map;
};

void gameloop(Scenery<Scene>& scenery, SDL_Renderer* renderer);
bool input(Scene* scene);
bool update(Scene* scene);
void render(Scene* scene, SDL_Renderer* renderer);

#endif
