#ifndef XTIGHTPOINTER_H
#define XTIGHTPOINTER_H

#include "XGlobal.h"
#include "XAssert.h"

namespace Eks
{

template <typename T, xsize C> class TightPointer
  {
public:
  enum PointerBits
    {
    SpareBits = 1,
    DataBits = 0x1,
    PointerBits = (-1)^DataBits,
    };

  TightPointer()
    {
    }

  TightPointer(T *t, xuint8 d)
    {
    set(t, d);
    }

  T *get()
    {
    return (T*)((xsize)_data & PointerBits);
    }

  const T *get() const
    {
    return (T*)((xsize)_data & PointerBits);
    }

  void set(T *t)
    {
    set(t, getExtra());
    }

  void setExtra(xuint8 d)
    {
    set(get(), d);
    }

  xuint8 getExtra() const
    {
    return (xsize)_data & DataBits;
    }

  void set(T *t, xuint8 d)
    {
    xAssert((xsize)t == ((xsize)t&PointerBits));
    xAssert(d == (d&DataBits));
    _data = (T*)((xsize)t | (xsize)d);
    }

private:
  xCompileTimeAssert(C <= SpareBits);
  T *_data;
  };

}

#endif // XTIGHTPOINTER_H
