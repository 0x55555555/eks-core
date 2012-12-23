#include "XAllocatorBase"
#include "XAssert"

#ifndef Q_CC_MSVC
# include <malloc.h>
#endif

X_IMPLEMENT_MEMORY_LOGGER(xTotalGlobalAllocatorSize);

namespace Eks
{

GlobalAllocator g_allocator;

GlobalAllocator *GlobalAllocator::instance()
  {
  return &g_allocator;
  }

struct MemoryHandle
  {
  xsize allocSize;
  xuint32 padding[(sizeof(xsize) == 4) ? 3 : 2];
  };


void *GlobalAllocator::alloc(xsize size, xsize alignment)
  {
  xAssert(((alignment-1) & sizeof(MemoryHandle)) == 0);

  X_MEMORY_LOGGER_ALLOC(xTotalGlobalAllocatorSize, size);

#ifndef Q_CC_MSVC
  void *m = qMallocAligned(sizeof(MemoryHandle) + size, alignment);

#else
  // msvc malloc should always be 16 byte aligned
  xAssert(alignment == 16);
  void *m = _aligned_malloc(sizeof(MemoryHandle) + size, alignment);

#endif


  xAssertIsAligned(m);

  MemoryHandle *handle = (MemoryHandle*)m;

  handle->allocSize = size;

  return handle+1;
  }

void GlobalAllocator::free(void *mem)
  {
  if(mem)
    {
    MemoryHandle *h = (MemoryHandle*)mem;
    h = h-1;

    X_MEMORY_LOGGER_FREE(xTotalGlobalAllocatorSize, h->allocSize);

#ifndef Q_CC_MSVC
    qFreeAligned(h);
#else
    _aligned_free(h);
#endif
    }
  }

}
