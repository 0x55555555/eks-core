#include "XMemoryTracker"

XMemoryTracker::XMemoryTracker(XAllocatorBase* forwarder) : _forwarder(forwarder), _liveAllocations(0)
  {
  }

void *XMemoryTracker::alloc(xsize size, xsize alignment)
  {
  xAssert(_forwarder);

  void *mem = _forwarder->alloc(size, alignment);
  ++_liveAllocations;

  return mem;
  }

void XMemoryTracker::free(void *mem)
  {
  xAssert(_forwarder);

  xAssert(_liveAllocations > 0);
  --_liveAllocations;

  _forwarder->free(mem);
  }
