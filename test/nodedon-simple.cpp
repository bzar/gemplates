#include "nodedon.h"
#include <cassert>
struct NodeBase
{

};

int main()
{
  Nodedon<NodeBase> n;

  auto root = n.insert({});
  auto child0 = n.insert({}, root);
  auto child1 = n.insert({}, root);

  assert(root->children.size() == 2);
  assert(child0->parent == root);
  assert(child0->parent != child1);
  assert(child0->parent == child1->parent);

  for(int i = 0; i < 100; ++i)
  {
    n.insert({}, child1);
  }

  assert(child0->children.size() == 0);
  assert(child1->children.size() == 100);

  n.remove(child0);

  assert(root->children.size() == 1);

  n.remove(root);

  return 0;
}
