#include "XTime"
#include "QDebug"
#include "XGlobal"
#include "XAssert"
#include "QDataStream"

#if defined(Q_OS_WIN)
# include <windows.h>
#else
# include <sys/time.h>
#endif

namespace Eks
{

#define SECOND_IN_NANO_SECONDS 1000000000

Time::Time(xint64 secs, xint64 nanosecs) : _secs(secs), _nanosecs(nanosecs)
  {
  }

Time Time::fromMilliseconds(double ms)
  {
  xint64 seconds = ms / 1000.0;
  xint64 nanoseconds = ms * 1000000.0;
  nanoseconds -= seconds * SECOND_IN_NANO_SECONDS;
  return Time(seconds, nanoseconds);
  }

void Time::beginAccurateTiming()
  {
#if defined(Q_OS_WIN) && !defined(X_ARCH_ARM)
  HANDLE h = GetCurrentProcess();
  SetProcessAffinityMask(h, 1);
#endif
  }

void Time::set(xint64 seconds, xint64 nanosecs)
  {
  _secs = seconds;
  _nanosecs = nanosecs;
  }

#if defined(Q_OS_WIN) && !defined(X_ARCH_ARM)
inline LARGE_INTEGER getFrequency() { LARGE_INTEGER freq; xVerify(QueryPerformanceFrequency(&freq)); return freq; }
LARGE_INTEGER frequency(getFrequency());
#endif

Time Time::now()
  {
#if defined(Q_OS_WIN) && !defined(X_ARCH_ARM)
  LARGE_INTEGER time;
  QueryPerformanceCounter(&time);

  xint64 secs = time.QuadPart / frequency.QuadPart;

  xuint64 elapsedNanoseconds = time.QuadPart - (secs * frequency.QuadPart);
  elapsedNanoseconds = (elapsedNanoseconds * SECOND_IN_NANO_SECONDS) / frequency.QuadPart;
  xAssert(elapsedNanoseconds < SECOND_IN_NANO_SECONDS);

  return Time(secs, elapsedNanoseconds);
#elif defined(X_ARCH_ARM)
  xAssertFail();
  return Time();
#else
  // find the current time from the system in floating point seconds
  struct timeval newTime;
  gettimeofday(&newTime, 0);
  xAssert(newTime.tv_usec < 1000000);
  return Time(newTime.tv_sec, 1000 * newTime.tv_usec);
#endif
  }

Time::Time() : _secs(0), _nanosecs(0)
  {
  }

Time::Time(const Time &t) : _secs(t._secs), _nanosecs(t._nanosecs)
  {
  }

Time &Time::operator+=(const Time &t)
  {
  _secs += t._secs;
  _nanosecs += t._nanosecs;

  _secs += _nanosecs / SECOND_IN_NANO_SECONDS;
  _nanosecs = _nanosecs % SECOND_IN_NANO_SECONDS;

  return *this;
  }

Time Time::operator+(const Time &t) const
  {
  xint64 s, n;
  s = _secs + t._secs;
  n = _nanosecs + t._nanosecs;

  s += n / SECOND_IN_NANO_SECONDS;
  n = n % SECOND_IN_NANO_SECONDS;

  return Time(s, n);
  }

Time Time::operator-(const Time &t) const
  {
  xint64 secs = _secs - t._secs;
  xint64 nano = _nanosecs - t._nanosecs;
  if(nano < 0)
    {
    secs -= 1;
    nano += SECOND_IN_NANO_SECONDS;
    xAssert(nano > 0);
    }
  return Time(secs, nano);
  }

Time Time::operator/(double d) const
  {
  return *this * (1.0/d);
  }

Time Time::operator*(double d) const
  {
  double secs = (double)_secs * d;
  double nanosecs = (double)_nanosecs * d;

  xint64 nanosecsInt = (xint64)(nanosecs + (secs * SECOND_IN_NANO_SECONDS));
  nanosecsInt = nanosecsInt % SECOND_IN_NANO_SECONDS;

  return Time(secs, nanosecsInt);
  }

bool Time::operator<(const Time &t) const
  {
  if(_secs < t._secs)
    {
    return true;
    }
  else if(_secs == t._secs)
    {
    return _nanosecs < t._nanosecs;
    }

  return false;
  }

bool Time::operator==(const Time &t) const
  {
  return _secs == t._secs && _nanosecs == t._nanosecs;
  }

bool Time::operator<=(const Time &t) const
  {
  return *this < t || *this == t;
  }

bool Time::operator>(const Time &t) const
  {
  return t < *this;
  }

bool Time::operator>=(const Time &t) const
  {
  return *this > t || *this == t;
  }

TimeStatistics::TimeStatistics() : _count(0)
  {
  }

void TimeStatistics::append(Time t)
  {
  if(_count == 0)
    {
    _minimum = t;
    _maximum = t;
    }
  else
    {
    if(t < _minimum)
      {
      _minimum = t;
      }
    if(t > _maximum)
      {
      _maximum = t;
      }
    }

  _total += t;
  ++_count;
  }

void TimeStatistics::append(const TimeStatistics &o)
  {
  if(count() == 0)
    {
    *this = o;
    return;
    }
  else if(o.count() == 0)
    {
    return;
    }

  _count += o.count();
  _total += o.total();

  if(o.minimum() < minimum())
    {
    _minimum = o.minimum();
    }
  if(o.maximum() > maximum())
    {
    _maximum = o.maximum();
    }
  }

void TimeStatistics::clear()
  {
  _count = 0;
  }

QDataStream &operator<<(QDataStream &str, const Time &l)
  {
  qint64 s, n;
  l.get(s, n);
  return str << s << n;
  }

QDataStream &operator>>(QDataStream &str, Time &l)
  {
  qint64 s, n;
  str >> s >> n;
  l.set(s, n);
  return str;
  }

}
