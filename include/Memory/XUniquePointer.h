#ifndef XUNIQUEPOINTER_H
#define XUNIQUEPOINTER_H

#include "Memory/XAllocatorBase.h"

namespace Eks
{

namespace detail
{
template <typename Alloc> struct Deleter : Alloc
  {
  Deleter()
    {
    }

  Deleter(const Alloc &a) : Alloc(a)
    {
    }

  template <typename X> Deleter(const Deleter<X> &a) : Alloc(a.allocator())
    {
    }

  template <typename T> void operator()(T *ptr)
    {
    Alloc::allocator()->destroy(ptr);
    }
  };
}

template <typename T, typename Alloc> class UniquePointer
    : public std::unique_ptr<T, detail::Deleter<Alloc> >
  {
  typedef std::unique_ptr<T, detail::Deleter<Alloc> > ParentType;
public:
  UniquePointer()
    {
    }

  UniquePointer(std::nullptr_t)
    {
    }

  UniquePointer(T *t, const Alloc &a)
      : ParentType(t, a)
    {
    }

  template <typename X> UniquePointer(UniquePointer<X>&& p)
      : ParentType(p.release(), p.get_deleter())
    {
    }

  template <typename X>ParentType &operator=(UniquePointer<X>&& p)
    {
    ParentType& ths = *this;
    ths = std::move(p);
    return *this;
    }

  ParentType &operator=(std::nullptr_t)
    {
    ParentType& ths = *this;
    ths = nullptr;
    return *this;
    }
  };

template <typename T>
    Eks::UniquePointer<T, TypedAllocator<T>> AllocatorBase::createUnique()
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
