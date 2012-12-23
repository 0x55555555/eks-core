#include "XMemoryTracker"
#include "XAssert"

namespace Eks
{

MemoryTracker::MemoryTracker(AllocatorBase* forwarder) : _forwarder(forwarder), _liveAllocations(0)
  {
  }

void *MemoryTracker::alloc(xsize size, xsize alignment)
  {
  xAssert(_forwarder);

  void *mem = _forwarder->alloc(size, alignment);
  ++_liveAllocations;

  return mem;
  }

void MemoryTracker::free(void *mem)
  {
  xAssert(_forwarder);

  xAssert(_liveAllocations > 0);
  --_liveAllocations;

  _forwarder->free(mem);
  }

}
