#include "XAllocatorBase"
#include "XAssert"

X_IMPLEMENT_MEMORY_LOGGER(xTotalGlobalAllocatorSize);

XGlobalAllocator g_allocator;

XGlobalAllocator *XGlobalAllocator::instance()
  {
  return &g_allocator;
  }

struct MemoryHandle
  {
  xsize allocSize;
  xuint32 padding[3];
  };

void *XGlobalAllocator::alloc(xsize size, xsize alignment)
  {
  xAssert(((alignment-1) & sizeof(MemoryHandle)) == 0);

  X_MEMORY_LOGGER_ALLOC(xTotalGlobalAllocatorSize, size);
  void *m = qMallocAligned(sizeof(MemoryHandle) + size, alignment);

  MemoryHandle *handle = (MemoryHandle*)m;

  handle->allocSize = size;

  return handle+1;
  }

void XGlobalAllocator::free(void *mem)
  {
  if(mem)
    {
    MemoryHandle *h = (MemoryHandle*)mem;
    h = h-1;

    X_MEMORY_LOGGER_FREE(xTotalGlobalAllocatorSize, h->allocSize);
    qFreeAligned(h);
    }
  }
