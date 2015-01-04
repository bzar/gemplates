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
  typename std::enable_if<std::__not_<std::is_same<T, TT>>::value, TT&>::type get()
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
  typename std::enable_if<std::__not_<std::is_same<T, TT>>::value, Container<TT>&>::type get()
  {
    return rest.template get<TT>();
  }

private:
  Container<T> container;
  ContainerTuple<Container, Ts...> rest;
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
private:
  Container<T> container;
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
  typename std::enable_if<std::__not_<std::is_same<T, TT>>::value, Container<Wrapper<TT>>&>::type get()
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

