#ifndef XUNORDEREDMAP_H
#define XUNORDEREDMAP_H

#include <unordered_map>
#include <functional>
#include "Memory/XTypedAllocator.h"

namespace Eks
{

template <typename K, typename V, typename Alloc=TypedAllocator<std::pair<const K, V>>> class UnorderedMap
    : public std::unordered_map<K, V, std::hash<K>, std::equal_to<K>, Alloc>
  {
  typedef std::unordered_map<K, V, std::hash<K>, std::equal_to<K>, Alloc> ParentType;
public:
  UnorderedMap(const Alloc& alloc)
    : ParentType(alloc)
    {
    }

  const V &value(const K &k, const V &def) const
    {
    auto it = ParentType::find(k);
    if (it == ParentType::end())
      {
      return def;
      }

    return it->second;
    }


  V value(const K &k) const
    {
    auto it = ParentType::find(k);
    if (it == ParentType::end())
      {
      return V();
      }

    return it->second;
    }

  bool contains(const K &k) const
    {
    return ParentType::find(k) != ParentType::end();
    }

  template <typename T> class TransformWalker
    {
  public:
    typedef std::function<T(const typename ParentType::const_iterator &)> Function;
    class TransformIterator
      {
    public:
      TransformIterator(typename ParentType::const_iterator d, const Function *fn)
          : _it(d), _transform(fn)
        {
        }

      TransformIterator& operator++()
        {
        ++_it;
        return *this;
        }

      T operator*()
        {
        return (*_transform)(_it);
        }

      bool operator!=(const TransformIterator &it)
        {
        return _it != it._it;
        }

    private:
      typename ParentType::const_iterator _it;
      const Function *_transform;
      };

    TransformWalker(const ParentType &d, Function &&fn)
        : _data(d), _transform(fn)
      {
      }

    TransformIterator begin() const { return TransformIterator{_data.begin(), &_transform}; }
    TransformIterator end() const { return TransformIterator{_data.end(), &_transform}; }

  private:
    const ParentType &_data;
    Function _transform;
    };

  template <typename T> TransformWalker<T> transform(typename TransformWalker<T>::Function t) const
    {
    return { *this, std::move(t) };
    }

  TransformWalker<const typename ParentType::key_type &> keys() const
    {
    typedef const typename ParentType::key_type &Return;

    return transform<Return>(
      [] (const typename ParentType::const_iterator &it) -> Return
        {
        return it->first;
        }
      );
    }


  TransformWalker<const typename ParentType::mapped_type &> values() const
    {
    typedef const typename ParentType::mapped_type &Return;

    return transform<Return>(
      [] (const typename ParentType::const_iterator &it) -> Return
        {
        return it->second;
        }
      );
    }

  };

}

#endif // XUNORDEREDMAP_H
