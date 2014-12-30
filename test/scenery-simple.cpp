#include <iostream>
#include <unordered_map>
#include <vector>
#include <memory>
#include <string>
#include <algorithm>
#include "scenery.h"

struct Scene
{
  int x;
};

int main()
{
  Scenery<Scene>::Manager scenery;
  scenery.create("first", 1);
  scenery.push("first");
  scenery.insert("second", new Scene{2});
  scenery.create("third", 3);
  std::cout << scenery.current()->x << " = " << 1 << std::endl;
  scenery.push("third");
  std::cout << scenery.current()->x << " = " << 3 << std::endl;
  scenery.push("second");
  std::cout << scenery.current()->x << " = " << 2 << std::endl;
  scenery.pop();
  std::cout << scenery.current()->x << " = " << 3 << std::endl;
  scenery.pop();
  std::cout << scenery.current()->x << " = " << 1 << std::endl;
  scenery.remove("first");
  std::cout << scenery.current() << " = " << 0 << std::endl;
  scenery.remove("second");

  return 0;
}
