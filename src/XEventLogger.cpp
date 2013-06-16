#include "XEventLogger"
#include "QtCore/QThreadStorage"
#include "QtCore/QThread"
#include "XCore"
#include "XStringSimple"

namespace Eks
{

EventLocation::EventLocation(const CodeLocation &l, const char *data)
  {
  xAssert(Core::eventLogger());
  Core::eventLogger()->createLocation(this, l, data);
  }

ThreadEventLogger::ThreadEventLogger(QThread *t, Eks::AllocatorBase *alloc)
    : _primaryVector(alloc),
      _secondaryVector(alloc)
  {
  _thread = t;
  _allocator = alloc;
  _currentID = 0;

  _events = &_primaryVector;
  _availableEvents = &_secondaryVector;
  }

ThreadEventLogger::~ThreadEventLogger()
  {
  }

void *ThreadEventLogger::operator new(size_t, void *w)
  {
  return w;
  }

void ThreadEventLogger::operator delete(void *, void *)
  {
  xAssertFail();
  }

void ThreadEventLogger::operator delete(void* ptr)
  {
  auto a = Core::eventLogger()->deleteThread(static_cast<ThreadEventLogger*>(ptr));

  a->free(ptr);
  }

ThreadEventLogger::EventID ThreadEventLogger::beginDurationEvent(const EventLocation::ID location)
  {
  xAssert(_currentID < X_SIZE_SENTINEL);

  xsize id = _currentID++;
  addItem(EventType::Begin, location, id);

  return id;
  }

void ThreadEventLogger::endDurationEvent(EventID id)
  {
  addItem(EventType::End, X_UINT32_SENTINEL, id);

  if(id == (_currentID - 1))
    {
    --_currentID;
    }
  }

void ThreadEventLogger::momentEvent(const EventLocation::ID loc)
  {
  addItem(EventType::Moment, loc, X_SIZE_SENTINEL);
  }

ThreadEventLogger::EventVector *ThreadEventLogger::swapEventVector(EventVector *vec)
  {
  auto i = _events.exchange(vec);
  return i;
  }

ThreadEventLogger::EventVector *ThreadEventLogger::getAvailableEvents()
  {
  return _availableEvents;
  }

void ThreadEventLogger::setAvailableEvents(EventVector *e)
  {
  xAssert(e);
  _availableEvents = e;
  }

void ThreadEventLogger::addItem(EventType type, const EventLocation::ID d, xsize id)
  {
  auto locked = _events.exchange(0);
  // at this point _events may be written over,
  // we must ignore any value in there until we put back our value.

  xAssert(locked);

  locked->resize(locked->size() + 1);

  auto &item = locked->back();
  item.time = Time::now();
  item.type = type;
  item.id = id;
  item.location = d;

  // spin to ensure we set correctly,
  // below if syncing we swap the vector out
  // and put it back if zero.
  _events.exchange(locked);
  }

class EventLogger::Impl
  {
public:
  Eks::AllocatorBase *_allocator;
  EventLogger::Watcher *_watcher;
  QThreadStorage<ThreadEventLogger*> _logger;

  std::atomic_flag _locationLock;
  EventLogger::EventLocationVector _locations;
  EventLocation::ID _locationID;

  std::atomic_flag _loggerLock;
  ThreadEventLogger *_lastLogger;
  };

EventLogger::EventLogger(Eks::AllocatorBase *allocator)
  {
  _impl.create(allocator);
  _impl->_allocator = allocator;
  _impl->_lastLogger = 0;
  _impl->_loggerLock.clear();
  _impl->_locationLock.clear();

  _impl->_locationID = 0;

  _impl->_watcher = 0;
  }

EventLogger::~EventLogger()
  {
  }

ThreadEventLogger *EventLogger::threadLogger()
  {
  if(!_impl->_logger.hasLocalData())
    {
    ThreadEventLogger *logger =
        _impl->_allocator->create<ThreadEventLogger>(
          QThread::currentThread(),
          _impl->_allocator);
    _impl->_logger.setLocalData(logger);

    logger->setNext(_impl->_lastLogger);
    _impl->_lastLogger = logger;
    }

  return _impl->_logger.localData();
  }

void EventLogger::setEventWatcher(Watcher *w)
  {
  xAssert(!_impl->_watcher || !w);
  _impl->_watcher = w;
  }

void EventLogger::createLocation(EventLocation *loc, const CodeLocation &locData, const char *data)
  {
  while (_impl->_locationLock.test_and_set(std::memory_order_acquire)) ; // spin
  loc->setId(_impl->_locationID++);

  LocationReference ref = { loc->id(), locData.file(), locData.line(), locData.function(), data };

  _impl->_locations << ref;
  _impl->_locationLock.clear(std::memory_order_release);
  }

void EventLogger::syncCachedEvents()
  {
  if(_impl->_watcher)
    {
    while (_impl->_locationLock.test_and_set(std::memory_order_acquire)) ; // spin

    if(_impl->_locations.size())
      {
      _impl->_watcher->onLocations(_impl->_locations);
      _impl->_locations.clear();
      }

    _impl->_locationLock.clear(std::memory_order_release);
    }

  while (_impl->_loggerLock.test_and_set(std::memory_order_acquire)) ; // spin

  auto w = _impl->_lastLogger;
  while(w)
    {
    ThreadEventLogger::EventVector *availableEvents = w->getAvailableEvents();
    xAssert(availableEvents);
    xAssert(availableEvents->isEmpty());
    
    auto vec = w->swapEventVector(availableEvents);

    // if there is no vector, its locked for writing, continue,
    // assume our vector is cleared in a sec, dont write over the memory,
    // as it may already be cleared.
    if(!vec)
      {
      xAssert(availableEvents->isEmpty());
      continue;
      }

    xAssert(vec != availableEvents || vec->size() == 0);
    w->setAvailableEvents(vec);

    if(_impl->_watcher && vec->size())
      {
      _impl->_watcher->onEvents(w->thread(), *vec);
      }

    vec->clear();

    w = w->next();
    }

  _impl->_loggerLock.clear(std::memory_order_release);
  }

Eks::AllocatorBase *EventLogger::deleteThread(ThreadEventLogger *t)
  {
  while (_impl->_loggerLock.test_and_set(std::memory_order_acquire)) ; // spin

  auto w = &_impl->_lastLogger;
  while(*w)
    {
    if(*w == t)
      {
      (*w) = (*w)->next();
      break;
      }

    *w = (*w)->next();
    }

  _impl->_loggerLock.clear(std::memory_order_release);

  return _impl->_allocator;
  }

#if X_EVENT_LOGGING_ENABLED

ScopedEvent::ScopedEvent(const EventLocation *loc)
  {
  auto locationId = loc->id();
  _id = Core::eventLogger()->threadLogger()->beginDurationEvent(locationId);
  }

ScopedEvent::~ScopedEvent()
  {
  Core::eventLogger()->threadLogger()->endDurationEvent(_id);
  }

#endif
}
