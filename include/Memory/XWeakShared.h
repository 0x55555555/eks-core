#ifndef XWEAKSHARED_H
#define XWEAKSHARED_H

#include "XGlobal.h"
#include "Utilities/XMacroHelpers.h"
#include "Utilities/XAssert.h"

namespace Eks
{

class WeakSharedPointerData;

class WeakSharedData
  {
public:
  WeakSharedData() : _first(0) { }
  ~WeakSharedData();

private:
  WeakSharedPointerData *_first;
  friend class WeakSharedPointerData;
  X_DISABLE_COPY(WeakSharedData);
  };

class WeakSharedPointerData
  {
public:
  WeakSharedPointerData(WeakSharedData *data) : _data(0), _next(0)
    {
    assign(data);
    }

  WeakSharedPointerData(const WeakSharedPointerData &data) : _data(0), _next(0)
    {
    assign(data._data);
    }

  ~WeakSharedPointerData()
    {
    clear();
    }

  WeakSharedPointerData &operator=(const WeakSharedPointerData &cpy)
    {
    assign(cpy._data);
    return *this;
    }

  void clear()
    {
    if(_data)
      {
      xAssert(_data->_first);
      if(_data->_first == this)
        {
        _data->_first = _data->_first->_next;
        }
      else
        {
        WeakSharedPointerData *next = _data->_first;
        while(next->_next != this)
          {
          xAssert(next->_next);
          next = next->_next;
          }
        xAssert(next->_next);
        xAssert(next->_next == this);
        next->_next = _next;
        }
      }
    _data = 0;
    _next = 0;
    }

protected:
  void assign(WeakSharedData *data)
    {
    clear();
    if(data)
      {
      _data = data;
      if(!data->_first)
        {
        data->_first = this;
        }
      else
        {
        WeakSharedPointerData *next = data->_first;
        while(next->_next)
          {
          next = next->_next;
          }
        next->_next = this;
        xAssert(_next == 0);
        }
      }
    }

  WeakSharedData *_data;
  WeakSharedPointerData *_next;
  friend class WeakSharedData;
  };

template <typename T> class WeakSharedPointer : public WeakSharedPointerData
  {
public:
  WeakSharedPointer(T *data) : WeakSharedPointerData(data) { }

  void assign(T *data) { WeakSharedPointerData::assign(data); }

  T *data() { return static_cast<T*>(_data); }
  const T *data() const { return static_cast<T*>(_data); }
  };


inline WeakSharedData::~WeakSharedData()
  {
  WeakSharedPointerData *ptr = _first;
  while(ptr)
    {
    ptr->_data = 0;
    WeakSharedPointerData *next = ptr->_next;
    ptr->_next = 0;
    ptr = next;
    }
  }

}

#endif // XWEAKSHARED_H
