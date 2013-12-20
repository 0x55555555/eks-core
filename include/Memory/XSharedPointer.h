#ifndef XSHARED_H
#define XSHARED_H

#include <atomic>
#include "Memory/XTypedAllocator.h"

namespace Eks
{

template <typename T, typename Policy, typename Alloc> class ConstSharedPointer;
template <typename T, typename Policy, typename Alloc> class SharedPointer;

namespace detail
{
class SharedData
  {
public:
  SharedData() : _ref(0) { }
  SharedData(const SharedData &) : _ref(0)
    {
    }

private:
  // returns true if the data is still referenced.
  bool deref()
    {
    xuint32 old = --_ref;
    return old != 0;
    }

  void ref()
    {
    ++_ref;
    }

  mutable std::atomic<xuint32> _ref;

  template <typename T, typename Policy, typename Alloc> friend class Eks::SharedPointer;
  template <typename T, typename Policy, typename Alloc> friend class Eks::ConstSharedPointer;
  };

struct SharedPolicy
  {
  template <typename T> static void onWrite(T *)
    {
    }
  };

struct CopyOnWritePolicy
  {
  template <typename T> static void onWrite(T *ptr)
    {
    if (ptr && ptr->deref())
      {
      Eks::AllocatorBase *alloc = allocator()->allocator();
      ptr = alloc->create<T>(*constPtr());
      ptr->ref();
      }
    else if(ptr)
      {
      ptr->ref();
      xAssert(_ref == 1);
      }
    }
  };
}

template <typename T,
          typename SharedPolicy = detail::SharedPolicy,
          typename Alloc=TypedAllocator<T> > class ConstSharedPointer
    : Alloc
  {
public:
  typedef SharedPolicy Policy;
  ConstSharedPointer()
    : _ptr(0)
    {
    }

  explicit ConstSharedPointer(const T *ptr, const Alloc &a = Alloc())
      : Alloc(a), _ptr(const_cast<T*>(ptr))
    {
    xAssert(_ptr);
    _ptr->ref();
    }

  ConstSharedPointer(const ConstSharedPointer<T, Policy, Alloc> &cpy, const Alloc &a = Alloc())
      : Alloc(a), _ptr(cpy._ptr)
    {
    if(_ptr)
      {
      _ptr->ref();
      }
    }

  ~ConstSharedPointer()
    {
    clear();
    }

  ConstSharedPointer<T, Policy, Alloc> &operator=(const ConstSharedPointer<T, Policy, Alloc> &cpy)
    {
    assign(const_cast<T*>(cpy.ptr()));
    return *this;
    }

  bool operator==(const ConstSharedPointer<T, Policy, Alloc> &oth)
    {
    return _ptr == oth._ptr;
    }

  void clear()
    {
    if(_ptr && !_ptr->deref())
      {
      allocator()->destroy(_ptr);
      _ptr = 0;
      }
    }

  void assign(T *cpy)
    {
    clear();

    if(cpy)
      {
      _ptr = cpy;
      _ptr->ref();
      }
    }

  operator const bool() const { return ptr(); }
  operator const T *() const { return ptr(); }

  const T &operator *() const { return *ptr(); }

  const T *operator->() const { return ptr(); }

  const T *ptr() const { return static_cast<const T*>(_ptr); }
  const T *constPtr() const { return static_cast<const T*>(_ptr); }

protected:
  Alloc *allocator()
    {
    return this;
    }

  T *_ptr;
  };

template <typename T,
          typename SharedPolicy = detail::SharedPolicy,
          typename Alloc=TypedAllocator<T> > class SharedPointer
    : public ConstSharedPointer<T, SharedPolicy, Alloc>
  {
public:
  SharedPointer()
    {
    }

  explicit SharedPointer(T *ptr, const Alloc &a = Alloc())
    : ConstSharedPointer<T, Policy, Alloc>(ptr, a)
    {
    }

  SharedPointer(const SharedPointer<T, Policy, Alloc> &cpy, const Alloc &a = Alloc())
    : ConstSharedPointer(cpy, a)
    {
    }

  SharedPointer<T, Policy, Alloc> &operator=(const SharedPointer<T, Policy, Alloc> &cpy)
    {
    assign(cpy.ptr());
    return *this;
    }

  operator T *() { return ptr(); }

  T &operator *() { Policy::onWrite(this); return *ptr(); }
  T &operator *() const { return *ptr(); }

  T *operator->() { Policy::onWrite(this); return ptr(); }
  T *operator->() const { return ptr(); }

  T *ptr() { Policy::onWrite(this); return static_cast<T*>(_ptr); }
  T *ptr() const { return static_cast<T*>(_ptr); }
  T *constPtr() const { return static_cast<T*>(_ptr); }

private:
  SharedPointer(const ConstSharedPointer<T, Policy, Alloc> &);
  SharedPointer<T, Policy, Alloc> operator=(const ConstSharedPointer<T, Policy, Alloc> &);
  };

template <typename T>
    Eks::SharedPointer<T, Eks::detail::SharedPolicy, TypedAllocator<T>> AllocatorBase::createShared()
  {
  void *mem = alloc(sizeof(T), AlignmentOf<T>::Alignment);
  return Eks::SharedPointer<T, Eks::detail::SharedPolicy, TypedAllocator<T>>(new(mem) T(), this);
  }

}

#endif // XSHARED_H