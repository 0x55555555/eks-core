#ifndef XGLOBALALLOCATOR_H
#define XGLOBALALLOCATOR_H

#include "XGlobal.h"
#include "Memory/XThreadSafeAllocator.h"

namespace Eks
{

class EKSCORE_EXPORT GlobalAllocator : public ThreadSafeAllocator
  {
public:
  GlobalAllocator();
  ~GlobalAllocator();

  void *alloc(xsize size, xsize alignment=0) X_OVERRIDE;
  void free(void *mem) X_OVERRIDE;
  };

}

#endif // XGLOBALALLOCATOR_H
