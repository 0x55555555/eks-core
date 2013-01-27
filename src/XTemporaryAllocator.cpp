#include "XTemporaryAllocator"

namespace Eks
{
TemporaryAllocatorCore::TemporaryAllocatorCore(AllocatorBase *allocator, xsize blockSize)
  {
  _allocator = allocator;
  _blockSize = blockSize;
  _freeBlock = 0;
  }

TemporaryAllocatorCore::~TemporaryAllocatorCore()
  {
  Block *b = _freeBlock;
  while(b)
    {
    Block *n = b->next;
    _allocator->free(b);
    b = n;
    }
  }

void *TemporaryAllocatorCore::Block::alloc(xsize s, xsize alignment)
  {
  xuint8 *newCurrent = roundToAlignment(current, alignment);
  void *out = newCurrent;

  newCurrent += s;

  if(newCurrent <= end)
    {
    current = newCurrent;
    return out;
    }

  return 0;
  }

TemporaryAllocatorCore::Block *TemporaryAllocatorCore::findBlock()
  {
  if(_freeBlock)
    {
    Block *b = _freeBlock;
    _freeBlock = b->next;

    b->next = 0;

    return b;
    }

  return createBlock();
  }

void TemporaryAllocatorCore::releaseBlock(Block *b)
  {
  xAssert(b);
  xAssert(b->next == 0);

  b->next = _freeBlock;
  _freeBlock = b;
  }

TemporaryAllocatorCore::Block *TemporaryAllocatorCore::createBlock()
  {
  Block *b = (Block *)_allocator->alloc(sizeof(Block) + _blockSize);
  b->current = b->data;
  b->next = 0;
  b->end = b->data + _blockSize;

  return b;
  }

TemporaryAllocator::TemporaryAllocator(TemporaryAllocatorCore *core)
    : _current(0),
      _used(0),
      _allocationCount(0)
  {
  _core = core;
  }

TemporaryAllocator::~TemporaryAllocator()
  {
  xAssert(_allocationCount == 0);
  _core->releaseBlock(_current);

  TemporaryAllocatorCore::Block *b = _used;
  while(b)
    {
    TemporaryAllocatorCore::Block *next = b->next;

    b->next = 0;
    _core->releaseBlock(b);

    b = next;
    }
  }

void *TemporaryAllocator::alloc(xsize size, xsize alignment)
  {
  --_allocationCount;

  if(_current)
    {
    void *d = _current->alloc(size, alignment);
    if(d)
      {
      return d;
      }
    }
  
  // move to the used list
  if(_current)
    {
    _current->next = _used;
    }
  _used = _current;

  // get a new current
  _current = 0;
  _current = _core->findBlock();

  void *d = _current->alloc(size, alignment);
  xAssert(d);
  return d;
  }

void TemporaryAllocator::free(void *mem)
  {
  if(mem)
    {
    xAssert(_allocationCount > 0);
    --_allocationCount;
    }
  }
}
