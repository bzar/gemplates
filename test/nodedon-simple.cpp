#include "nodedon.h"
#include <iostream>
struct NodeBase
{

};

struct Data0 {};
struct Data1 {};
struct Data2 {};
struct Data3 {};
struct Data4 {};
struct Data5 {};
struct Data6 {};
struct Data7 {};

using N = Nodedon<NodeBase, Data0, Data1, Data2, Data3, Data4, Data5, Data6, Data7>;

int main()
{
  N::Context ctx;

  auto root = ctx.add(N::Node{});
  ctx.add(root, Data0{});
  auto child0 = ctx.add(N::Node{}, root);
  ctx.add(child0, Data0{});
  auto child1 = ctx.add(N::Node{}, root);
  ctx.add(child1, Data1{});

  for(N::Node& node : ctx.get<N::Node>())
  {
    std::cout << "Node" << std::endl;
  }

  return 0;
}
