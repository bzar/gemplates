#include "cabinet.h"
#include <cassert>

int main()
{
  Cabinet<std::string> cabinet;

  auto ptr = cabinet.insert("Foobar");
  assert(*ptr == "Foobar");

  auto foo = cabinet.insert("Foo");
  auto bar = cabinet.insert("Bar");
  auto baz = cabinet.insert("Baz");

  cabinet.remove(0);

  assert(*foo == "Foo");
  assert(*bar == "Bar");
  assert(*baz == "Baz");

  bar.remove();

  assert(*foo == "Foo");
  assert(*baz == "Baz");

  baz.remove();

  assert(*foo == "Foo");

  foo.remove();

  assert(cabinet.size() == 0);

  return 0;
}

