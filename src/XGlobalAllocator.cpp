#include "Memory/XGlobalAllocator.h"
#include "Utilities/XEventLogger.h"
#include "Utilities/XAssert.h"
#include "Memory/XLoggingAllocator.h"

#ifndef Q_CC_MSVC
# include <malloc.h>
#endif

namespace Eks
{

GlobalAllocator g_allocator;

GlobalAllocator::GlobalAllocator()
  {
  }

GlobalAllocator::~GlobalAllocator()
  {
  }

void *GlobalAllocator::alloc(xsize size, xsize alignment)
  {
#ifndef Q_CC_MSVC
  void *m = qMallocAligned(size, alignment);

#else
  // msvc malloc should always be 16 byte aligned
  void *m = _aligned_malloc(size, alignment);

#endif


  xAssertIsSpecificAligned(m, alignment);

  return m;
  }

void GlobalAllocator::free(void *mem)
  {
  if(!mem)
    {
    return;
    }

#ifndef Q_CC_MSVC
  qFreeAligned(mem);
#else
  _aligned_free(mem);
#endif
  }

}
