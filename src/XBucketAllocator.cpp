#include "Memory/XBucketAllocator.h"
#include "QDebug"

namespace Eks
{

void BucketAllocator::debugDump() const
  {
  Q_FOREACH(xsize s, _internal.keys())
    {
    FixedSizeBucketAllocatorBase *alloc = _internal.value(s);
    alloc->debugDump();
    }
  }

void FixedSizeBucketAllocatorBase::debugDump() const
  {
  qDebug() << "Bucket allocator (Size: " << _size << " bytes)," << bucketCount() << "buckets.";
  if(!empty())
    {
    xsize i = 0;
    Bucket *b = _first;
    while(b)
      {
      qDebug() << "  Bucket" << i++;
      b->debugDump();
      b = b->next();
      }
    }
  else
    {
    qDebug() << "  Empty.";
    }
  }

void FixedSizeBucketAllocatorBase::Bucket::debugDump() const
  {
  xuint32 mask = 0;
  for(xsize i=0, s=(_count/32); i<s; ++i)
    {
    mask = _masks[i];
    qDebug() << "    Mask " << i << "/" << s << ": Free mask:" << mask << "|" << QString::number(mask, 2).rightJustified(32, QChar('0'));
    }
  }

}
