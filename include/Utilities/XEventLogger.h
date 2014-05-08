#ifndef XEVENTLOGGER_H
#define XEVENTLOGGER_H

#include "XGlobal.h"
#include "Utilities/XTime.h"
#include "Containers/XVector.h"
#include <atomic>

#if 0

namespace Eks
{

class AllocatorBase;

class EKSCORE_EXPORT EventLocation
  {
public:
  typedef xuint32 ID;

XProperties:
  XProperty(ID, id, setId);

public:
  EventLocation(const CodeLocation &l, const char *data = "");
  };

class EKSCORE_EXPORT ThreadEventLogger
  {
XProperties:
  XROProperty(QThread *, thread);
  XProperty(ThreadEventLogger *, next, setNext);

public:

  enum class EventType : xuint8
    {
    Begin,
    End,
    Moment
    };

  typedef xuint64 EventID;
  struct EventItem
    {
    Eks::Time time;
    EventType type;
    EventLocation::ID location;
    EventID id;
    };
  typedef Eks::Vector<EventItem, 1024> EventVector;

  ThreadEventLogger(QThread *thread, Eks::AllocatorBase *allocator);
  ~ThreadEventLogger();

  EventID beginDurationEvent(const EventLocation::ID location);
  void endDurationEvent(EventID id);

  void momentEvent(const EventLocation::ID location);

  EventVector *swapEventVector(EventVector *vec);

  EventVector *getAvailableEvents();
  void setAvailableEvents(EventVector *);

  void *operator new(size_t, void *w);
  void operator delete(void *ptr, void *);
  void operator delete(void* ptr);

private:
  void addItem(EventType type, const EventLocation::ID d, xsize id);

  Eks::AllocatorBase *_allocator;
  EventID _currentID;

  std::atomic<EventVector *> _events;
  EventVector * _availableEvents;

  EventVector _primaryVector;
  EventVector _secondaryVector;
  };

class EKSCORE_EXPORT EventLogger
  {
public:
  EventLogger(Eks::AllocatorBase *);
  ~EventLogger();

  ThreadEventLogger *threadLogger();

  void createLocation(EventLocation *, const CodeLocation &loc, const char *data);

  struct LocationReference
    {
    EventLocation::ID id;
    const char *file;
    xuint32 line;
    const char *function;
    const char *data;
    };

  typedef Eks::Vector<LocationReference, 1024> EventLocationVector;

  class Watcher
    {
  public:
    virtual void onEvents(
        const QThread *thread,
        const ThreadEventLogger::EventVector &) = 0;

    virtual void onLocations(const EventLocationVector &) = 0;
    };

  void setEventWatcher(Watcher *w);

  void syncCachedEvents();

  Eks::AllocatorBase *deleteThread(ThreadEventLogger *t);

private:
  class Impl;
  Eks::UniquePointer<Impl> _impl;
  };

#if X_EVENT_LOGGING_ENABLED

class EKSCORE_EXPORT ScopedEvent
  {
public:
  ScopedEvent(const EventLocation *);
  ~ScopedEvent();

private:
  ThreadEventLogger::EventID _id;
  };

# define X_EVENT_FUNCTION \
  static const Eks::EventLocation __loc##__LINE__(X_CURRENT_CODE_LOCATION); Eks::ScopedEvent _EVENT_##__LINE__(&__loc##__LINE__)

# define X_EVENT_BLOCK(data) \
  static const Eks::EventLocation __loc##__LINE__(X_CURRENT_CODE_LOCATION, data); Eks::ScopedEvent _EVENT_##__LINE__(&__loc##__LINE__)

#else

# define X_EVENT_FUNCTION
# define X_EVENT_BLOCK(data)

#endif

}

#endif

#endif // XEVENTLOGGER_H
