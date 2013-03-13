#include "XGlobalAllocator"
#include "XEventLogger"
#include "XAssert"
#include "XLoggingAllocator"

#ifndef Q_CC_MSVC
# include <malloc.h>
#endif

X_DECLARE_MEMORY_LOGGER(EKSCORE_EXPORT, xTotalGlobalAllocatorSize);
X_IMPLEMENT_MEMORY_LOGGER(xTotalGlobalAllocatorSize);

namespace Eks
{

GlobalAllocator g_allocator;

GlobalAllocator::GlobalAllocator()
  {
  _allocationCount = 0;
  }

GlobalAllocator::~GlobalAllocator()
  {
  xAssert(_allocationCount == 0);
  }

AllocatorBase *GlobalAllocator::instance()
  {
#ifdef X_ENABLE_EVENT_LOGGING
  static LoggingAllocator alloc(&g_allocator);
  return &alloc;
#else
  return &g_allocator;
#endif
  }

#ifdef X_ENABLE_EVENT_LOGGING
struct MemoryHandleHeader
  {
  xsize allocSize;
  xuint32 padding[(sizeof(xsize) == 4) ? 3 : 2];
  };
#endif

enum
  {
#ifdef X_ENABLE_EVENT_LOGGING
  MemoryHandleHeaderSize = sizeof(MemoryHandleHeader)
#else
  MemoryHandleHeaderSize = 0
#endif
  };


void *GlobalAllocator::alloc(xsize size, xsize alignment)
  {
  xAssert(((alignment-1) & MemoryHandleHeaderSize) == 0);

  X_MEMORY_LOGGER_ALLOC(xTotalGlobalAllocatorSize, size);

#ifndef Q_CC_MSVC
  void *m = qMallocAligned(MemoryHandleHeaderSize + size, alignment);

#else
  // msvc malloc should always be 16 byte aligned
  void *m = _aligned_malloc(MemoryHandleHeaderSize + size, alignment);

#endif


  xAssertIsSpecificAligned(m, alignment);

#ifdef X_ENABLE_EVENT_LOGGING
  MemoryHandleHeader *handle = (MemoryHandleHeader*)m;

  memset(handle->padding, 0, sizeof(handle->padding));
  handle->allocSize = size;
#endif

  const xsize handleSize = MemoryHandleHeaderSize;

  ++_allocationCount;

  return (void*)(((xuint8*)m) + handleSize);
  }

void GlobalAllocator::free(void *m)
  {
  if(!m)
    {
    return;
    }

#ifdef X_ENABLE_EVENT_LOGGING
  MemoryHandleHeader *h = (MemoryHandleHeader*)m;
  h = h-1;

  xsize allocSize = h->allocSize;

  X_MEMORY_LOGGER_FREE(xTotalGlobalAllocatorSize, allocSize);
#endif

  const xsize handleSize = MemoryHandleHeaderSize;

  void *mem = (((xuint8*)m) - handleSize);

  --_allocationCount;

#ifndef Q_CC_MSVC
  qFreeAligned(mem);
#else
  _aligned_free(mem);
#endif
  }

}
