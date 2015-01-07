#include "unrolled_list.h"
#include <iostream>

template<typename T>
void print(T const& ts)
{
  for(auto& t : ts)
  {
    std::cout << t << " ";
  }
  std::cout << std::endl;
}

int main()
{
  {
    UnrolledList<int, 4> l;
    auto r1 = l.emplace(1);
    auto r2 = l.emplace(2);
    auto r3 = l.emplace(3);
    auto r4 = l.emplace(4);
    auto r5 = l.emplace(5);

    l.remove(r2);

    r2 = l.emplace(2);

    l.remove(r1);
    l.remove(r2);
    l.remove(r5);
    l.remove(r4);
    l.remove(r3);
  }

  {
    UnrolledList<int, 128> l;

    for(int i = 0; i < 128*8; ++i)
    {
      l.insert(i);
    }

    long int sum = 0;
    for(auto& i : l)
    {
      sum += i;
    }
    std::cout << sum << " = " << 1024*(1024-1)/2 << std::endl;
  }

  {
    UnrolledList<int, 2> l;
    auto r1 = l.emplace(1);
    auto r2 = l.emplace(2);
    auto r3 = l.emplace(3);
    auto r4 = l.emplace(4);
    auto r5 = l.emplace(5);

    print(l);

    l.remove(r5);
    print(l);

    l.remove(r1);
    print(l);
    for(auto& i : l)
    {
      std::cout << i << " ";
    }
    std::cout << std::endl;

    l.remove(r3);
    print(l);

    l.remove(r4);
    print(l);

    l.remove(r2);
    print(l);
  }

  {
    UnrolledList<int, 2> l;
    auto r1 = l.emplace(1);
    auto r2 = l.emplace(2);
    auto r3 = l.emplace(3);
    auto r4 = l.emplace(4);
    auto r5 = l.emplace(5);

    print(l);

    l.remove(r1);
    print(l);

    r1 = l.emplace(10);
    print(l);

    l.remove(r2);
    l.remove(r3);
    l.remove(r5);

    r5 = l.emplace(50);
    r3 = l.emplace(30);
    r2 = l.emplace(20);

    print(l);

    l.remove(r5);
    l.remove(r4);
    l.remove(r3);
    l.remove(r2);
    l.remove(r1);

    l.emplace(1);
    l.emplace(2);
    l.emplace(3);
    l.emplace(4);
    l.emplace(5);
    l.emplace(6);
    l.emplace(7);

    print(l);

  }
  return 0;
}
