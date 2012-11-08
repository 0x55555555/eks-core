#define builtIn
#include "XTime"
#include "QDebug"
#include "XGlobal"
#include "XAssert"

#if defined(Q_OS_WIN)
# include <windows.h>
#else
# include <sys/time.h>
#endif

#define SECOND_IN_NANO_SECONDS 1000000000

XTime::XTime(xint64 secs, xint64 nanosecs) : _secs(secs), _nanosecs(nanosecs)
  {
  }

XTime XTime::fromMilliseconds(double ms)
  {
  xint64 seconds = ms / 1000.0;
  xint64 nanoseconds = ms * 1000000.0;
  nanoseconds -= seconds * SECOND_IN_NANO_SECONDS;
  xAssert(nanoseconds > 0);
  return XTime(seconds, nanoseconds);
  }

void XTime::beginAccurateTiming()
  {
#if defined(Q_OS_WIN) && !defined(X_ARCH_ARM)
  HANDLE h = GetCurrentProcess();
  SetProcessAffinityMask(h, 1);
#endif
  }

#if defined(Q_OS_WIN) && !defined(X_ARCH_ARM)
inline LARGE_INTEGER getFrequency() { LARGE_INTEGER freq; xVerify(QueryPerformanceFrequency(&freq)); return freq; }
LARGE_INTEGER frequency(getFrequency());
#endif

XTime XTime::now()
  {
#if defined(Q_OS_WIN) && !defined(X_ARCH_ARM)
  LARGE_INTEGER time;
  QueryPerformanceCounter(&time);

  xint64 secs = time.QuadPart / frequency.QuadPart;

  xuint64 elapsedNanoseconds = time.QuadPart - (secs * frequency.QuadPart);
  elapsedNanoseconds = (elapsedNanoseconds * SECOND_IN_NANO_SECONDS) / frequency.QuadPart;
  xAssert(elapsedNanoseconds < SECOND_IN_NANO_SECONDS);

  return XTime(secs, elapsedNanoseconds);
#elif defined(X_ARCH_ARM)
  xAssertFail();
  return XTime();
#else
  // find the current time from the system in floating point seconds
  struct timeval newTime;
  gettimeofday(&newTime, 0);
  xAssert(newTime.tv_usec < 1000000);
  return XTime(newTime.tv_sec, 1000 * newTime.tv_usec);
#endif
  }

XTime::XTime() : _secs(0), _nanosecs(0)
  {
  }

XTime::XTime(const XTime &t) : _secs(t._secs), _nanosecs(t._nanosecs)
  {
  }

XTime &XTime::operator+=(const XTime &t)
  {
  _secs += t._secs;
  _nanosecs += t._nanosecs;

  _secs += _nanosecs / SECOND_IN_NANO_SECONDS;
  _nanosecs = _nanosecs % SECOND_IN_NANO_SECONDS;

  return *this;
  }

XTime XTime::operator+(const XTime &t)
  {
  xint64 s, n;
  s = _secs + t._secs;
  n = _nanosecs + t._nanosecs;

  s += n / SECOND_IN_NANO_SECONDS;
  n = n % SECOND_IN_NANO_SECONDS;

  return XTime(s, n);
  }

XTime XTime::operator-(const XTime &t) const
  {
  xint64 secs = _secs - t._secs;
  xint64 nano = _nanosecs - t._nanosecs;
  if(nano < 0)
    {
    secs -= 1;
    nano += SECOND_IN_NANO_SECONDS;
    xAssert(nano > 0);
    }
  return XTime(secs, nano);
  }

XTime XTime::operator/(double d) const
  {
  return *this * (1.0/d);
  }

XTime XTime::operator*(double d) const
  {
  double secs = (double)_secs * d;
  double nanosecs = (double)_nanosecs * d;

  xint64 nanosecsInt = (xint64)(nanosecs + (secs * SECOND_IN_NANO_SECONDS));
  nanosecsInt = nanosecsInt % SECOND_IN_NANO_SECONDS;

  return XTime(secs, nanosecsInt);
  }

bool XTime::operator<(const XTime &t) const
  {
  if(_secs < t._secs)
    {
    return true;
    }
  return _nanosecs < t._nanosecs;
  }
bool XTime::operator>(const XTime &t) const
  {
  if(_secs > t._secs)
    {
    return true;
    }
  return _nanosecs > t._nanosecs;
  }

XTimeStatistics::XTimeStatistics() : _count(0)
  {
  }

void XTimeStatistics::append(XTime t)
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

void XTimeStatistics::append(const XTimeStatistics &o)
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

void XTimeStatistics::clear()
  {
  _count = 0;
  }
