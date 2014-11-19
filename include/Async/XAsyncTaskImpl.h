#ifndef XASYNCTASKIMPL_H
#define XASYNCTASKIMPL_H

#include "Memory/XThreadSafeAllocator.h"
#include "Async/XAsyncTask.h"

// Implementation details of Eks::Task

namespace Eks
{

namespace detail
{

class EKSCORE_EXPORT TaskImpl
  {
public:
  static TaskImpl *create(ThreadSafeAllocator *allocator);
  static void completeAndDestroy(TaskImpl *, const void *result);

  void addDependent(TaskImpl *);

  template <typename T> void setUserData(const T &t)
    {
    xCompileTimeAssert(sizeof(T) <= UserDataSize);
    new(_userData) T(t);
    }

  template <typename T> T &createUserData()
    {
    xCompileTimeAssert(sizeof(T) <= UserDataSize);
    return *new(_userData) T();
    }

  template <typename T> void clearUserData()
    {
    userData<T>().~T();
    }

  template <typename T> const T &userData() const
    {
    return *reinterpret_cast<const T*>(_userData);
    }

  template <typename T> T &userData()
    {
    return *reinterpret_cast<T*>(_userData);
    }

  enum
    {
    MaxDependents = 2,
    UserDataSize = sizeof(void *) * 2,
    };

  xuint8 _userData[UserDataSize];
  void (*_onDependencyComplete)(TaskImpl *impl, TaskImpl *dep, const void *data);
  TaskImpl *_dependents[MaxDependents];
  xsize _dependentCount;
  ThreadSafeAllocator *_allocator;
  };

template <typename Fn, typename Arg, typename ResultType> class ContinuationTaskImpl
  {
public:
  typedef ResultType Result;

  static void call(const Fn& fn, Result *res, const Arg *arg)
    {
    *res = fn(*arg);
    }
  };

template <typename Fn> class ContinuationTaskImpl<Fn, void, void>
  {
public:
  struct Result { };

  static void call(const Fn &fn, Result *, const void *)
    {
    fn();
    }
  };

template <typename Fn, typename Arg> class ContinuationTaskImpl<Fn, Arg, void>
  {
public:
  struct Result { };

  static void call(const Fn &fn, Result *, const Arg *arg)
    {
    fn(*arg);
    }
  };

template <typename Fn, typename ResultType> class ContinuationTaskImpl<Fn, void, ResultType>
  {
public:
  typedef ResultType Result;

  static void call(const Fn &fn, Result *res, const void *)
    {
    *res = fn();
    }
  };

template <typename Fn, typename Arg, typename Result> class ContinuationTask
  {
public:
  typedef Task<Result> TaskT;

  static TaskT create(const Task<Arg> &arg, const Fn &fn)
    {
    // create a new task
    TaskT res(arg.token()->_allocator);

    // get the task token, so we can set up dependencies
    typename TaskT::Token token = res.token();

    // token needs to depend on the arg
    arg.token()->addDependent(token);

    // set the userdata of the token so we can get it in complete
    token->setUserData(fn);

    // setup the complete callback for the token
    token->_onDependencyComplete = complete;

    return res;
    }

  static void complete(TaskImpl *ths, TaskImpl *, const void *arg)
    {
    // get the arg as the right type
    const Arg *argData = reinterpret_cast<const Arg*>(arg);
    // get the function from userdata
    const Fn &data = ths->userData<Fn>();

    typedef ContinuationTaskImpl<Fn, Arg, Result> Impl;

    // get a result object (might be an empty struct)
    typename Impl::Result res;
    // call the task function
    Impl::call(data, &res, argData);

    // complete the task and any dependents syncronously
    TaskT::completeTaskSync(ths, &res);

    // delete the userdata object (important if its a functor)
    ths->clearUserData<Fn>();
    }
  };

}

template <typename Result> Task<Result>::Task(Eks::ThreadSafeAllocator *allocator)
    : _token(Impl::create(allocator))
  {
  }

template <typename ResultType>
  template <typename OthResult>
    Task<void> Task<ResultType>::whenBoth(const Task<OthResult> &oth)
  {
  typedef Task<void> TaskT;
  TaskT res(oth.token()->_allocator);

  TaskT::Token resToken = res.token();

  oth.token()->addDependent(resToken);
  token()->addDependent(resToken);

  resToken->createUserData<std::atomic<int>>() = 2;

  resToken->_onDependencyComplete = [](Impl *ths, Impl *, const void *)
    {
    std::atomic_int &data = ths->userData<std::atomic<int>>();

    // find the result before decremented.
    int val = data.fetch_sub(1);

    // if the value was one before decrementing
    // then it is now 0, so we need to do the thing.
    if(val == 1)
      {
      ths->clearUserData<std::atomic<int>>();

      TaskT::completeTaskSync(ths, (TaskT::Result*)0);
      }
    };

  return res;
  }

template <typename Result>
  template <typename T>
    void Task<Result>::completeTaskSync(Token token, const T *val)
  {
  Impl::completeAndDestroy(token, val);
  }

}

#endif // XASYNCTASKIMPL_H
