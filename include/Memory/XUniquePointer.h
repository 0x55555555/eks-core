#ifndef XUNIQUEPOINTER_H
#define XUNIQUEPOINTER_H

#include "XGlobal.h"
#include "Utilities/XMacroHelpers.h"
#include "Memory/XTypedAllocator.h"
#include "Utilities/XProperty.h"
#include "Memory/XAllocatorBase.h"

namespace Eks
{

template <typename T,
          typename Deleter> class UniquePointer : public Deleter
  {
XProperties:
  XROProperty(T *, pointer);

public:
  explicit UniquePointer(T *value=nullptr, Deleter d=Deleter())
      : Deleter(d),
        _pointer(value),
        _delete(wrapDelete<T>)
    {
    }

  /// \brief move from a further derived class
  template <typename X, typename XDel> UniquePointer(UniquePointer<X, XDel> &&value)
      : Deleter(value),
        _pointer(value._pointer),
        _delete(value._delete)
    {
    value.release();
    }

  UniquePointer(std::nullptr_t)
      : _pointer(nullptr),
        _delete(wrapDelete<int>)
    {
    }

  UniquePointer(UniquePointer<T, Deleter> &&oth)
      : Deleter(oth),
        _pointer(0)
    {
    std::swap(_pointer, oth._pointer);
    std::swap(_delete, oth._delete);
    }

  ~UniquePointer()
    {
    clear();
    }

  UniquePointer<T>& operator=(std::nullptr_t)
    {
    clear();
    return *this;
    }

  UniquePointer<T>& operator=(UniquePointer<T, Deleter> &&oth)
    {
    assign(std::move(oth));
    return *this;
    }

  /// \brief assign from a further derived class
  template <typename Higher, typename HDeleter> UniquePointer<T>& operator=(
      UniquePointer<Higher, HDeleter> &&oth)
    {
    assign(std::move(oth));
    return *this;
    }

  void clear()
    {
    if(_pointer)
      {
      _delete(_pointer, Deleter::allocator());
      _pointer = 0;
      }
    }

  template <typename Higher, typename HDeleter> void assign(
      UniquePointer<Higher, HDeleter> &&data)
    {
    clear();
    (Deleter&)(*this) = data;
    _pointer = data.release();
    _delete = data._delete;
    }

  void assign(UniquePointer<T> &&data)
    {
    clear();
    (Deleter&)(*this) = data;
    std::swap(_pointer, data._pointer);
    _delete = data._delete;
    }

  T *release()
    {
    T *t = _pointer;
    _pointer = nullptr;
    return t;
    }

  T *value() { return _pointer; }
  const T *value() const { return _pointer; }

  T *operator->() { return _pointer; }
  const T *operator->() const { return _pointer; }

  operator bool() const
    {
    return _pointer != 0;
    }

private:
  X_DISABLE_COPY(UniquePointer);

  template <typename X> static void wrapDelete(void *p, AllocatorBase* d)
    {
    d->destroy(static_cast<X*>(p));
    }

  void (*_delete)(void *, AllocatorBase* d);

  template <typename, typename> friend class UniquePointer;
  };

template <typename T> Eks::UniquePointer<T, TypedAllocator<T>> AllocatorBase::createUnique()
  {
  void *mem = alloc(sizeof(T), AlignmentOf<T>::Alignment);

  auto t = new(mem) T();
  TypedAllocator<T> alloc(this);

  Eks::UniquePointer<T, TypedAllocator<T>> ptr(t, alloc);
  return std::move(ptr);
  }

template <typename T, typename A>
    Eks::UniquePointer<T, TypedAllocator<T>> AllocatorBase::createUnique(A &&a)
  {
  void *mem = alloc(sizeof(T), AlignmentOf<T>::Alignment);

  auto t = new(mem) T(std::forward<A>(a));
  TypedAllocator<T> alloc(this);

  Eks::UniquePointer<T, TypedAllocator<T>> ptr(t, alloc);
  return std::move(ptr);
  }

template <typename T, typename A, typename B>
    Eks::UniquePointer<T, TypedAllocator<T>> AllocatorBase::createUnique(A &&a, B &&b)
  {
  void *mem = alloc(sizeof(T), AlignmentOf<T>::Alignment);

  auto t = new(mem) T(std::forward<A>(a), std::forward<B>(b));
  TypedAllocator<T> alloc(this);

  Eks::UniquePointer<T, TypedAllocator<T>> ptr(t, alloc);
  return std::move(ptr);
  }

template <typename T, typename A, typename B, typename C>
    Eks::UniquePointer<T, TypedAllocator<T>> AllocatorBase::createUnique(A &&a, B &&b, C &&c)
  {
  void *mem = alloc(sizeof(T), AlignmentOf<T>::Alignment);

  auto t = new(mem) T(std::forward<A>(a), std::forward<B>(b), std::forward<C>(c));
  TypedAllocator<T> alloc(this);

  Eks::UniquePointer<T, TypedAllocator<T>> ptr(t, alloc);
  return std::move(ptr);
  }

template <typename T, typename A, typename B, typename C, typename D>
    Eks::UniquePointer<T, TypedAllocator<T>> AllocatorBase::createUnique(A &&a, B &&b, C &&c, D &&d)
  {
  void *mem = alloc(sizeof(T), AlignmentOf<T>::Alignment);

  auto t = new(mem) T(std::forward<A>(a), std::forward<B>(b), std::forward<C>(c), std::forward<D>(d));
  TypedAllocator<T> alloc(this);

  Eks::UniquePointer<T, TypedAllocator<T>> ptr(t, alloc);
  return std::move(ptr);
  }

template <typename T, typename A, typename B, typename C, typename D, typename E>
    Eks::UniquePointer<T, TypedAllocator<T>> AllocatorBase::createUnique(A &&a, B &&b, C &&c, D &&d, E &&e)
  {
  void *mem = alloc(sizeof(T), AlignmentOf<T>::Alignment);

  auto t = new(mem) T(std::forward<A>(a), std::forward<B>(b), std::forward<C>(c), std::forward<D>(d), std::forward<E>(e));
  TypedAllocator<T> alloc(this);

  Eks::UniquePointer<T, TypedAllocator<T>> ptr(t, alloc);
  return std::move(ptr);
  }
}

#endif // XUNIQUEPOINTER_H
