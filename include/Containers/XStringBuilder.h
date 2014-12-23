#pragma once
#include "Memory/XTemporaryAllocator.h"
#include "Containers/XStringSimple.h"
#include "Containers/XStringBuffer.h"

namespace Eks
{

class StringBuilder
  {
public:
  StringBuilder()
      : _alloc(Eks::Core::temporaryAllocator()),
        _buf(&_alloc)
    {
    _buf.reserve(1024);
    }

  StringBuilder(StringBuilder &&oth)
      : _alloc(std::move(oth._alloc)),
        _buf(std::move(oth._buf))
    {
    _buf.allocator() = &_alloc;
    }

  StringBuilder &operator=(StringBuilder &&oth)
    {
    xAssertFail(); // todo - _buf's allocator may be confused here...
    _alloc = std::move(oth._alloc);
    _buf = std::move(oth._buf);
    _buf.allocator() = &_alloc;

    return *this;
    }

  void clear()
    {
    _buf.clear();
    }

  template <typename It>
  void resizeAndCopy(String::size_type newSize, It val)
    {
    _buf.resizeAndCopy(newSize, val);
    }

  template <typename T> StringBuilder &operator<<(const T &t)
    {
    _buf.appendType(t);

    return *this;
    }

  operator Eks::String() const
    {
    return Eks::String(_buf, Core::defaultAllocator());
    }

  Eks::String value(Eks::AllocatorBase *a) const
    {
    return Eks::String(_buf, a);
    }

  const Eks::String &value() const
    {
    return _buf;
    }

  const char *data() const
    {
    return _buf.data();
    }

  xsize length() const
    {
    return _buf.length();
    }

  template <typename T> void adjust(const T &t)
    {
    t(_buf);
    }

private:
  Eks::TemporaryAllocator _alloc;
  Eks::String _buf;
  };

}
