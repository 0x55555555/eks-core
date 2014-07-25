#ifndef XVARIANT_H
#define XVARIANT_H

#include "XGlobal.h"
#include "Math/XMathHelpers.h"
#include "Utilities/XProperty.h"

namespace Eks
{

namespace detail
{

template <typename T> class VariantStorageBase
  {
public:
  template <typename... CtorArgs> void set(CtorArgs&&... args)
    {
    new(get()) T(std::forward<CtorArgs>(args)...);
    }

  void *get()
    {
    return reinterpret_cast<void*>(&t);
    }

  const void *get() const
    {
    return reinterpret_cast<void*>(&t);
    }

  void destroy()
    {
    auto d = (T*)get();
    d->~T();
    }

private:
  typename std::aligned_storage<sizeof(T), alignof(T)>::type t;
  };

template <xuint8 IndexStart, typename T, typename... Args> class VariantStorage
  {
public:
  VariantStorage() : other()
    {
    }

  template <typename X> static xuint8 indexFor()
    {
    if (std::is_same<X, T>::value)
      {
      return IndexStart;
      }

    return Other::template indexFor<X>();
    }

  template <typename X, typename... CtorArgs> void set(CtorArgs&&... args)
    {
    if (std::is_same<X, T>::value)
      {
      t.set(std::forward<CtorArgs>(args)...);
      return;
      }

    other.template set<X>(std::forward<CtorArgs>(args)...);
    }

  template <typename X> X &get()
    {
    if (std::is_same<X, T>::value)
      {
      return *(X*)t.get();
      }

    return other.template get<X>();
    }


  template <typename X> const X &get() const
    {
    if (std::is_same<X, T>::value)
      {
      return *(X*)t.get();
      }

    return other.template get<X>();
    }

  void assign(xuint8 i, const VariantStorage& s)
    {
    if (i == IndexStart)
      {
      t.set(s.get<T>());
      return;
      }

    other.template assign(i, s.other);
    }

  void destroy(xuint8 i)
    {
    if (i == IndexStart)
      {
      t.destroy();
      return;
      }

    other.destroy(i);
    }

  typedef VariantStorage<IndexStart+1, Args...> Other;
  union
    {
    VariantStorageBase<T> t;
    Other other;
    };

private:
  };

template <xuint8 IndexStart, typename T> class VariantStorage<IndexStart, T>
  {
public:
  VariantStorage() : null(0)
    {
    }

  template <typename X> static xuint8 indexFor()
    {
    if (std::is_same<X, T>::value)
      {
      return IndexStart;
      }

    abort(); // Invlaid type specified.
    return std::numeric_limits<xuint8>::max();
    }

  template <typename X, typename... CtorArgs> void set(CtorArgs&&... args)
    {
    if (std::is_same<X, T>::value)
      {
      t.set(std::forward<CtorArgs>(args)...);
      return;
      }

    abort(); // Invlaid type specified.
    }

  template <typename X> X &get()
    {
    if (std::is_same<X, T>::value)
      {
      return *(X*)t.get();
      }

    abort(); // Invlaid type specified.
    }

  template <typename X> const X &get() const
    {
    if (std::is_same<X, T>::value)
      {
      return *(X*)t.get();
      }

    abort(); // Invlaid type specified.
    }

  void assign(xuint8 i, const VariantStorage& s)
    {
    if (i == IndexStart)
      {
      t.set(s.get<T>());
      return;
      }

    abort(); // Invlaid type specified.
    }

  void destroy(xuint8 i)
    {
    if (i == IndexStart)
      {
      t.destroy();
      return;
      }

    abort(); // Invlaid type specified.
    }

  union
    {
    VariantStorageBase<T> t;
    xuint8 null;
    };
  };

}

template <typename... Args> class Variant
  {
public:
  class VariantType
    {
  public:
    enum Defined
      {
      Null = std::numeric_limits<xuint8>::max()
      };

    bool operator==(Defined d) const
      {
      return id == d;
      }

    bool operator==(const VariantType &d) const
      {
      return id == d.id;
      }

    bool operator<(const VariantType &t)
      {
      return id < t.id;
      }

  private:
    VariantType(xuint8 i) : id(i) { }
    xuint8 id;
    friend class Variant;
    };
  typedef detail::VariantStorage<0, Args...> Storage;

XProperties:
  XROProperty(VariantType, type);
  XROProperty(Storage, data);

  Variant()
       : _type(VariantType::Null)
    {
    }

  Variant(const Variant& v)
    : _type(VariantType::Null)
    {
    set(v._type, v);
    }

  ~Variant()
    {
    clear();
    }

  Variant &operator=(const Variant &v)
    {
    set(v.typeFor(), v);
    return *this;
    }

  template <typename T, typename... CtorArgs> static Variant<Args...> create(CtorArgs&&... args)
    {
    Variant v;
    v.set<T>(std::forward<CtorArgs>(args)...);
    return v;
    }

  template <typename T, typename... CtorArgs> void set(CtorArgs&&... args)
    {
    clear();
    _type.id = _data.template indexFor<T>();
    _data.template set<T>(std::forward<CtorArgs>(args)...);
    }

  void set(const VariantType &t, const Variant &v)
    {
    clear();
    _type = t;
    _data.assign(t.id, v._data);
    }

  template <typename T> bool is() const
    {
    return _type.id == _data.template indexFor<T>();
    }

  template <typename T> const T &get() const
    {
    if (!is<T>())
      {
      throw std::bad_cast();
      }

    return _data.template get<T>();
    }

  void clear()
    {
    if (_type.id != VariantType::Null)
      {
      _data.destroy(_type.id);
      _type.id = VariantType::Null;
      }
    }

  template <typename T> static VariantType typeFor()
    {
    return VariantType(Storage::template indexFor<T>());
    }
  };


}

#endif
