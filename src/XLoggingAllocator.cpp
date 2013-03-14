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
  xsize _size;
  };

class LoggingAllocator::Impl
  {
public:
  // deliberately dont use Eks types here - no memory monitoring!
  std::unordered_map<void *, Allocation> _allocations;
  Eks::AllocatorBase *_parent;
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

  qDebug() << "Logging live allocations:";
  xForeach(auto &a, _impl->_allocations)
    {
    qDebug() << "Allocation:";
    for(xsize i = 0; i < a.second._size; ++i)
      {
      StackWalker::getSymbolName(a.second._symbol[i], symString, 1024);
      qDebug() << symString;
      }
    }

  Eks::StackWalker::terminateSymbolNames();
  }

void *LoggingAllocator::alloc(xsize size, xsize alignment)
  {
  void *result = _impl->_parent->alloc(size, alignment);
  class Visitor : public StackWalker::Visitor
    {
  public:
    Allocation *allocation;
    void visit(xsize level, void *symbol) X_OVERRIDE
      {
      if(level < Allocation::kMaxStackSize)
        {
        allocation->_symbol[level] = symbol;
        allocation->_size = xMax(level+1, allocation->_size);
        }
      }
    } visitor;

  xAssert(_impl->_allocations.find(result) == _impl->_allocations.end());
  visitor.allocation = &(_impl->_allocations[result]);
  memset(visitor.allocation, 0, sizeof(Allocation));

  StackWalker::walk(1, &visitor);

  return result;
  }

void LoggingAllocator::free(void *mem)
  {
  if(mem)
    {
    std::unordered_map<void *, Allocation>::iterator it = _impl->_allocations.find(mem);

    xAssert(it != _impl->_allocations.end());
    _impl->_allocations.erase(mem);
    }

  return _impl->_parent->free(mem);
  }

}
