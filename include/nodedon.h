#include "cabinet.h"
#include "typetuple.h"
#include <type_traits>

template<typename NodeBase, typename... Components>
struct Nodedon
{
  template<typename T>
  using Pointer = typename Cabinet<T>::Pointer;

  struct Node : public NodeBase
  {
    using NodeBase::NodeBase;
    Pointer<Node> parent;
    std::vector<Pointer<Node>> children;
    ContainerTuple<Pointer, Components...> components;

    template<typename T>
    Pointer<T> get()
    {
      return components.template get<T>();
    }
  };

  struct NodeAware
  {
    Pointer<Node> node;
  };

  class Context
  {
  public:

    Pointer<Node> add(Node&& node, Pointer<Node> parent = {})
    {
      auto ptr = d.template get<Node>().insert(std::forward<Node>(node));

      if(parent)
      {
        ptr->parent = parent;
        parent->children.push_back(ptr);
      }
      return ptr;
    }
    template<typename T>
    Pointer<T> add(Pointer<Node>& node, T&& component)
    {
      auto ptr = d.template get<T>().insert(std::forward<T>(component));
      node->components.template get<T>() = ptr;
      setNodePointer<T>(ptr, node);
      return ptr;
    }
    template<typename C, typename... Cs>
    Pointer<Node> add(Pointer<Node>& node, C&& c, Cs&&... cs)
    {
      add<C>(node, std::forward<C>(c));
      add<Cs...>(node, std::forward<Cs>(cs)...);
      return node;
    }
    void remove(Pointer<Node> node)
    {
      for(Pointer<Node>& child : node->children)
      {
        remove(child);
      }
      helper<Components...>::removeComponents(node);
      node.remove();
    }

    template<typename T>
    Cabinet<T>& get()
    {
      return d.template get<T>();
    }

  private:
    template<typename T>
    typename std::enable_if<std::is_base_of<NodeAware, T>::value, void>::type
    setNodePointer(Pointer<T>& t, Pointer<Node>& node)
    {
      t->node = node;
    }
    template<typename T>
    typename std::enable_if<!std::is_base_of<NodeAware, T>::value, void>::type
    setNodePointer(Pointer<T>&, Pointer<Node>&)
    {
    }

    template<typename... Cs>
    struct helper
    {
      static void removeComponents(Pointer<Node>&)
      {
      }
    };

    template<typename C, typename... Cs>
    struct helper<C, Cs...>
    {
      static void removeComponents(Pointer<Node>& node)
      {
        node->components.template get<C>().remove();
        helper<Cs...>::removeComponents(node);
      }
    };


    ContainerTuple<Cabinet, Node, Components...> d;
  };
};
