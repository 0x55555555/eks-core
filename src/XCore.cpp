#include "XCore"
#include "XGlobalAllocator"
#include "XLoggingAllocator"
#include "XUnorderedMap"

namespace Eks
{

struct Core::Impl
  {
  Impl()
    : disabledAsserts(&alloc)
  {
  }

  Eks::GlobalAllocator alloc;
#ifdef X_ENABLE_EVENT_LOGGING
  Eks::LoggingAllocator allocLogger;
#endif

  Eks::AllocatorBase *defaultLogger;

#if X_ASSERTS_ENABLED
  UnorderedMap<CodeLocation, bool> disabledAsserts;
#endif
  } *g_core = 0;

Core::Core()
  {
  xAssert(!g_core);
  g_core = new Core::Impl;

#ifdef X_ENABLE_EVENT_LOGGING
  g_core->allocLogger.setAllocator(&g_core->alloc);
#endif

  g_core->defaultLogger = globalAllocator();
  }

Core::~Core()
  {
  delete g_core;
  g_core = 0;
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

UnorderedMap<CodeLocation, bool> *Core::disabledAsserts()
  {
#if X_ASSERTS_ENABLED
  return &g_core->disabledAsserts;
#else
  return 0;
#endif
  }

}
