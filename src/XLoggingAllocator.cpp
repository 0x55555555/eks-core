#include "XLoggingAllocator"
#include "XAssert"
#include "QDebug"
#include "unordered_map"

namespace Eks
{

struct Allocation
  {
  std::string _stack;
  };

class LoggingAllocator::Impl
  {
public:
  // deliberately dont use Eks types here - no memory monitoring!
  std::string _walkCache;
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
  qDebug() << "Logging live allocations:";
  xForeach(auto &a, _impl->_allocations)
    {
    qDebug() << "Allocation\n" << a.second._stack.data();
    }
  }

void *LoggingAllocator::alloc(xsize size, xsize alignment)
  {
  void *result = _impl->_parent->alloc(size, alignment);
  class Visitor : public StackWalker::Visitor
    {
  public:
    std::string *location;
    void visit(xsize level, const char* name, xsize offset) X_OVERRIDE
      {
      if(strlen(name))
        {
        (void)level;
        (void)offset;
        *location += name;
        *location += "\n";
        }
      }
    } visitor;

  _impl->_walkCache.clear();
  visitor.location = &_impl->_walkCache;

  StackWalker::walk(1, &visitor);

  xAssert(_impl->_allocations.find(result) == _impl->_allocations.end());
  _impl->_allocations[result]._stack = _impl->_walkCache;

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
