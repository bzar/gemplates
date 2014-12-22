#include <iostream>
#include <cassert>
#include <memory>
#include <chrono>

#include "eventful.h"

const int COUNT = 10000;

class Signaler
{
public:
  struct Sign { int i; };
  Eventful::Bus<Sign> signal;
  void sendZero() { signal.pub<Sign>({0}); }
};

class Incrementer
{
public:
  struct Result { int i; };
  Eventful::Bus<Result> signal;

  Incrementer() = default;
  // Subscription lambdas reference this -> no copying
  Incrementer(const Incrementer&) = delete;
  Incrementer& operator=(const Incrementer&) = delete;

  template<typename Source, typename Signal>
  void listen(Source& src)
  {
    subs.template get<Eventful::Sub<Signal>>() = src.signal.template sub<Signal>([this](Signal const& s) {
      signal.template pub<Result>({s.i + 1});
    });
  }

private:
  TypeTuple<
    Eventful::Sub<Signaler::Sign>,
    Eventful::Sub<Result>
  > subs;
};

template<typename Source, typename Signal>
class Aggregator
{
public:
  Aggregator() = default;
  // Subscription lambdas reference this -> no copying
  Aggregator(const Aggregator&) = delete;
  Aggregator& operator=(const Aggregator&) = delete;

  void listen(Source& src)
  {
    subs.push_back(src.signal.template sub<Signal>([&](Signal const& s) {
      total += s.i;
    }));
  }
  int total = 0;
private:
  std::vector<Eventful::Sub<Signal>> subs;
};

int main()
{
  auto t0 = std::chrono::steady_clock::now();
  Signaler s;
  std::vector<std::unique_ptr<Incrementer>> is;
  Aggregator<Incrementer, Incrementer::Result> a;

  is.push_back(std::unique_ptr<Incrementer>(new Incrementer));
  is.at(0)->template listen<Signaler, Signaler::Sign>(s);

  for(int i = 1; i < COUNT; ++i)
  {
    is.push_back(std::unique_ptr<Incrementer>(new Incrementer));
    Incrementer& inc = *is.at(i);
    Incrementer& pinc = *is.at(i - 1);
    inc.template listen<Incrementer, Incrementer::Result>(pinc);
    a.listen(inc);
  }
  auto t1 = std::chrono::steady_clock::now();

  s.sendZero();
  assert(a.total == 50004999);

  auto t2 = std::chrono::steady_clock::now();

  auto dt1 = std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0).count();
  auto dt2 = std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count();
  std::cout << "Created " << COUNT << " items long chain in " << dt1 << " ns (" << dt1/COUNT << " ns per node)" << std::endl;
  std::cout << "Processed " << COUNT << " items long chain in " << dt2 << " ns (" << dt2/COUNT << " ns per node)" << std::endl;

  return 0;
}
