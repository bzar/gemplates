#ifndef SCENERY_H
#define SCENERY_H

#include <unordered_map>
#include <vector>
#include <memory>
#include <string>
#include <algorithm>
#include <type_traits>

template<typename Scene, typename Key = std::string>
struct Scenery
{
  class Manager;

  struct ManagerAware
  {
    Manager* manager;
  };

  class Manager
  {
  public:
    template<typename... Args>
    void create(Key&& key, Args&&... args)
    {
      auto ret = scenes.emplace(std::make_pair(std::forward<Key>(key), std::unique_ptr<Scene>(new Scene{std::forward<Args>(args)...})));
      if(ret.second)
      {
        setManagerPointer(ret.first->second);
      }
    }

    void insert(Key&& key, Scene* scene)
    {
      auto ret = scenes.insert(std::make_pair(std::forward<Key>(key), std::unique_ptr<Scene>(scene)));
      if(ret.second)
      {
        setManagerPointer(ret.first->second);
      }
    }

    void remove(Key const& key)
    {
      auto iter = scenes.find(key);
      if(iter != scenes.end())
      {
        Scene* scene = iter->second.get();
        auto toRemove = std::remove_if(stack.begin(), stack.end(), [scene](Scene* s){ return s == scene; });
        if(toRemove != stack.end())
        {
          stack.erase(toRemove);
        }
        scenes.erase(iter);
      }
    }

    void push(Key const& key)
    {
      auto iter = scenes.find(key);
      if(iter == scenes.end())
        return;

      stack.push_back(iter->second.get());
    }

    void pop()
    {
      stack.pop_back();
    }

    void set(Key const& key)
    {
      auto iter = scenes.find(key);
      if(iter == scenes.end())
        return;

      stack.clear();
      stack.push_back(iter->second.get());
    }

    Scene* current()
    {
      if(stack.empty())
        return nullptr;

      return stack.back();
    }

  private:
    template<typename S>
    typename std::enable_if<std::is_base_of<ManagerAware, S>::value, void>::type
    setManagerPointer(std::unique_ptr<S>& s)
    {
      s->manager = this;
    }
    template<typename S>
    typename std::enable_if<!std::is_base_of<ManagerAware, S>::value, void>::type
    setManagerPointer(std::unique_ptr<S>&)
    {
    }

    std::vector<Scene*> stack;
    std::unordered_map<Key, std::unique_ptr<Scene>> scenes;
  };

};
#endif //SCENERY_H
