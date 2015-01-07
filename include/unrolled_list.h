#ifndef UNROLLED_LIST_H
#define UNROLLED_LIST_H

#include <array>
#include <deque>
#include <type_traits>

template<typename T, std::size_t NODE_SIZE = 32>
class UnrolledList
{
private:
  struct Element;
  struct Node;

public:
  class Reference
  {
  public:
    Reference() = default;
    Reference(Reference const&) = default;
    Reference(Reference&&) = default;
    Reference& operator=(Reference const&) = default;
    Reference& operator=(Reference&&) = default;
    operator bool() const;

    T& operator*() const;
    T* operator->() const;

  private:
    Reference(Element* e) : element(e) {}
    friend class UnrolledList<T, NODE_SIZE>;
    Element* element = nullptr;
  };

  class iterator
  {
  public:
    typedef std::forward_iterator_tag iterator_type;
    typedef T value_type;
    typedef T* pointer;
    typedef const T* const_pointer;
    typedef T& reference;
    typedef const T& const_reference;
    typedef std::size_t size_type;
    typedef std::size_t difference_type;

    iterator() = default;
    iterator(iterator const& other) = default;
    iterator& operator=(iterator const& other) = default;

    bool operator==(iterator const& other);
    bool operator!=(iterator const& other);

    reference operator*();
    pointer operator->();

    iterator& operator++();
    iterator operator++(int c);
  private:
    friend class UnrolledList<T, NODE_SIZE>;
    inline Element* element() const;
    iterator(Node* n, size_type i) : n(n), i(i) {}
    Node* n = nullptr;
    size_type i = 0;
  };
  UnrolledList() = default;
  UnrolledList(UnrolledList const&) = delete;
  UnrolledList& operator=(UnrolledList const&) = delete;
  UnrolledList(UnrolledList&&) = default;
  UnrolledList& operator=(UnrolledList&&) = default;
  ~UnrolledList();

  Reference emplace(T&& t);
  Reference insert(T const& t);
  void remove(Reference const& r);
  iterator begin() const;
  iterator end() const;
  std::size_t size() const;

private:
  struct Element
  {
    bool deleted = true;
    T data;
  };

  struct Node
  {
    std::array<Element, NODE_SIZE> elements;
    std::size_t size = 0;
    Node* next = nullptr;
  };

  Element* getFreeElement();

  std::size_t _size = 0;
  Node* _first = nullptr;
  Node* _last = nullptr;
  iterator _begin;
  iterator _end;
  std::deque<Element*> _free;
};

template<typename T, std::size_t NODE_SIZE>
UnrolledList<T, NODE_SIZE>::~UnrolledList()
{
  Node* n = _first;
  while(n)
  {
    Node* p = n;
    n = n->next;
    delete p;
  }

}

template<typename T, std::size_t NODE_SIZE>
typename UnrolledList<T, NODE_SIZE>::Reference UnrolledList<T, NODE_SIZE>::emplace(T&& t)
{
  Element* e = getFreeElement();
  e->data = std::forward<T>(t);
  return e;
}

template<typename T, std::size_t NODE_SIZE>
typename UnrolledList<T, NODE_SIZE>::Reference UnrolledList<T, NODE_SIZE>::insert(T const& t)
{
  Element* e = getFreeElement();
  e->data = t;
  return e;
}
template<typename T, std::size_t NODE_SIZE>
void UnrolledList<T, NODE_SIZE>::remove(UnrolledList<T, NODE_SIZE>::Reference const& r)
{
  r.element->deleted = true;
  _free.push_back(r.element);
  --_size;
}

template<typename T, std::size_t NODE_SIZE>
typename UnrolledList<T, NODE_SIZE>::iterator UnrolledList<T, NODE_SIZE>::begin() const
{
  // First element
  if(_size > 0 && !_begin.element()->deleted)
  {
    return _begin;
  }
  // First element deleted
  else if(_size > 0)
  {
    iterator b = _begin;
    return ++b;
  }
  // Empty container
  else
  {
    return _end;
  }
}

