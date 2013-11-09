#include "XCore"
#include "XGlobalAllocator"
#include "XLoggingAllocator"
#include "XTemporaryAllocator"
#include "XUnorderedMap"
#include "XEventLogger"

namespace Eks
{

struct Core::Impl
  {
  Impl() :
    temporaryAllocator(&alloc)
#if X_ASSERTS_ENABLED
    , disabledAsserts(&alloc)
#endif
#if X_EVENT_LOGGING_ENABLED
    , eventLogger(&alloc)
#endif
  {
  }

  Eks::GlobalAllocator alloc;
#ifdef X_ENABLE_EVENT_LOGGING
  Eks::LoggingAllocator allocLogger;
#endif

  Eks::AllocatorBase *defaultLogger;

  Eks::TemporaryAllocatorCore temporaryAllocator;

#if X_ASSERTS_ENABLED
  UnorderedMap<CodeLocation, bool> disabledAsserts;
#endif

#if X_EVENT_LOGGING_ENABLED
  EventLogger eventLogger;
#endif

  } *g_core = 0;

Core::Core()
  {
  xAssert(!g_core);
  _impl = g_core = new Core::Impl;

#ifdef X_ENABLE_EVENT_LOGGING
  g_core->allocLogger.setAllocator(&g_core->alloc);
#endif

  g_core->defaultLogger = globalAllocator();

  }

Core::~Core()
  {
  xAssert(g_core == _impl);
  delete _impl;
  _impl = g_core = 0;
  }

AllocatorBase *Core::defaultAllocator()
  {
  xAssert(g_core);

  return g_core->defaultLogger;
  }

AllocatorBase *Core::globalAllocator()
  {
  xAssert(g_core);

#ifdef X_ENABLE_EVENT_LOGGING
  return &g_core->allocLogger;
#else
  return &g_core->alloc;
#endif
  }

TemporaryAllocatorCore *Core::temporaryAllocator()
  {
  xAssert(g_core);
  return &g_core->temporaryAllocator;
  }

UnorderedMap<CodeLocation, bool> *Core::disabledAsserts()
  {
#if X_ASSERTS_ENABLED
  return &g_core->disabledAsserts;
#else
  return 0;
#endif
  }

EventLogger *Core::eventLogger()
  {
#if X_EVENT_LOGGING_ENABLED
  return &g_core->eventLogger;
#else
  return 0;
#endif
  }

}
