#include "cabinet.h"
#include "typetuple.h"
#include <type_traits>
#include <vector>
#include <algorithm>

template<typename NodeBase>
class Nodedon
{
public:
  struct Node;
  using NodeRef = typename Cabinet<Node>::Pointer;

  struct Node : public NodeBase
  {
    using NodeBase::NodeBase;
    NodeRef parent;
    std::vector<NodeRef> children;
  };

  struct NodeAware
  {
    NodeRef node;
  };

  NodeRef insert(Node&& node)
  {
    return nodes.insert(std::forward<Node>(node));
  }

  NodeRef insert(Node&& node, NodeRef parent)
  {
    NodeRef ref = nodes.insert(std::forward<Node>(node));
    ref->parent = parent;
    parent->children.push_back(ref);
    return ref;
  }

  void remove(NodeRef node)
  {
    // Remove from parent's children
    if(node->parent)
    {
      auto& cs = node->parent->children;
      cs.erase(std::remove(cs.begin(), cs.end(), node));
    }

    // Remove own children
    for(NodeRef child : node->children)
    {
      child.remove();
    }

    // Remove self
    node.remove();
  }

  template<typename T, typename T2, typename... Ts>
  void setNode(T& t, T2& t2, Ts&... ts, NodeRef node)
  {
    setNode(t, node);
    setNode(t2, ts..., node);
  }

  template<typename T>
  static typename std::enable_if<std::is_base_of<NodeAware, T>::value, void>::type
  setNode(T& t, NodeRef node)
  {
    t.node = node;
  }
  template<typename T>
  static typename std::enable_if<!std::is_base_of<NodeAware, T>::value, void>::type
  setNode(T&, NodeRef)
  {
  }

private:
  Cabinet<Node> nodes;
};

