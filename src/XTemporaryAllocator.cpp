#include "Memory/XTemporaryAllocator.h"

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

TemporaryAllocatorCore::Block *TemporaryAllocatorCore::findBlock(xsize expectedSize)
  {
  Block **prevB = &_freeBlock;
  Block *b = _freeBlock;
  while(b && (xsize)(b->end - b->data) < expectedSize)
    {
    prevB = &b->next;
    b = b->next;
    }

  if(b)
    {
    *prevB = b->next;

    b->next = 0;

    return b;
    }

  return createBlock(expectedSize);
  }

void TemporaryAllocatorCore::releaseBlock(Block *b)
  {
  xAssert(b);
  xAssert(b->next == 0);
  
  b->current = b->data;
  b->next = _freeBlock;
  _freeBlock = b;
  }

TemporaryAllocatorCore::Block *TemporaryAllocatorCore::createBlock(xsize expected)
  {
  xsize newSize = std::max(expected, _blockSize);

  Block *b = (Block *)_allocator->alloc(sizeof(Block) + newSize);
  b->current = b->data;
  b->next = 0;
  b->end = b->data + newSize;

  return b;
  }

TemporaryAllocator::TemporaryAllocator(TemporaryAllocatorCore *core)
    : _current(0),
      _used(nullptr),
      _allocationCount(0)
  {
  init(core);
  }

TemporaryAllocator::~TemporaryAllocator()
  {
  reset();
  }

void TemporaryAllocator::reset()
  {
  xAssert(_allocationCount == 0);
  xAssert(!_used || _current);

  if(_current)
    {
    _core->releaseBlock(_current);
    }

  TemporaryAllocatorCore::Block *b = _used;
  while(b)
    {
    TemporaryAllocatorCore::Block *next = b->next;

    b->next = 0;
    _core->releaseBlock(b);

    b = next;
    }

  _current = nullptr;
  _used = nullptr;
  _allocationCount = 0;
  }

void TemporaryAllocator::init(TemporaryAllocatorCore *core)
  {
  xAssert(!_current);
  xAssert(!_used);
  xAssert(!_allocationCount);

  _core = core;
  }

void *TemporaryAllocator::alloc(xsize size, xsize alignment)
  {
  xAssert(_core);
  ++_allocationCount;

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

  static float ExpandFactor = 2.0f;

  // get a new current
  _current = 0;
  _current = _core->findBlock(size * ExpandFactor);

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
