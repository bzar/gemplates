#include "componentstorage.h"
#include <cassert>
#include <iostream>

struct C0 { int i; };
struct C1 { float f; };
struct C2 { bool b; };
struct C3 { char c; };

int main()
{
  ComponentStorage<C0, C1, C2, C3> stg;

  auto ref = stg.insert(C0 { 42 });

  assert(ref->i == 42);

  stg.insert(C1 { 1.2 },
             C2 { false });

  auto refs = stg.insert(C0 { 1 },
                         C1 { 0.5 },
                         C2 { true },
                         C3 { 'B' });

  assert(refs.get<C0>()->i == 1);
  assert(refs.get<C1>()->f > 0.49 && refs.get<C1>()->f < 0.51);
  assert(refs.get<C2>()->b == true);
  assert(refs.get<C3>()->c == 'B');

  int sum = 0;
  for(C0& c0 : stg.get<C0>())
  {
    sum += c0.i;
  }
  assert(sum == 43);

  return 0;
}
