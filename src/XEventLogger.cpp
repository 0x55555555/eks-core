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

ThreadEventLogger::EventData &ThreadEventLogger::EventData::operator=(const EventData &d)
  {
  if(d._hasEmbeddedLocation)
    {
    setLocation(d.location());
    }
  if(d._hasAllocatedLocation)
    {
    setAllocatedLocation(d.allocatedLocation());
    }
  return *this;
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

bool ThreadEventLogger::EventData::hasEmbeddedLocation() const
  {
  xAssert(!_hasAllocatedLocation || !_hasEmbeddedLocation);
  return _hasEmbeddedLocation;
  }

bool ThreadEventLogger::EventData::hasAllocatedLocation() const
  {
  xAssert(!_hasAllocatedLocation || !_hasEmbeddedLocation);
  return _hasAllocatedLocation;
  }

ThreadEventLogger::ThreadEventLogger(QThread *t, Eks::AllocatorBase *alloc)
  {
  _thread = t;
  _allocator = alloc;
  _currentID = 0;

  _events = _allocator->create<EventVector>(_allocator);

  _myEventsData = _events;
  _lockedData = 0;
  }

ThreadEventLogger::~ThreadEventLogger()
  {
  _allocator->destroy(_events.exchange(0));
  _events = 0;
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

ThreadEventLogger::EventID ThreadEventLogger::beginDurationEvent(const EventData *e)
  {
  xAssert(_currentID < X_SIZE_SENTINEL);

  xsize id = _currentID++;
  addItem(EventType::Begin, e, id);

  return id;
  }

void ThreadEventLogger::endDurationEvent(EventID id)
  {
  addItem(EventType::End, 0, id);

  if(id == (_currentID - 1))
    {
    --_currentID;
    }
  }

void ThreadEventLogger::momentEvent(const EventData *e)
  {
  addItem(EventType::Moment, e, X_SIZE_SENTINEL);
  }

ThreadEventLogger::EventVector *ThreadEventLogger::swapEventVector(EventVector *vec)
  {
  auto i = _events.exchange(vec);
  _myEventsData = i;
  return i;
  }

void ThreadEventLogger::addItem(EventType type, const EventData *d, xsize id)
  {
  auto locked = _events.exchange(0);
  _myEventsData = 0;
  _lockedData = locked;
  // at this point _events may be written over,
  // we must ignore any value in there until we put back our value.

  xAssert(locked);

  locked->resize(locked->size() + 1);

  auto &item = locked->back();
  item.time = Time::now();
  item.type = type;
  item.id = id;

  if(d)
    {
    item.data = *d;
    }

  // spin to ensure we set correctly,
  // below if syncing we swap the vector out
  // and put it back if zero.
  _events.exchange(locked);
  _myEventsData = locked;
  _lockedData = 0;
  }

class EventLogger::Impl
  {
public:
  Eks::AllocatorBase *_allocator;
  EventLogger::Watcher *_watcher;
  QThreadStorage<ThreadEventLogger*> _logger;

  ThreadEventLogger::EventVector *_eventSwap;

  std::atomic_flag _loggerLock;
  ThreadEventLogger *_lastLogger;
  };

EventLogger::EventLogger(Eks::AllocatorBase *allocator)
  {
  _impl.create(allocator);
  _impl->_allocator = allocator;
  _impl->_lastLogger = 0;
  _impl->_loggerLock.clear();

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

void EventLogger::syncCachedEvents()
  {
  while (_impl->_loggerLock.test_and_set(std::memory_order_acquire)) ; // spin

  xAssert(_impl->_eventSwap->isEmpty());

  auto w = _impl->_lastLogger;
  while(w)
    {
    ThreadEventLogger::EventVector *currentEvents = _impl->_eventSwap;
    xAssert(currentEvents);
    xAssert(currentEvents->isEmpty());
    
    auto vec = w->swapEventVector(currentEvents);

    // if there is no vector, its locked for writing, continue,
    // assume our vector is cleared in a sec, dont write over the memory,
    // as it may already be cleared.
    if(!vec)
      {
      xAssert(currentEvents->isEmpty());
      continue;
      }

    xAssert(vec != _impl->_eventSwap || vec->size() == 0);
    _impl->_eventSwap = vec;

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
