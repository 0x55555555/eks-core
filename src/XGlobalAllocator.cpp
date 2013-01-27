#include "XGlobalAllocator"
#include "XEventLogger"
#include "XAssert"

#ifndef Q_CC_MSVC
# include <malloc.h>
#endif

X_DECLARE_MEMORY_LOGGER(EKSCORE_EXPORT, xTotalGlobalAllocatorSize);
X_IMPLEMENT_MEMORY_LOGGER(xTotalGlobalAllocatorSize);

namespace Eks
{

GlobalAllocator g_allocator;

GlobalAllocator *GlobalAllocator::instance()
  {
  return &g_allocator;
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

#ifdef MemoryHandleHeader
  MemoryHandleHeader *handle = (MemoryHandleHeader*)m;

  handle->allocSize = size;
#endif

  return (void*)(((xuint8*)m) + MemoryHandleHeaderSize);
  }

void GlobalAllocator::free(void *m)
  {
  if(!m)
    {
    return;
    }

#ifdef MemoryHandleHeader
  MemoryHandleHeader *h = (MemoryHandleHeader*)m;
  h = h-1;

  xsize allocSize = h->allocSize;

  X_MEMORY_LOGGER_FREE(xTotalGlobalAllocatorSize, allocSize);
#endif

  void *mem = (((xuint8*)m) - MemoryHandleHeaderSize);
  


#ifndef Q_CC_MSVC
  qFreeAligned(mem);
#else
  _aligned_free(mem);
#endif
  }

}
