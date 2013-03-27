#include "XAsyncTask"
#include "XAssert"

namespace Eks
{

namespace detail
{

TaskImpl *TaskImpl::create(ThreadSafeAllocator *allocator)
  {
  // todo, allocator

  TaskImpl *impl = allocator->create<TaskImpl>();
  
  impl->_onDependencyComplete = 0;
  impl->_dependentCount = 0;
  impl->_allocator = allocator;

  return impl;
  }

void TaskImpl::completeAndDestroy(TaskImpl *impl, const void *result)
  {
  for(size_t i = 0; i < impl->_dependentCount; ++i)
    {
    TaskImpl *dep = impl->_dependents[i];

    dep->_onDependencyComplete(dep, impl, result);
    }

  impl->_allocator->destroy(impl);
  }

void TaskImpl::addDependent(TaskImpl *dep)
  {
  xAssert(_dependentCount < MaxDependents);
  _dependents[_dependentCount++] = dep;
  }

}
}
