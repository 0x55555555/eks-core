#ifndef XTEMPORARYALLOCATOR_H
#define XTEMPORARYALLOCATOR_H

#include "Memory/XAllocatorBase.h"

namespace Eks
{

class EKSCORE_EXPORT TemporaryAllocatorCore
  {
public:
  enum
    {
    SmallBlockSize = 1 << 12,
    LargeBlockSize = 1 << 14,
    HugeBlockSize = 1 << 16
    };
  TemporaryAllocatorCore(AllocatorBase *allocator, xsize blockSize=HugeBlockSize);
  ~TemporaryAllocatorCore();

  struct Block
    {
    void *alloc(xsize s, xsize alignment);

    Block *next;
    xuint8 *end;
    xuint8 *current;
    xuint8 data[1];
    };

  Block *findBlock(xsize expectedSize);
  void releaseBlock(Block *);

private:
  X_DISABLE_COPY(TemporaryAllocatorCore)

  Block *createBlock(xsize expected);

  Block *_freeBlock;
  xsize _blockSize;
  AllocatorBase *_allocator;
  };

class EKSCORE_EXPORT TemporaryAllocator : public AllocatorBase
  {
public:
  TemporaryAllocator(TemporaryAllocatorCore *core);
  ~TemporaryAllocator();

  void *alloc(xsize size, xsize alignment) X_OVERRIDE;
  void free(void *mem) X_OVERRIDE;

private:
  X_DISABLE_COPY(TemporaryAllocator)

  TemporaryAllocatorCore *_core;

  TemporaryAllocatorCore::Block *_current;

  TemporaryAllocatorCore::Block *_used;
  xsize _allocationCount;
  };

}

#endif // XTEMPORARYALLOCATOR_H
