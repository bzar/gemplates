#include <iostream>
#include <cassert>
#include <chrono>
#include <vector>

#include "eventful.h"
struct E0 { long unsigned int x; };
struct E1 { long unsigned int x; };
struct E2 {};
struct E3 { long unsigned int x; };
struct E4 {};
struct E5 {};
struct E6 {};
struct E7 {};
struct E8 {};
struct E9 {};
struct E10 {};
struct E11 {};
struct E12 {};
struct E13 {};
struct E14 {};
struct E15 {};

unsigned long int global = 0;
void handleE0(E0 const& e)
{
  global += e.x;
}

unsigned int COUNT = 1000000;

class Counter
{
public:
  void count(E3 const& e) { c += e.x; }
  long unsigned int getCount() const { return c; }
private:
  long unsigned int c = 0;
};

int main()
{
  Eventful::Bus<E0, E1, E2, E3, E4, E5, E6, E7, E8, E9, E10, E11, E12, E13, E14, E15> bus;


  // Add extra listeners to make sure they don't add overhead
  std::vector<Eventful::Sub<E12>> extraSubs12;
  for(int i = 0; i < 50; ++i)
  {
    extraSubs12.push_back(bus.sub<E12>([](E12 const&){}));
  }
  std::vector<Eventful::Sub<E13>> extraSubs13;
  for(int i = 0; i < 50; ++i)
  {
    extraSubs13.push_back(bus.sub<E13>([](E13 const&){}));
  }
  std::vector<Eventful::Sub<E14>> extraSubs14;
  for(int i = 0; i < 50; ++i)
  {
    extraSubs14.push_back(bus.sub<E14>([](E14 const&){}));
  }
  std::vector<Eventful::Sub<E15>> extraSubs15;
  for(int i = 0; i < 50; ++i)
  {
    extraSubs15.push_back(bus.sub<E15>([](E15 const&){}));
  }

  // Test performance with function handler
  {
    Eventful::Sub<E0> e0sub = bus.sub<E0>(handleE0);
    auto t0 = std::chrono::steady_clock::now();

    global = 0;
    for(long unsigned int i = 0; i < COUNT; ++i)
    {
      bus.pub(E0{i});
    }
    auto t1 = std::chrono::steady_clock::now();
    auto dt = std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0).count();
    std::cout << "Handling " << COUNT << " trivial events with function handler took " << dt << " ns (" << dt/COUNT << " ns per event)" << std::endl;
    assert(global == 499999500000l);
  }

  // Test performance with lambda handler
  {
    unsigned long int counter = 0;
    auto e1sub = bus.sub<E1>([&counter](E1 const& e) { counter += e.x; });

    auto t0 = std::chrono::steady_clock::now();

    for(long unsigned int i = 0; i < COUNT; ++i)
    {
      bus.pub(E1{i});
    }
    auto t1 = std::chrono::steady_clock::now();
    auto dt = std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0).count();

    std::cout << "Handling " << COUNT << " trivial events with lambda handler took " << dt << " ns (" << dt/COUNT << " ns per event)" << std::endl;
    assert(counter == 499999500000l);
  }

  // Test performance with member function handler
  {
    Counter counter;
    auto e3sub = bus.sub<E3>(&counter, &Counter::count);

    auto t0 = std::chrono::steady_clock::now();

    for(long unsigned int i = 0; i < COUNT; ++i)
    {
      bus.pub(E3{i});
    }
    auto t1 = std::chrono::steady_clock::now();
    auto dt = std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0).count();

    std::cout << "Handling " << COUNT << " trivial events with member function handler took " << dt << " ns (" << dt/COUNT << " ns per event)" << std::endl;
    assert(counter.getCount() == 499999500000l);
  }
  return 0;
}

