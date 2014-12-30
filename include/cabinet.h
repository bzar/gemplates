#include <vector>
#include <unordered_map>

template<typename T>
class Cabinet
{
private:
  struct Folder
  {
    std::size_t id;
    T content;
  };
public:
  typedef T value_type;
  typedef T* pointer;
  typedef const T* const_pointer;
  typedef T& reference;
  typedef const T& const_reference;
  typedef size_t size_type;
  typedef size_t difference_type;

  class iterator
  {
  public:
    typedef std::forward_iterator_tag iterator_type;
    typedef Cabinet::value_type value_type;
    typedef Cabinet::pointer pointer;
    typedef Cabinet::const_pointer const_pointer;
    typedef Cabinet::reference reference;
    typedef Cabinet::const_reference const_reference;
    typedef Cabinet::size_type size_type;
    typedef Cabinet::difference_type difference_type;

    iterator() : i() {}
    iterator(iterator const& other) = default;
    iterator& operator=(iterator const& other) = default;

    bool operator==(iterator const& other) { return i == other.i; }
    bool operator!=(iterator const& other) { return i != other.i; }

    reference operator*() { return i->content; }
    pointer operator->() { return &(operator*()); }

    iterator& operator++() { ++i; return *this; }
    iterator operator++(int c) { return iterator(i.operator++(c)); }
  private:
    friend class Cabinet<T>;
    iterator(typename std::vector<Folder>::iterator i) : i(i) {}
    typename std::vector<Folder>::iterator i;
  };

  Cabinet() = default;
  Cabinet(Cabinet const&) = delete;
  Cabinet& operator=(Cabinet const&) = delete;

  class Pointer
  {
  public:
    Pointer() : cabinet(nullptr), id(0), version(0), cached(nullptr) {}
    Pointer(Pointer const&) = default;
    Pointer& operator=(Pointer const&) = default;

    T& operator*() const
    {
      if(!cached || version != cabinet->version)
      {
        // caching does not break internal state
        Pointer* p = const_cast<Pointer*>(this);
        p->cached = &cabinet->get(id);
        p->version = cabinet->version;
      }
      return *cached;
    }
    T* operator->() const
    {
      return &operator*();
    }
    void remove()
    {
      cabinet->removeById(id);
    }
    operator bool() const
    {
      return cabinet != nullptr;
    }

  private:
    friend class Cabinet<T>;
    Pointer(Cabinet* cabinet, std::size_t id, unsigned int version, T* cached) : cabinet(cabinet), id(id), version(version), cached(cached) {}
    Cabinet* cabinet;
    std::size_t id;
    unsigned int version;
    T* cached;
  };

  Pointer insert(T&& t)
  {
    std::size_t id = next;
    map.insert(std::make_pair(id, contents.size()));
    ++next;

    size_type prevCapacity = contents.capacity();
    contents.push_back({id, t});
    if(contents.capacity() != prevCapacity)
    {
      ++version;
    }
    return {this, id, version, &contents.back().content};
  }

  std::size_t size() const
  {
    return contents.size();
  }

  T& at(std::size_t index) const
  {
    return contents.at(index).content;
  }

  void remove(std::size_t index)
  {
    Folder& f = contents.at(index);
    map.at(contents.back().id) = index;
    map.erase(f.id);
    f = std::move(contents.back());
    contents.pop_back();
    ++version;
  }

  iterator begin() { return iterator(contents.begin()); }
  iterator end() { return iterator(contents.end()); }

private:
  friend class Pointer;
  friend class iterator;

  T& get(std::size_t id)
  {
    std::size_t index = map.at(id);
    return contents.at(index).content;
  }

  void removeById(std::size_t id)
  {
    std::size_t index = map.at(id);
    remove(index);
  }


  std::size_t next = 1;
  std::unordered_map<std::size_t, std::size_t> map;
  std::vector<Folder> contents;
  unsigned int version = 0;
};
