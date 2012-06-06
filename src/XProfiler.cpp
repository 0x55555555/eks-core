#define builtIn
#include "XProfiler"
#include "QThread"
#include "QMutexLocker"

XProfiler::ProfilingContext::ProfilingContext(ProfilingContext* parent, xuint32 context, const char *message)
    : _parent(parent), _context(context), _firstChild(0), _nextSibling(0), _message(message)
  {
  }

XProfiler* g_instance = 0;

XProfiler::ProfilingContext *XProfiler::ProfilingContext::findChildContext(xuint32 context, const char *mes)
  {
  xAssert(g_instance);

  ProfilingContext* child = firstChild();
  if(child)
    {
    ProfilingContext* oldSibling = 0;
    while(child)
      {
      if(child->context() == context && strcmp(mes, child->message()) == 0)
        {
        return child;
        }
      oldSibling = child;
      child = child->nextSibling();
      }
    oldSibling->_nextSibling = (ProfilingContext*)g_instance->_contextAllocator.alloc();
    new(oldSibling->_nextSibling) ProfilingContext(this, context, mes);
    return oldSibling->_nextSibling;
    }

  _firstChild = (ProfilingContext*)g_instance->_contextAllocator.alloc();
  new(_firstChild) ProfilingContext(this, context, mes);
  return _firstChild;
  }

const XProfiler::ProfilingContext *XProfiler::ProfilingContext::findChildContext(xuint32 context, const char *mes) const
  {
  xAssert(g_instance);

  ProfilingContext* child = firstChild();
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

XProfiler::ProfileHandle::ProfileHandle(ProfilingContext* ctx)
    : _context(ctx), _start(XTime::now())
  {
  }


XProfiler::ProfileHandle XProfiler::start(xuint32 component, const char *mess)
  {
  if(g_instance == 0)
    {
    g_instance = new XProfiler();
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

void XProfiler::end(const ProfileHandle &handle)
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

XProfiler::ProfilingContext *XProfiler::rootContext()
  {
  if(g_instance)
    {
    return g_instance->_rootContext;
    }
  return 0;
  }

void XProfiler::clearResults()
  {
  // todo: write this neatly, but for now this will work.

  if(g_instance)
    {
    g_instance->_currentContexts.clear();
    g_instance->_contextAllocator.~XFixedSizeBucketAllocator();
    new(&g_instance->_contextAllocator) XFixedSizeBucketAllocator(sizeof(ProfilingContext), 256, 1024);

    g_instance->_rootContext = (ProfilingContext *)g_instance->_contextAllocator.alloc();
    new(g_instance->_rootContext) ProfilingContext(0, X_UINT32_SENTINEL, "");
    }
  }

QString XProfiler::stringForContext(xuint32 t)
  {
  if(g_instance)
    {
    return g_instance->_contextStrings[t];
    }
  return "";
  }

void XProfiler::setStringForContext(xuint32 t, const QString &str)
  {
  if(g_instance == 0)
    {
    g_instance = new XProfiler();
    }

  g_instance->_contextStrings[t] = str;
  }

XProfiler::XProfiler() : _contextAllocator(sizeof(ProfilingContext), 256, 1024)
  {
  _rootContext = (ProfilingContext *)_contextAllocator.alloc();
  new(_rootContext) ProfilingContext(0, X_UINT32_SENTINEL, "");
  }

XProfiler::ProfileScopedBlock::ProfileScopedBlock(xuint32 component, const char *message)
    : _handle(XProfiler::start(component, message))
  {
  }

XProfiler::ProfileScopedBlock::~ProfileScopedBlock()
  {
  XProfiler::end(_handle);
  }
