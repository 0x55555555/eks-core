#define builtIn
#include "XProfiler"
#include "QThread"
#include "QMutexLocker"

namespace Eks
{

ProfilerProfilingContext::ProfilerProfilingContext(
    ProfilerProfilingContext* parent,
    xuint32 context,
    const char *message)
    : _parent(parent), _context(context), _firstChild(0), _nextSibling(0), _message(message)
  {
  }

Profiler* g_instance = 0;

ProfilerProfilingContext *ProfilerProfilingContext::findChildContext(xuint32 context, const char *mes)
  {
  xAssert(g_instance);

  ProfilerProfilingContext* child = firstChild();
  if(child)
    {
    ProfilerProfilingContext* oldSibling = 0;
    while(child)
      {
      if(child->context() == context && strcmp(mes, child->message()) == 0)
        {
        return child;
        }
      oldSibling = child;
      child = child->nextSibling();
      }
    oldSibling->_nextSibling = (ProfilerProfilingContext*)g_instance->_contextAllocator.alloc();
    new(oldSibling->_nextSibling) ProfilerProfilingContext(this, context, mes);
    return oldSibling->_nextSibling;
    }

  _firstChild = (ProfilerProfilingContext*)g_instance->_contextAllocator.alloc();
  new(_firstChild) ProfilerProfilingContext(this, context, mes);
  return _firstChild;
  }

const ProfilerProfilingContext *ProfilerProfilingContext::findChildContext(xuint32 context, const char *mes) const
  {
  xAssert(g_instance);

  ProfilerProfilingContext* child = firstChild();
  while(child)
    {
    if(child->context() == context && strcmp(mes, child->message()) == 0)
      {
      return child;
      }
    child = child->nextSibling();
    }
  return 0;
  }

Profiler::ProfileHandle::ProfileHandle(ProfilingContext* ctx)
    : _context(ctx), _start(XTime::now())
  {
  }


Profiler::ProfileHandle Profiler::start(xuint32 component, const char *mess)
  {
  if(g_instance == 0)
    {
    g_instance = new Profiler();
    }
  QMutexLocker lock(&g_instance->_lock);

  ProfilingContext *&currentCtx = g_instance->_currentContexts[QThread::currentThread()];
  if(currentCtx)
    {
    ProfilingContext *ctx = currentCtx->findChildContext(component, mess);
    currentCtx = ctx;
    }
  else
    {
    currentCtx = (ProfilingContext*)g_instance->_contextAllocator.alloc();
    new(currentCtx) ProfilingContext(g_instance->_rootContext, component, mess);
    }

  return ProfileHandle(currentCtx);
  }

void Profiler::end(const ProfileHandle &handle)
  {
  xAssert(g_instance);
  QMutexLocker lock(&g_instance->_lock);

  XTimeStatistics &stats = handle._context->timeStats();
  stats.append(XTime::now() - handle._start);

  xAssert(QThread::currentThread());
  ProfilingContext *&currentCtx = g_instance->_currentContexts[QThread::currentThread()];
  xAssert(currentCtx);
  currentCtx = currentCtx->parent();
  }

Profiler::ProfilingContext *Profiler::rootContext()
  {
  if(g_instance)
    {
    return g_instance->_rootContext;
    }
  return 0;
  }

void Profiler::clearResults()
  {
  // todo: write this neatly, but for now this will work.

  if(g_instance)
    {
    g_instance->_currentContexts.clear();
    g_instance->_contextAllocator.~FixedSizeBucketAllocator();
    new(&g_instance->_contextAllocator) FixedSizeBucketAllocator(sizeof(ProfilingContext), 256, 1024);

    g_instance->_rootContext = (ProfilingContext *)g_instance->_contextAllocator.alloc();
    new(g_instance->_rootContext) ProfilingContext(0, X_UINT32_SENTINEL, "");
    }
  }

QString Profiler::stringForContext(xuint32 t)
  {
  if(g_instance)
    {
    return g_instance->_contextStrings[t];
    }
  return "";
  }

void Profiler::setStringForContext(xuint32 t, const QString &str)
  {
  if(g_instance == 0)
    {
    g_instance = new Profiler();
    }

  g_instance->_contextStrings[t] = str;
  }

Profiler::Profiler() : _contextAllocator(sizeof(ProfilingContext), 256, 1024)
  {
  _rootContext = (ProfilingContext *)_contextAllocator.alloc();
  new(_rootContext) ProfilingContext(0, X_UINT32_SENTINEL, "");
  }

Profiler::ProfileScopedBlock::ProfileScopedBlock(xuint32 component, const char *message)
    : _handle(Profiler::start(component, message))
  {
  }

Profiler::ProfileScopedBlock::~ProfileScopedBlock()
  {
  Profiler::end(_handle);
  }

}
