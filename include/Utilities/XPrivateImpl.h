#ifndef XPRIVATEIMPL_H
#define XPRIVATEIMPL_H

#include "XGlobal.h"
#include "Utilities/XAssert.h"

namespace Eks
{

template <xsize size> class PrivateImpl
  {
public:
  PrivateImpl()
    {
    _valid = false;
    }

  ~PrivateImpl()
    {
    xAssert(!_valid);
    }

  template <typename T> T *data()
    {
    sizeCheck<T>();
    xAssert(_valid);

    return reinterpret_cast<T*>(_data);
    }

  template <typename T> const T *data() const
    {
    sizeCheck<T>();
    xAssert(_valid);

    return reinterpret_cast<const T*>(_data);
    }

  template <typename T, typename... Args> T *create(Args &&... a)
    {
    sizeCheck<T>();
    xAssert(!_valid);

    T *res = new(_data) T(std::forward<Args>(a)...);
    _valid = true;

    return res;
    }

  template <typename T> void destroy()
    {
    sizeCheck<T>();

    if(_valid)
      {
      T *d = data<T>();
      (void)d;
      d->~T();
      _valid = false;
      }
    }

  bool isValid() const
    {
    return _valid;
    }

private:
  X_DISABLE_COPY(PrivateImpl)

  template <typename T> X_CONST_EXPR void sizeCheck() const
    {
    typedef typename std::aligned_storage<sizeof(T), std::alignment_of<T>::value>::type Type;
    xCompileTimeAssert(sizeof(Type) <= size);
    }

  xuint8 _data[size];
  xuint8 _valid;
  };

template <typename T, size_t Size>
class PrivateImplTyped : public PrivateImpl<Size>
  {
public:
  template <typename... Args> PrivateImplTyped(Args &&... a)
    {
    xCompileTimeAssert(sizeof(T) <= Size);
    PrivateImpl<Size>::template create<T>(std::forward<Args>(a)...);
    }

  PrivateImplTyped(const PrivateImplTyped &t)
    {
    PrivateImpl<Size>::template create<T>(t.data());
    }

  ~PrivateImplTyped()
    {
    PrivateImpl<Size>::template destroy<T>();
    }

  PrivateImplTyped &operator=(const PrivateImplTyped &t)
    {
    PrivateImpl<Size>::template destroy<T>();
    PrivateImpl<Size>::template create<T>(t.data());
    return *this;
    }

  T &data() const
    {
    return *const_cast<T*>(PrivateImpl<Size>::template data<T>());
    }

  T *operator->() const
    {
    return &data();
    }
  };

}

#endif // XPRIVATEIMPL_H
