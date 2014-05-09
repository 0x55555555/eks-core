#ifndef XSCOPEDVALUE_H
#define XSCOPEDVALUE_H

namespace Eks
{

template <typename T> class ScopedValue
  {
public:
  ScopedValue(T &t, const T &val)
    {
    assign(t, val);
    }

  ~ScopedValue()
    {
    release();
    }

  void assign(T &t, const T &val)
    {
    _oldValue = _value;
    _value = &t;
    *_value = val;
    }

  void release()
    {
    if (_value)
      {
      *_value = _oldValue;

      _value = nullptr;
      }
    }

private:
  T *_value;
  T _oldValue;
  };

}

#endif // XSCOPEDVALUE_H
