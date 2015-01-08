#ifndef COMPONENTSTORAGE_H
#define COMPONENTSTORAGE_H

#include "typetuple.h"
#include "unrolled_list.h"

template<typename C>
class ComponentContainer
{
public:
  ComponentContainer() = default;
  ComponentContainer(ComponentContainer const&) = delete;
  ComponentContainer(ComponentContainer&&) = delete;
  ComponentContainer& operator=(ComponentContainer const&) = delete;
  ComponentContainer& operator=(ComponentContainer&&) = delete;

  class Reference
  {
  public:
    Reference() : container(nullptr), position() {}

    Reference(Reference const&) = default;
    Reference(Reference&&) = default;
    Reference& operator=(Reference const&) = default;
    Reference& operator=(Reference&&) = default;

    C& operator*() const
    {
      return *position;
    }
    C* operator->() const
    {
      return &operator*();
    }
    operator bool() const
    {
      return container != nullptr;
    }
  private:
    friend class ComponentContainer;

    using InternalRef = typename UnrolledList<C>::Reference;
    Reference(ComponentContainer* ctr, InternalRef pos) : container(ctr), position(pos) {}

    ComponentContainer<C>* container;
    InternalRef position;
  };

  Reference insert(C&& c)
  {
    auto ref = content.emplace(std::forward<C>(c));
    return { this, ref };
  }

  void remove(Reference ref)
  {
    content.remove(ref.position);
  }

  using iterator = typename UnrolledList<C>::iterator;
  iterator begin() const
  {
    return content.begin();
  }
  iterator end() const
  {
    return content.end();
  }

private:
  friend class Reference;
  UnrolledList<C> content;
};



template<typename... Components>
class ComponentStorage
{
public:
  template<typename C>
  using Reference = typename ComponentContainer<C>::Reference;

  template<typename... Cs>
  using ReferenceTuple = ContainerTuple<Reference, Cs...>;

  template<typename C>
  Reference<C> insert(C&& c)
  {
    ComponentContainer<C>& ctr = containers.template get<C>();
    return ctr.insert(std::forward<C>(c));
  }

  template<typename... Cs>
  ReferenceTuple<Cs...> insert(Cs&&... cs)
  {
    ReferenceTuple<Cs...> ct;
    helper<decltype(ct), Cs...>::insert(containers, ct, std::forward<Cs>(cs)...);
    return ct;
  }

  template<typename C>
  ComponentContainer<C>& get()
  {
    return containers.template get<C>();
  }

  template<typename C>
  void remove(Reference<C> ref)
  {
    get<C>().remove(ref);
  }

private:
  using Containers = ContainerTuple<ComponentContainer, Components...>;

  template<typename CT, typename... Cs>
  struct helper
  {
    static void insert(Containers&, CT&, Cs&&...)
    {
    }
  };
  template<typename CT, typename C, typename... Cs>
  struct helper<CT, C, Cs...>
  {
    static void insert(Containers& ctrs, CT& ct, C&& c, Cs&&... cs)
    {

      auto& ctr = ctrs.template get<C>();
      ct.template get<C>() = ctr.insert(std::forward<C>(c));
      helper<CT, Cs...>::insert(ctrs, ct, std::forward<Cs>(cs)...);
    }
  };

  Containers containers;
};

#endif // COMPONENTSTORAGE_H

