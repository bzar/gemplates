#ifndef EVENTFUL_H
#define EVENTFUL_H

#include <functional>
#include <memory>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <queue>

#include "typetuple.h"

namespace Eventful
{
  template<typename T>
  using Fun = std::function<void(T const&)>;

  template<typename T>
  class Sub
  {
  public:
    Sub() = default;
    Sub(Fun<T>&& fun) : fun(std::make_shared<Fun<T>>(fun))
    {
    }

    void unsub()
    {
      fun.reset(nullptr);
    }

    std::weak_ptr<Fun<T>> wptr() const
    {
      return fun;
    }

  private:
    std::shared_ptr<Fun<T>> fun;
  };

  template<typename... EventTypes>
  class Bus
  {
  public:
    Bus() = default;
    ~Bus() = default;

    template<typename T>
    Sub<T> sub(Fun<T>&& fun)
    {
      return _sub<T>(std::forward<Fun<T>>(fun));
    }

    template<typename T, typename S>
    Sub<T> sub(S* subscriber, void (S::*member)(T const&))
    {
      return _sub<T>([subscriber, member](T const& t) { (subscriber->*member)(t); });
    }

    template<typename T>
    void pub(T const& t)
    {
      bool hasRemoved = false;
      for(FunPointer<T>& wptr : funs.template get<T>())
      {

        if(auto sptr = wptr.lock())
        {
          (*sptr)(t);
        }
        else
        {
          hasRemoved = true;
        }
      }

      if(hasRemoved)
      {
        auto fs = funs.template get<T>();
        fs.erase(std::remove_if(fs.begin(), fs.end(), [](FunPointer<T> f){return f.expired(); }));
      }
    }

  private:
    template<typename EventType>
    using FunPointer = std::weak_ptr<Fun<EventType>>;
    template<typename T>
    using FunContainer = std::vector<T>;

    typedef WrappingContainerTuple<FunContainer, FunPointer, EventTypes...> Funs;

    template<typename T>
    Sub<T> _sub(Fun<T>&& fun)
    {
      Sub<T> s(std::forward<Fun<T>>(fun));
      funs.template get<T>().push_back(s.wptr());

      return s;
    }
    Funs funs;
  };

  template<typename... EventTypes>
  class QueuedBus
  {
  public:
    QueuedBus() = default;
    ~QueuedBus() = default;

    template<typename T>
    Sub<T> sub(Fun<T>&& fun)
    {
      return bus.sub(std::forward<Fun<T>>(fun));
    }

    template<typename T, typename S>
    Sub<T> sub(S* subscriber, void (S::*member)(T const&))
    {
      return bus.sub(subscriber, member);
    }

    template<typename T>
    void pub(T const& t)
    {
      queues.template get<T>().push(t);
    }

    template<typename T>
    void immediate(T const& t)
    {
      bus.template pub<T>(t);
    }

    int process(int n = -1)
    {
      return Helper<EventTypes...>::process(bus, queues, n);
    }

    void forward(QueuedBus<EventTypes...>& other)
    {
      Helper<EventTypes...>::forward(queues, other);
    }

  private:
    template<typename...Ts>
    struct Helper
    {
      static int process(Bus<EventTypes...>&, ContainerTuple<std::queue, EventTypes...>&, int, int i = 0)
      {
        return i;
      }
      static void forward(ContainerTuple<std::queue, EventTypes...>&, QueuedBus<EventTypes...>&)
      {

      }
    };

    template<typename T, typename... Ts>
    struct Helper<T, Ts...>
    {
      static int process(Bus<EventTypes...>& bus, ContainerTuple<std::queue, EventTypes...>& queues, int n, int i = 0)
      {
        std::queue<T>& queue = queues.template get<T>();
        while((n < 0 || i < n) && !queue.empty())
        {
          T const& t = queue.front();
          bus.template pub<T>(t);
          queue.pop();
          ++i;
        }
        if(i == n)
        {
          return i;
        }
        else
        {
          return Helper<Ts...>::process(bus, queues, n, i);
        }
      }
      static void forward(ContainerTuple<std::queue, EventTypes...>& queues, QueuedBus<EventTypes...>& other)
      {
        std::queue<T>& queue = queues.template get<T>();
        while(!queue.empty())
        {
          T const& t = queue.front();
          other.template pub<T>(t);
        }
        queue.clear();
        return Helper<Ts...>::process(queues, other);
      }
    };

    Bus<EventTypes...> bus;
    ContainerTuple<std::queue, EventTypes...> queues;
  };

}

#endif // EVENTFUL_H
