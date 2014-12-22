#include "cabinet.h"
#include "typetuple.h"
#include <boost/variant.hpp>

template<typename NodeBase, typename... NodeTypes>
struct Nodedon
{
  template<typename T>
  using Pointer = typename Cabinet<T>::Pointer;

  struct Node : public NodeBase
  {
    using NodeBase::NodeBase;
    Pointer<Node> parent;
    std::vector<Pointer<Node>> children;
    boost::variant<Pointer<NodeTypes>...> data;

    template<typename T>
    Pointer<T> get()
    {
      return boost::get<Pointer<T>>(data);
    }
  };

  class Context
  {
  public:

    template<typename T>
    Pointer<Node> add(Node&& node, T&& data, Pointer<Node> parent = {})
    {
      auto ptr = d.template get<Node>().insert(std::forward<Node>(node));
      ptr->data = d.template get<T>().insert(std::forward<T>(data));

      if(parent)
      {
        ptr->parent = parent;
        parent->children.push_back(ptr);
      }
      return ptr;
    }

    template<typename T>
    Cabinet<T>& get()
    {
      return d.template get<T>();
    }

  private:
    ContainerTuple<Cabinet, Node, NodeTypes...> d;
  };
};
