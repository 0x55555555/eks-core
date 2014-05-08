#ifndef XCORE_H
#define XCORE_H

#include "XGlobal.h"
#include "Containers/XUnorderedMap.h"

namespace Eks
{

class EventLogger;
class AllocatorBase;
class TemporaryAllocatorCore;
class CodeLocation;

class EKSCORE_EXPORT Core
  {
public:
  Core();
  ~Core();

  static AllocatorBase *defaultAllocator();
  static AllocatorBase *globalAllocator();
  static TemporaryAllocatorCore *temporaryAllocator();

  static UnorderedMap<CodeLocation, bool> *disabledAsserts();

  static EventLogger *eventLogger();

private:
  struct Impl;
  Impl *_impl;
  };
}

#endif // XCORE_H
