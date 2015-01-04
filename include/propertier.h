#ifndef PROPERTIER_H
#define PROPERTIER_H

#include <functional>
#include <boost/variant.hpp>
#include <typetuple.h>
#include <vector>
#include <unordered_map>

template<typename Key, typename... Properties>
class Propertier
{
public:
  using Index = std::vector::size_type;
  template<typename T>
  using Getter = std::function<T&()>;
  template<typename T>
  using Setter = std::function<void(T const&)>;

  template<typename T>
  Index create(Key&& key, T&& value)
  {
    values.template get<T>().emplace(value);
  }
  template<typename T>
  Index create(Key&& key, Getter&& getter, Setter&& setter)
  {
    values.template get<T>().emplace(Virtual{getter, setter});
  }

private:
  template<typename T>
  struct Virtual
  {
    Getter<T> get;
    Setter<T> set;
  };

  template<typename T>
  using Value = boost::variant<T, Virtual<T>>;

  std::unordered_map<Key, Index> keys;
  WrappingContainerTuple<std::vector, Value, Properties...> values;
};

#endif // PROPERTIER_H

