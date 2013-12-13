#ifndef XALLOCATORBASE_H
#define XALLOCATORBASE_H

#include "XGlobal.h"
#include "Utilities/XAssert.h"
#include "Memory/XResourceDescription.h"

namespace Eks
{

namespace detail
{
struct SharedPolicy;
}

template <typename T> class TypedAllocator;
template <typename T, typename Policy, typename Alloc> class SharedPointer;
template <typename T, typename Alloc=Eks::TypedAllocator<T>> class UniquePointer;

class AllocatorBase
  {
public:
  virtual ~AllocatorBase() {}
  virtual void *alloc(xsize size, xsize alignment=1) = 0;
  virtual void free(void *mem) = 0;

  MemoryResource alloc(const ResourceDescription &fmt)
    {
    return alloc(fmt.size(), fmt.alignment());
    }

  void free(const MemoryResource &rsc)
    {
    return free(rsc.data());
    }

  // todo, variadic template args here...
  template <typename T>
    Eks::SharedPointer<T, Eks::detail::SharedPolicy, TypedAllocator<T>> createShared();

  template <typename T>
    Eks::UniquePointer<T, TypedAllocator<T>> createUnique();
  template <typename T, typename A>
    Eks::UniquePointer<T, TypedAllocator<T>> createUnique(A &&a);
  template <typename T, typename A, typename B>
    Eks::UniquePointer<T, TypedAllocator<T>> createUnique(A &&a, B &&b);

  template <typename T> T *create()
    {
    void *mem = alloc(sizeof(T), AlignmentOf<T>::Alignment);

#ifdef X_MSVC
#pragma warning(push)
# pragma warning(disable: 4345) /* behavior change: an object of POD type constructed with an initializer of the form () will be default-initialized */
#endif

    return new(mem) T();

#ifdef X_MSVC
#pragma warning(pop)
#endif
    }

  template <typename T, typename A> T *create(A &&a)
    {
    void *mem = alloc(sizeof(T), AlignmentOf<T>::Alignment);
    return new(mem) T(std::forward<A>(a));
    }

  template <typename T,
            typename A,
            typename B> T *create(A &&a, B &&b)
    {
    void *mem = alloc(sizeof(T), AlignmentOf<T>::Alignment);
    return new(mem) T(std::forward<A>(a), std::forward<B>(b));
    }

  template <typename T,
            typename A,
            typename B,
            typename C> T *create(A &&a, B &&b, C &&c)
    {
    void *mem = alloc(sizeof(T), AlignmentOf<T>::Alignment);
    return new(mem) T(std::forward<A>(a), std::forward<B>(b), std::forward<C>(c));
    }

  template <typename T,
            typename A,
            typename B,
            typename C,
            typename D> T *create(A &&a, B &&b, C &&c, D &&d)
    {
    void *mem = alloc(sizeof(T), AlignmentOf<T>::Alignment);
    return new(mem) T(std::forward<A>(a), std::forward<B>(b), std::forward<C>(c), std::forward<D>(d));
    }


  // todo, variadic template args here...
  template <typename T> T *createWithAlignment(xsize alignment)
    {
    void *d = alloc(sizeof(T), alignment);
    return new(d) T();
    }

  template <typename T, typename A> T *createWithAlignment(A a, xsize alignment)
    {
    void *mem = alloc(sizeof(T), alignment);
    return new(mem) T(a);
    }

  template <typename T,
            typename A,
            typename B> T *createWithAlignment(A a, B b, xsize alignment)
    {
    void *mem = alloc(sizeof(T), alignment);
    return new(mem) T(a, b);
    }

  template <typename T,
            typename A,
            typename B,
            typename C> T *createWithAlignment(A a, B b, C c, xsize alignment)
    {
    void *mem = alloc(sizeof(T), alignment);
    return new(mem) T(a, b, c);
    }

  template <typename T,
            typename A,
            typename B,
            typename C,
            typename D> T *createWithAlignment(A a, B b, C c, D d, xsize alignment)
    {
    void *mem = alloc(sizeof(T), alignment);
    return new(mem) T(a, b, c, d);
    }

  template <typename T> void destroy(T *t)
    {
    destruct(t);
    free(t);
    }

  template <typename Type>
      static void construct(Type *p, const Type& val)
    {
    new(p) Type(val);
    }

  template <typename Type>
      static void construct(Type *p, Type&& val)
    {
    new(p) Type(std::move(val));
    }

  template <typename U> static void destruct(U *p)
    {
    (void)p;
    p->~U();
    }
  };

}

#endif // XALLOCATORBASE_H
