#pragma once
#include "XGlobal.h"
#include "Containers/XStringSimple.h"
#include "Containers/XStringBuilder.h"

namespace Eks
{

class StringBuilder;

class EKSCORE_EXPORT StringRef
  {
public:
  StringRef();
  StringRef(StringRef &&);
  StringRef& operator=(StringRef&& other);

#if X_QT_INTEROP
  StringRef(const QString &);
#endif

  template <typename C, xsize S, typename A>
  StringRef(const Eks::StringBase<C, S, A> &in)
    {
    xCompileTimeAssert(sizeof(C) == sizeof(Eks::Char));

    _type = TypeEks;
    _data.eks = in.data();
    _length = in.length();
    }

  StringRef(const Eks::Char *in)
    {
    _type = TypeEks;
    _data.eks = in;
    _length = Eks::StringBase<Eks::Char>::Traits::length(in);
    }

  template <typename T> void toString(T &out) const
    {
    out.clear();

    if(_type == TypeEks)
      {
      out.resizeAndCopy(_length, _data.eks);
      }
  #if X_QT_INTEROP
    else if(_type == TypeQt)
      {
      out = *_data.qt;
      }
  #endif
    }

  Eks::StringBuilder toBuilder() const;

  bool isEmpty() const;

  bool operator==(const StringRef &oth) const;
  template <typename T> bool operator==(const T &oth) const
    {
    if(_type == TypeEks)
      {
      return oth == _data.eks;
      }
  #if X_QT_INTEROP
    else if(_type == TypeQt)
      {
      return (*_data.qt) == oth.data();
      }
  #endif

    return false;
    }

  struct DataStub
    {
    operator const char *() const { return bld.data(); }
    Eks::StringBuilder bld;
    };
  DataStub data() const;

private:
  X_DISABLE_COPY(StringRef)

  enum
    {
    TypeEks,
#if X_QT_INTEROP
    TypeQt
#endif
    } _type;

  xsize _length;
  union
    {
    const Eks::Char *eks;
#if X_QT_INTEROP
    const QString *qt;
#endif
    } _data;
  };

}
