#ifndef TYPETUPLE
#define TYPETUPLE

#include <type_traits>

template<typename T, typename... Ts>
class TypeTuple
{
public:
  template<typename TT>
  typename std::enable_if<std::is_same<T, TT>::value, T&>::type get()
  {
    return t;
  }
  template<typename TT>
  typename std::enable_if<!std::is_same<T, TT>::value, TT&>::type get()
  {
    return rest.template get<TT>();
  }

private:
  T t;
  TypeTuple<Ts...> rest;
};

template<typename T>
class TypeTuple<T>
{
public:
  template<typename TT>
  typename std::enable_if<std::is_same<T, TT>::value, T&>::type get()
  {
    return t;
  }
private:
  T t;
};
template<template<typename...> class Container, typename T, typename... Ts>
class ContainerTuple
{
public:
  template<typename TT>
  typename std::enable_if<std::is_same<T, TT>::value, Container<T>&>::type get()
  {
    return container;
  }
  template<typename TT>
  typename std::enable_if<!std::is_same<T, TT>::value, Container<TT>&>::type get()
  {
    return rest.template get<TT>();
  }

  template<typename TT, typename... TTs>
  void partialAssign(ContainerTuple<Container, TT, TTs...> const& other)
  {
    assign(other);
    rest.partialAssign(other);
  }

  Container<T> container;
  ContainerTuple<Container, Ts...> rest;
private:
  template<typename TT>
  typename std::enable_if<!std::is_same<T, TT>::value>::type assign(ContainerTuple<Container, TT> const&)
  {
  }
  template<typename TT, typename... TTs>
  typename std::enable_if<std::is_same<T, TT>::value>::type assign(ContainerTuple<Container, TT, TTs...> const& other)
  {
    container = other.container;
  }
  template<typename TT, typename TT2, typename... TTs>
  typename std::enable_if<!std::is_same<T, TT>::value>::type assign(ContainerTuple<Container, TT, TT2, TTs...> const& other)
  {
    assign(other.rest);
  }
};

template<template<typename...> class Container, typename T>
class ContainerTuple<Container, T>
{
public:
  template<typename TT>
  typename std::enable_if<std::is_same<T, TT>::value, Container<T>&>::type get()
  {
    return container;
  }
  template<typename TT, typename... TTs>
  void partialAssign(ContainerTuple<Container, TT, TTs...> const& other)
  {
    assign(other);
  }
  Container<T> container;
private:
  template<typename TT>
  typename std::enable_if<!std::is_same<T, TT>::value>::type assign(ContainerTuple<Container, TT> const&)
  {
  }
  template<typename TT, typename... TTs>
  typename std::enable_if<std::is_same<T, TT>::value>::type assign(ContainerTuple<Container, TT, TTs...> const& other)
  {
    container = other.container;
  }
  template<typename TT, typename TT2, typename... TTs>
  typename std::enable_if<!std::is_same<T, TT>::value>::type assign(ContainerTuple<Container, TT, TT2, TTs...> const& other)
  {
    assign(other.rest);
  }
};

template<template<typename...> class Container, template<typename> class Wrapper , typename T, typename... Ts>
class WrappingContainerTuple
{
public:
  template<typename TT>
  typename std::enable_if<std::is_same<T, TT>::value, Container<Wrapper<T>>&>::type get()
  {
    return container;
  }
  template<typename TT>
  typename std::enable_if<!std::is_same<T, TT>::value, Container<Wrapper<TT>>&>::type get()
  {
    return rest.template get<TT>();
  }

private:
  Container<Wrapper<T>> container;
  WrappingContainerTuple<Container, Wrapper, Ts...> rest;
};

template<template<typename...> class Container, template<typename> class Wrapper, typename T>
class WrappingContainerTuple<Container, Wrapper, T>
{
public:
  template<typename TT>
  Container<Wrapper<T>>& get()
  {
    static_assert(std::is_same<T, TT>::value, "no such type");
    return container;
  }
private:
  Container<Wrapper<T>> container;
};

#endif // TYPETUPLE

