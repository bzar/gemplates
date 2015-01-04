#include "bullets.h"
#include <iostream>
#include <cmath>

namespace
{
  SDL_Rect bulletRect(int size, int i)
  {
    if(size == 8)
      return { (i % 64) * size, size* (i/64), size, size };
    else
      return { i * size, size, size, size };

  }
}

Node createBullet8_0(Scene* scene, float x, float y, float dx, float dy)
{
  float v = sqrt(dx*dx + dy*dy);
  float nx = dx / v;
  float ny = dy / v;
  Node n = scene->addAnonymous({},
                               Position {x, y, 8, 8, dx, dy},
                               Sprite { scene->textureMap.get("img/bullets.png"),
                                        bulletRect(8, 0), {0, 0, 8, 8} },
                               Bullet { 128, [nx, ny, v](Bullet& b) {
                                          Position& p = *b.node->get<Position>();
                                          int const wl = 32;
                                          int const a = 2;
                                          float wv = ((b.time % wl) - wl/2) / static_cast<float>(wl/2);
                                          p.vx = nx*v + -ny*wv*a;
                                          p.vy = ny*v + nx*wv*a;
                                        }
                               });

  return n;
}


Node createBullet16_0(Scene* scene, float x, float y, float dx, float dy)
{
  Node n = scene->addAnonymous({},
                               Position {x, y, 16, 16, dx, dy},
                               Sprite { scene->textureMap.get("img/bullets.png"),
                                        bulletRect(16, 0), {0, 0, 16, 16} },
                               Bullet { 128, [dx, dy](Bullet& b) {
                                          if(b.time % 64 == 0)
                                          {
                                            Position& p = *b.node->get<Position>();
                                            Scene* scene = b.node->scene;
                                            auto x = p.x;
                                            auto y = p.y;
                                            scene->defer([scene, x, y, dx, dy] {
                                              createBullet8_0(scene, x, y, dy, -dx);
                                              createBullet8_0(scene, x, y, -dy, dx);
                                            });
                                          }
                                        }
                               });

  return n;
}


Node createBullet32_0(Scene* scene, float x, float y, float dx, float dy)
{
  Node n = scene->addAnonymous({},
                               Position {x, y, 32, 32, dx, dy},
                               Sprite { scene->textureMap.get("img/bullets.png"),
                                        bulletRect(32, 0), {0, 0, 32, 32} },
                               Bullet { 512, [dx, dy](Bullet& b) {
                                          if(b.time % 64 == 0)
                                          {
                                            Position& p = *b.node->get<Position>();
                                            Scene* scene = b.node->scene;
                                            auto x = p.x;
                                            auto y = p.y;
                                            scene->defer([scene, x, y, dx, dy] {
                                              createBullet16_0(scene, x, y, dy + dx/2, -dx);
                                              createBullet16_0(scene, x, y, -dy + dx/2, dx);
                                            });
                                          }
                                        }
                               });

  return n;
}


Node createBullet64_0(Scene* scene, float x, float y, float dx, float dy)
{
  return createBullet32_0(scene, x, y, dx, dy);
}


Node createBullet128_0(Scene* scene, float x, float y, float dx, float dy)
{
  return createBullet32_0(scene, x, y, dx, dy);
}


Node createBullet256_0(Scene* scene, float x, float y, float dx, float dy)
{
  return createBullet32_0(scene, x, y, dx, dy);
}
