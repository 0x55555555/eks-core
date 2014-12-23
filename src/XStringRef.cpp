#include "Utilities/XStringRef.h"

namespace Eks
{

StringRef::StringRef()
  {
  _type = TypeEks;
  _data.eks = 0;
  _length = 0;
  }

StringRef::StringRef(StringRef &&oth)
  {
  _type = oth._type;
  _data = oth._data;
  _length = oth._length;
  }

StringRef& StringRef::operator=(StringRef&& oth)
  {
  _type = oth._type;
  _data = oth._data;
  _length = oth._length;

  return *this;
  }

#if X_QT_INTEROP
StringRef::StringRef(const QString &t)
  {
  _type = TypeQt;
  _data.qt = &t;
  _length = t.length();
  }
#endif

bool StringRef::isEmpty() const
  {
  return _length == 0;
  }

bool StringRef::operator==(const StringRef &oth) const
  {
  if(_type == TypeEks)
    {
    if(oth._type == TypeEks)
      {
      return strcmp(_data.eks, oth._data.eks) == 0;
      }
#if X_QT_INTEROP
    else if(oth._type == TypeQt)
      {
      return  *oth._data.qt == _data.eks;
      }
#endif
    }
#if X_QT_INTEROP
  else if(_type == TypeQt)
    {
    if(oth._type == TypeEks)
      {
      return *_data.qt == oth._data.eks;
      }
    else if(oth._type == TypeQt)
      {
      return *_data.qt == *oth._data.qt;
      }
    }
#endif

  return false;
  }

}
