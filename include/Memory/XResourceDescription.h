#ifndef XRESOURCEDESCRIPTION_H
#define XRESOURCEDESCRIPTION_H

#include "XGlobal.h"
#include "Utilities/XProperty.h"
#include "Utilities/XAssert.h"

namespace Eks
{

#define X_ALIGN_BYTE_COUNT 16

#define xAssertIsSpecificAligned(ptr, alignment) xAssert(Eks::isAligned(ptr, alignment), (xsize)ptr, alignment, ((xsize)(ptr)&(alignment-1)))
#define xAssertIsAligned(ptr) xAssertIsSpecificAligned(ptr, X_ALIGN_BYTE_COUNT)

template <typename T> struct AlignmentOf
  {
  enum
    {
    Alignment = std::alignment_of<T>::value
    };
  };

template <typename T> bool isAligned(T ptr, xsize alignment = X_ALIGN_BYTE_COUNT)
  {
  const xptrdiff bitmask = (xptrdiff)alignment - 1;
  const xsize ptrD = (xsize)ptr;

  return (ptrD&bitmask) == 0;
  }

template <typename T> T offsetBy(T d, xptrdiff by)
  {
  return (T)((xuint8*)d + by);
  }

template <typename T> inline T roundToAlignment(T exp, xsize alignment = X_ALIGN_BYTE_COUNT)
  {
  if(alignment <= 1)
    {
    return exp;
    }

  const xptrdiff bitmask = (xptrdiff)alignment - 1;
  const xptrdiff bits = ((xptrdiff)exp)&bitmask;

  const xptrdiff offset = (xptrdiff)alignment - bits;

  T aligned = offsetBy(exp, offset);

  return aligned;
  }


class ResourceDescription;

template <typename T> struct ResourceDescriptionTypeHelper
  {
  typedef typename std::aligned_storage<sizeof(T), std::alignment_of<T>::value>::type Storage;
  static ResourceDescription createFor();
  };

class ResourceDescription
  {
XProperties:
  XProperty(xsize, size, setSize);
  XProperty(xsize, alignment, setAlignment);

public:
  ResourceDescription()
    : _size(0),
      _alignment(0)
    {
    }

  ResourceDescription(xsize s, xsize a)
    : _size(s),
      _alignment(a)
    {
    }

  void alignTo(xsize align)
    {
    _alignment = xMax(align, _alignment);
    }

  ResourceDescription operator+(const ResourceDescription &desc) const
    {
    ResourceDescription newDesc(*this);
    newDesc._size = Eks::roundToAlignment(_size, desc.alignment());
    newDesc._size += desc.size();
    return newDesc;
    }

  xsize allocatedSize() const
    {
    return _size + _alignment;
    }
  };

template <typename T, typename Derived> class MemoryResourceBase
  {
public:
  MemoryResourceBase(T rsc) : _resource(rsc)
    {
    }

  Derived decrememt(xsize s) const
    {
    return Derived(_resource - s);
    }

  Derived increment(xsize s) const
    {
    return Derived(_resource + s);
    }

  Derived align(const ResourceDescription &desc) const
    {
    return align(desc.alignment());
    }

  Derived align(xsize align) const
    {
    Derived rsc(Eks::roundToAlignment(_resource, align));
    xAssertIsSpecificAligned(rsc._resource, align);
    return rsc;
    }

  Derived alignAndIncrement(const ResourceDescription &rsc, MemoryResourceBase* next) const
    {
    Derived ths(Eks::roundToAlignment(_resource, rsc.alignment()));
    *next = ths.increment(rsc.size());
    return ths;
    }

  Derived increment(const ResourceDescription &rs) const
    {
    return Derived(_resource + rs.allocatedSize());
    }

protected:
  T _resource;
  };

class RelativeMemoryResource : public MemoryResourceBase<xptrdiff, RelativeMemoryResource>
  {
public:
  RelativeMemoryResource() : MemoryResourceBase(0)
    {
    }
  RelativeMemoryResource(xptrdiff x) : MemoryResourceBase(x)
    {
    }

  RelativeMemoryResource operator+(const RelativeMemoryResource& r)
    {
    return _resource + r._resource;
    }

  RelativeMemoryResource relativeTo(const RelativeMemoryResource& r)
    {
    return _resource - r._resource;
    }

  bool operator<(xsize s) const
    {
    return value() < (xptrdiff)s;
    }

  bool operator>(const RelativeMemoryResource s) const
    {
    return value() > s.value();
    }

  bool operator==(const RelativeMemoryResource &rsc) const
    {
    return _resource == rsc._resource;
    }

  bool isPre() const { return _resource < 0; }
  bool isZero() const { return _resource == 0; }
  bool isPost() const { return _resource > 0; }

  xptrdiff value() const { return _resource; }
  };

class MemoryResource : public MemoryResourceBase<xuint8*, MemoryResource>
  {
public:
  MemoryResource(void *rsc = nullptr) : MemoryResourceBase(static_cast<xuint8*>(rsc))
    {
    }

  bool isValid() const
    {
    return _resource != false;
    }

  template <typename T> static MemoryResource destroy(T *t)
    {
    ((T*)t)->~T();
    return Eks::MemoryResource((T*)t);
    }

  template <typename T> T *create() const
    {
    return new(_resource) T();
    }
  template <typename T, typename A> T *create(A&& a) const
    {
    return new(_resource) T(a);
    }

  void *data() const { return _resource; }
  };

template <typename T>
    ResourceDescription ResourceDescriptionTypeHelper<T>::createFor()
  {
  return ResourceDescription(sizeof(T), AlignmentOf<T>::Alignment);
  }
}

#endif // XRESOURCEDESCRIPTION_H
