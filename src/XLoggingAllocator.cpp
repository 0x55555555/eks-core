#include "XLoggingAllocator"
#include "XAssert"
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
  delete _impl;
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
      (void)level;
      (void)offset;
      *location += name;
      *location += "\n";
      }
    } visitor;

  _impl->_walkCache.clear();
  visitor.location = &_impl->_walkCache;

  StackWalker::walk(1, &visitor);

  _impl->_allocations[result]._stack = _impl->_walkCache;

  return result;
  }

void LoggingAllocator::free(void *mem)
  {
  _impl->_allocations.erase(mem);

  return _impl->_parent->free(mem);
  }

}
