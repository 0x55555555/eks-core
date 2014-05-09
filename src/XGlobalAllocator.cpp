#include "Memory/XGlobalAllocator.h"
#include "Utilities/XEventLogger.h"
#include "Utilities/XAssert.h"
#include "Memory/XLoggingAllocator.h"

#ifndef X_WIN
# include <stdlib.h>
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
  void *m = nullptr;

#ifndef X_WIN
  if (alignment != 1)
    {
    posix_memalign(&m, std::min(alignment, sizeof(void*)), size);
    }
  else
    {
    m = malloc(size);
    }

#else
  // msvc malloc should always be 16 byte aligned
  m = _aligned_malloc(size, alignment);

#endif

  xAssert(m);
  xAssertIsSpecificAligned(m, alignment);

  return m;
  }

void GlobalAllocator::free(void *mem)
  {
  if(!mem)
    {
    return;
    }

#ifndef X_WIN
  ::free(mem);
#else
  _aligned_free(mem);
#endif
  }

}
