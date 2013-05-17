#include "XLoggingAllocator"
#include "XAssert"
#include "QDebug"
#include "unordered_map"
#include "XStringSimple"
#include "XGlobalAllocator"

namespace Eks
{

struct Allocation
  {
  enum
    {
    kMaxStackSize = 15
    };
  void *_symbol[kMaxStackSize];
  xsize _symbolSize;
  xsize _allocSize;
  };

class LoggingAllocator::Impl
  {
public:
  Impl()
    {
    _maxSize = 0;
    _maxAllocs = 0;
    _size = 0;
    }

  // deliberately dont use Eks types here - no memory monitoring!
  std::unordered_map<void *, Allocation> _allocations;
  Eks::AllocatorBase *_parent;
  xsize _maxSize;
  xsize _size;
  xsize _maxAllocs;
  };

LoggingAllocator::LoggingAllocator(Eks::AllocatorBase *parent)
  {
  _impl = new Impl;
  _impl->_parent = parent;
  }

LoggingAllocator::~LoggingAllocator()
  {
  if(!_impl->_allocations.empty())
    {
    logAllocations();
    }

  delete _impl;
  }

void LoggingAllocator::setAllocator(Eks::AllocatorBase *alloc)
  {
  _impl->_parent = alloc;
  }

xsize LoggingAllocator::allocationCount() const
  {
  return _impl->_allocations.size();
  }

void LoggingAllocator::logAllocations() const
  {
  Eks::StackWalker::intialiseSymbolNames();

  GlobalAllocator alloc;
  Eks::String symString(&alloc);

  qDebug() << "Logging live allocations: [" << _impl->_allocations.size() << "/" << _impl->_maxAllocs << "] [" << _impl->_size << "/" << _impl->_maxSize << "] bytes";
  xForeach(auto &a, _impl->_allocations)
    {
    qDebug() << "Allocation " << a.first << ", " << a.second._allocSize << "bytes";
    for(xsize i = 0; i < a.second._symbolSize; ++i)
      {
      StackWalker::getSymbolName(a.second._symbol[i], symString, 1024);
      qDebug() << symString;
      }
    qDebug() << "";
    }

  Eks::StackWalker::terminateSymbolNames();
  }

#define UNINITIALISED 0xCDCDCDCD
#define DELETED 0xDDDDDDDD

void *LoggingAllocator::alloc(xsize size, xsize alignment)
  {
  void *result = _impl->_parent->alloc(size, alignment);

  memset(result, UNINITIALISED, size);

  class Visitor : public StackWalker::Visitor
    {
  public:
    Allocation *allocation;
    void visit(xsize level, void *symbol) X_OVERRIDE
      {
      if(level < Allocation::kMaxStackSize)
        {
        allocation->_symbol[level] = symbol;
        allocation->_symbolSize = xMax(level+1, allocation->_symbolSize);
        }
      }
    } visitor;

  xAssert(_impl->_allocations.find(result) == _impl->_allocations.end());
  visitor.allocation = &(_impl->_allocations[result]);
  memset(visitor.allocation, 0, sizeof(Allocation));
  visitor.allocation->_allocSize = size;

  StackWalker::walk(1, &visitor);

  _impl->_maxAllocs = xMax(_impl->_allocations.size(), _impl->_maxAllocs);
  _impl->_size += size;
  _impl->_maxSize = xMax(_impl->_maxSize, _impl->_size);

  return result;
  }

void LoggingAllocator::free(void *mem)
  {
  if(mem)
    {
    std::unordered_map<void *, Allocation>::iterator it = _impl->_allocations.find(mem);
    xAssert(it != _impl->_allocations.end());

    memset(mem, DELETED, it->second._allocSize);

    _impl->_size -= it->second._allocSize;

    _impl->_allocations.erase(mem);
    }


  return _impl->_parent->free(mem);
  }

}