template<typename T, std::size_t NODE_SIZE>
typename UnrolledList<T, NODE_SIZE>::iterator UnrolledList<T, NODE_SIZE>::end() const
{
  return _end;
}

template<typename T, std::size_t NODE_SIZE>
std::size_t UnrolledList<T, NODE_SIZE>::size() const
{
  return _size;
}
template<typename T, std::size_t NODE_SIZE>
typename UnrolledList<T, NODE_SIZE>::Element* UnrolledList<T, NODE_SIZE>::getFreeElement()
{
  Element* e;
  // Insert to previously removed element
  if(!_free.empty())
  {
    e = _free.front();
    _free.pop_front();
  }
  // Insert to an existing node
  else if(_last && _last->size < NODE_SIZE)
  {
    e = &_last->elements[_last->size];
    ++_last->size;
    if(_last->size == NODE_SIZE)
    {
      _end.i = 0;
      _end.n = nullptr;
    }
    else
    {
      _end.i = _last->size;
    }
  }
  // Insert to a new node
  else if(_last)
  {
    _last->next = new Node;
    _last = _last->next;
    _end = {_last, 1};
    e = &_last->elements[0];
    _last->size = 1;
  }
  // First insert
  else
  {
    _first = new Node;
    _last = _first;
    _begin = {_first, 0};
    _end = {_first, 1};
    e = &_last->elements[0];
    _last->size = 1;
  }

  e->deleted = false;
  ++_size;

  return e;
}

template<typename T, std::size_t NODE_SIZE>
T& UnrolledList<T, NODE_SIZE>::Reference::operator*() const
{
  return element->data;
}

template<typename T, std::size_t NODE_SIZE>
T* UnrolledList<T, NODE_SIZE>::Reference::operator->() const
{
  return &element->data;
}

template<typename T, std::size_t NODE_SIZE>
UnrolledList<T, NODE_SIZE>::Reference::operator bool() const
{
  return element != nullptr;
}

template<typename T, std::size_t NODE_SIZE>
bool UnrolledList<T, NODE_SIZE>::iterator::operator==(UnrolledList<T, NODE_SIZE>::iterator const& other)
{
  return n == other.n && (n == nullptr || i == other.i);
}

template<typename T, std::size_t NODE_SIZE>
bool UnrolledList<T, NODE_SIZE>::iterator::operator!=(UnrolledList<T, NODE_SIZE>::iterator const& other)
{
  return !operator==(other);
}

template<typename T, std::size_t NODE_SIZE>
typename UnrolledList<T, NODE_SIZE>::iterator::reference UnrolledList<T, NODE_SIZE>::iterator::operator*()
{
  return n->elements[i].data;
}

template<typename T, std::size_t NODE_SIZE>
typename UnrolledList<T, NODE_SIZE>::iterator::pointer UnrolledList<T, NODE_SIZE>::iterator::operator->()
{
  return &n->elements[i].data;
}

template<typename T, std::size_t NODE_SIZE>
typename UnrolledList<T, NODE_SIZE>::iterator& UnrolledList<T, NODE_SIZE>::iterator::operator++()
{
  do
  {
    ++i;
    if(i == NODE_SIZE)
    {
      i = 0;
      n = n->next;
    }
  } while(n != nullptr && n->elements[i].deleted && i < n->size);

  return *this;
}
template<typename T, std::size_t NODE_SIZE>
typename UnrolledList<T, NODE_SIZE>::iterator UnrolledList<T, NODE_SIZE>::iterator::operator++(int c)
{
  iterator iter(*this);
  for(int i = 0; i < c; ++i)
  {
    ++iter;
  }
  return iter;
}
template<typename T, std::size_t NODE_SIZE>
typename UnrolledList<T, NODE_SIZE>::Element* UnrolledList<T, NODE_SIZE>::iterator::element() const
{
  if(!n)
  {
    return nullptr;
  }
  else
  {
    return &n->elements[i];
  }

}

#endif // UNROLLED_LIST_H
