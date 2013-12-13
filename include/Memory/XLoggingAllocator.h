#ifndef XLOGGINGALLOCATOR
#define XLOGGINGALLOCATOR

#include "Memory/XAllocatorBase.h"
#include "Utilities/XProperty.h"

namespace Eks
{

class EKSCORE_EXPORT LoggingAllocator : public AllocatorBase
  {
public:
  LoggingAllocator(Eks::AllocatorBase* allocator=0);
  ~LoggingAllocator();

  void setAllocator(Eks::AllocatorBase *alloc);
  xsize allocationCount() const;
  void logAllocations() const;

  void *alloc(xsize size, xsize alignment=0) X_OVERRIDE;
  void free(void *mem) X_OVERRIDE;

private:
  class Impl;
  Impl *_impl;
  };

}

#endif // XLOGGINGALLOCATOR
