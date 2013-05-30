#include "XEventLogger"
#include "QtCore/QThreadStorage"
#include "QtCore/QThread"
#include "XCore"
#include "XStringSimple"

namespace Eks
{
ThreadEventLogger::EventData::EventData()
  {
  _hasEmbeddedLocation = false;
  _hasAllocatedLocation = false;
  }

ThreadEventLogger::EventData::EventData(const EventData &d)
  : _hasEmbeddedLocation(d._hasEmbeddedLocation),
    _hasAllocatedLocation(d._hasAllocatedLocation)
  {
  if(_hasEmbeddedLocation)
    {
    setLocation(d.location());
    }
  if(_hasAllocatedLocation)
    {
    setAllocatedLocation(d.allocatedLocation());
    }
  }

void ThreadEventLogger::EventData::clear()
  {
  if(_hasEmbeddedLocation)
    {
    reinterpret_cast<CodeLocation*>(&_data)->~CodeLocation();
    }
  if(_hasAllocatedLocation)
    {
    const Location &l = allocatedLocation();
    (void)l;
    reinterpret_cast<Location*>(&_data)->~Location();
    }
  }

ThreadEventLogger::EventData::~EventData()
  {
  clear();
  }

void ThreadEventLogger::EventData::setLocation(const CodeLocation &loc)
  {
  clear();
  _hasEmbeddedLocation = true;
  new(&_data) CodeLocation(loc);
  }

const CodeLocation &ThreadEventLogger::EventData::location() const
  {
  xAssert(_hasEmbeddedLocation);
  return *reinterpret_cast<const CodeLocation*>(&_data);
  }

void ThreadEventLogger::EventData::setAllocatedLocation(const Location &loc)
  {
  clear();
  _hasAllocatedLocation = true;
  new(&_data) Location(loc);
  }

const ThreadEventLogger::EventData::Location &ThreadEventLogger::EventData::allocatedLocation() const
  {
  xAssert(_hasAllocatedLocation);
  return *reinterpret_cast<const Location*>(&_data);
  }

ThreadEventLogger::ThreadEventLogger(QThread *t, Eks::AllocatorBase *alloc)
  {
  _thread = t;
  _allocator = alloc;
  _currentID = 0;
  _lockedEvents = 0;

  _events = _allocator->create<EventVector>(_allocator);
  }

ThreadEventLogger::~ThreadEventLogger()
  {
  _allocator->destroy(_events.exchange(0));
  _events = 0;
  }

ThreadEventLogger::EventID ThreadEventLogger::beginDurationEvent(const EventData *e)
  {
  xAssert(_currentID < X_SIZE_SENTINEL);
  EventItem *item = addAndLockItems();

  item->time = Time::now();
  item->type = EventType::Begin;
  item->data = *e;
  xsize id = item->id = _currentID++;

  unlockItems();

  return id;
  }

void ThreadEventLogger::endDurationEvent(EventID id)
  {
  EventItem *item = addAndLockItems();

  item->time = Time::now();
  item->type = EventType::End;
  item->id = id;

  if(id == (_currentID - 1))
    {
    --_currentID;
    }

  unlockItems();
  }

void ThreadEventLogger::momentEvent(const EventData *e)
  {
  EventItem *item = addAndLockItems();

  item->time = Time::now();
  item->type = EventType::Moment;
  item->data = *e;
  item->id = _currentID++;

  unlockItems();
  }

ThreadEventLogger::EventVector *ThreadEventLogger::swapEventVector(EventVector *vec)
  {
  return _events.exchange(vec);
  }

ThreadEventLogger::EventItem *ThreadEventLogger::addAndLockItems()
  {
  _lockedEvents = _events.exchange(0);

  xAssert(_lockedEvents);
  _lockedEvents->resize(_lockedEvents->size() + 1);

  return &_lockedEvents->back();
  }

void ThreadEventLogger::unlockItems()
  {
  _lockedEvents = _events.exchange(_lockedEvents);
  xAssert(!_lockedEvents);
  }

class EventLogger::Impl
  {
public:
  Eks::AllocatorBase *_allocator;
  EventLogger::Watcher *_watcher;
  QThreadStorage<ThreadEventLogger*> _logger;

  ThreadEventLogger::EventVector *_eventSwap;

  struct ThreadEventLoggerWrap
    {
    ThreadEventLoggerWrap(QThread *t, Eks::AllocatorBase *a) : d(t, a), next(0) { }
    ThreadEventLogger d;
    ThreadEventLoggerWrap *next;
    };

  ThreadEventLoggerWrap *_lastLogger;
  };

EventLogger::EventLogger(Eks::AllocatorBase *allocator)
  {
  _impl.create(allocator);
  _impl->_allocator = allocator;
  _impl->_lastLogger = 0;

  _impl->_watcher = 0;

  _impl->_eventSwap = _impl->_allocator->create<ThreadEventLogger::EventVector>(_impl->_allocator);
  }

EventLogger::~EventLogger()
  {
  _impl->_allocator->destroy(_impl->_eventSwap);
  }

ThreadEventLogger *EventLogger::threadLogger()
  {
  if(!_impl->_logger.hasLocalData())
    {
    Impl::ThreadEventLoggerWrap *logger =
        _impl->_allocator->create<Impl::ThreadEventLoggerWrap>(
          QThread::currentThread(),
          _impl->_allocator);
    _impl->_logger.setLocalData(&logger->d);

    logger->next = _impl->_lastLogger;
    _impl->_lastLogger = logger;
    }

  return _impl->_logger.localData();
  }

void EventLogger::setEventWatcher(Watcher *w)
  {
  xAssert(!_impl->_watcher || !w);
  _impl->_watcher = w;
  }

void EventLogger::syncCachedEvents()
  {
  auto w = _impl->_lastLogger;
  while(w)
    {
    xAssert(_impl->_eventSwap);
    xAssert(_impl->_eventSwap->size() == 0);

    auto vec = w->d.swapEventVector(_impl->_eventSwap);
    _impl->_eventSwap = vec;

    if(_impl->_watcher && vec->size())
      {
      _impl->_watcher->onEvents(w->d.thread(), *vec);
      }

    vec->clear();

    w = w->next;
    }
  }

#if X_EVENT_LOGGING_ENABLED

ScopedEvent::ScopedEvent(const CodeLocation &loc, const QString &dataStr)
  {
  ThreadEventLogger::EventData data;
  data.setLocation(loc);
  data.data = dataStr;

  _id = Core::eventLogger()->threadLogger()->beginDurationEvent(&data);
  }

ScopedEvent::~ScopedEvent()
  {
  Core::eventLogger()->threadLogger()->endDurationEvent(_id);
  }

#endif
}
