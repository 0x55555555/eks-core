#ifndef XOPTIONALPOINTER_H
#define XOPTIONALPOINTER_H

namespace Eks
{

template <typename T> class Optional
  {
public:
  Optional(T *location) : _val(location ? location : &_myCopy)
    {
    }

  operator T()
    {
    return *_val;
    }


  T operator->()
    {
    return *_val;
    }

  Optional<T> &operator=(const T &val)
    {
    *_val = val;
    return *this;
    }

  T *operator&()
    {
    return _val;
    }

private:
  T _myCopy;
  T *_val;
  };

}

#endif // XOPTIONALPOINTER_H
