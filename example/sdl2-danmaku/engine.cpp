#include "engine.h"

Node Scene::add(const Scene::NodeKey& key, N::Node&& node)
{
  auto ptr = nodes.add(std::forward<N::Node>(node));
  nodesById.emplace(std::make_pair(key, ptr));
  ptr->scene = this;
  return ptr;
}

Node Scene::addAnonymous(N::Node&& node)
{
  auto ptr = nodes.add(std::forward<N::Node>(node));
  ptr->scene = this;
  return ptr;
}

void gameloop(S::Manager& scenery, SDL_Renderer* renderer)
{

  bool running = true;
  Scene* scene;
  while(running && (scene = scenery.current()))
  {
    render(scene, renderer);
    SDL_Delay(16);
    running = input(scene) && update(scene);
  }
}

bool input(Scene* scene)
{
  SDL_Event e;
  while(SDL_PollEvent(&e))
  {
    if(e.type == SDL_QUIT)
    {
      return false;
    }
    else if(e.type == SDL_KEYDOWN && !e.key.repeat)
    {
      scene->bus.pub<KeyPress>({e.key.keysym});
    }
    else if(e.type == SDL_KEYUP && !e.key.repeat)
    {
      scene->bus.pub<KeyRelease>({e.key.keysym});
    }
  }
  return true;
}

bool update(Scene* scene)
{
  for(auto& d : scene->deferred)
  {
    d();
  }
  scene->deferred.clear();

  scene->bus.process();

  Cabinet<Bullet>& bullets = scene->nodes.get<Bullet>();

  for(unsigned int i = 0; i < bullets.size(); ++i)
  {
    Bullet& bullet = bullets.at(i);
    --bullet.life;

    if(bullet.life == 0)
    {
      scene->nodes.remove(bullet.node);
      --i;
      continue;
    }

    bullet.time += 1;
    bullet.behavior(bullet);
  }

  scene->bus.immediate<Update>({});

  for(Position& pos : scene->nodes.get<Position>())
  {
    pos.x += pos.vx;
    pos.y += pos.vy;
  }

  for(Sprite& sprite : scene->nodes.get<Sprite>())
  {
    Position& pos = *sprite.node->get<Position>();
    pos.w = sprite.rect.w;
    pos.h = sprite.rect.h;
    sprite.rect.x = static_cast<int>(pos.x - pos.w/2);
    sprite.rect.y = static_cast<int>(pos.y - pos.w/2);
  }
  for(Text& text : scene->nodes.get<Text>())
  {
    Position& pos = *text.node->get<Position>();
    text.rect.x = static_cast<int>(pos.x);
    text.rect.y = static_cast<int>(pos.y);
  }    
  return true;
}

void render(Scene* scene, SDL_Renderer* renderer)
{
  SDL_SetRenderDrawColor(renderer, 232, 232, 232, 255);
  SDL_RenderClear(renderer);

  for(auto& sprite : scene->nodes.get<Sprite>())
  {
#ifdef DEBUG_DRAW
    SDL_SetRenderDrawColor(renderer, 255, 0, 255, 255);
    SDL_RenderDrawRect(renderer, &sprite->rect);
#endif
    SDL_SetTextureAlphaMod(sprite.texture, 255 * sprite.opacity);
    if(sprite.src.w)
    {
      SDL_RenderCopy(renderer, sprite.texture, &sprite.src, &sprite.rect);
    }
    else
    {
      SDL_RenderCopy(renderer, sprite.texture, nullptr, &sprite.rect);
    }
  }

  // Ugly font init on first render :P
  if(!Text::font)
  {
    Text::font = TTF_OpenFont( "ttf/DejaVuSans.ttf", 28 );
  }
  for(auto& text : scene->nodes.get<Text>())
  {

    if(!text.texture && !text.text.empty())
    {
      SDL_Surface* surface = TTF_RenderText_Solid(text.font, text.text.data(), text.color);
      text.rect.w = surface->w;
      text.rect.h = surface->h;
      text.node->get<Position>()->w = surface->w;
      text.node->get<Position>()->h = surface->h;
      text.texture = SDL_CreateTextureFromSurface(renderer, surface);
      SDL_FreeSurface(surface);
    }
    if(text.texture)
    {
#ifdef DEBUG_DRAW
      SDL_SetRenderDrawColor(renderer, 255, 0, 255, 255);
      SDL_RenderDrawRect(renderer, &text.rect);
#endif
      SDL_RenderCopy(renderer, text.texture, NULL, &text.rect);
    }

  }

  SDL_RenderPresent(renderer);
}

Entity::Entity(std::initializer_list<Entity::PropertyMap::value_type>&& init) : properties(std::forward<std::initializer_list<PropertyMap::value_type>>(init))
{
}

Entity::Entity(Entity::PropertyMap&& properties) : properties(properties)
{
}
Sprite::Sprite(SDL_Texture* texture, SDL_Rect&& src, SDL_Rect&& rect) : texture(texture), src(src), rect(rect)
{
  if(rect.w == 0)
  {
    SDL_QueryTexture(texture, NULL, NULL, &this->rect.w, &this->rect.h);
  }
}

Text::Text()
{
}

Text::Text(std::string const& text, SDL_Color color) :
  text(text), color(color), texture(nullptr), rect{0, 0, 0, 0}
{
}

void Text::update()
{
  if(texture)
  {
    SDL_DestroyTexture(texture);
    texture = nullptr;
  }
}
TTF_Font* Text::font = nullptr;

TextureMap::TextureMap(SDL_Renderer* renderer) : renderer(renderer)
{

}

TextureMap::~TextureMap()
{
  for(auto& entry : map)
  {
    SDL_DestroyTexture(entry.second);
  }
  map.clear();
}

SDL_Texture* TextureMap::get(std::string const& name)
{
  SDL_Texture* texture = nullptr;

  auto iter = map.find(name);
  if(iter != map.end())
  {
    texture = iter->second;
  }
  else
  {
    SDL_Surface* surface = IMG_Load(name.data());
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    map.insert(std::make_pair(name, texture));
    SDL_FreeSurface(surface);
  }

  return texture;
}

