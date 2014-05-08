#include "Memory/XBucketAllocator.h"
#include <iostream>

namespace Eks
{

void BucketAllocator::debugDump() const
  {
  xForeach(xsize s, _internal.keys())
    {
    FixedSizeBucketAllocatorBase *alloc = _internal.value(s, nullptr);
    alloc->debugDump();
    }
  }

void FixedSizeBucketAllocatorBase::debugDump() const
  {
  std::cerr << "Bucket allocator (Size: " << _size << " bytes)," << bucketCount() << "buckets." << std::endl;
  if(!empty())
    {
    xsize i = 0;
    Bucket *b = _first;
    while(b)
      {
      std::cerr << "  Bucket" << i++ << std::endl;
      b->debugDump();
      b = b->next();
      }
    }
  else
    {
    std::cerr << "  Empty." << std::endl;
    }
  }

void FixedSizeBucketAllocatorBase::Bucket::debugDump() const
  {
  xuint32 mask = 0;
  for(xsize i=0, s=(_count/32); i<s; ++i)
    {
    mask = _masks[i];
    std::cerr << "    Mask " << i << "/" << s << ": Free mask:" << mask << "|" << std::to_string(mask) << std::endl;
    }
  }

}
