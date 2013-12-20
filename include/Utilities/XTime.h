#ifndef XTIMER_H
#define XTIMER_H

#include "Utilities/XProperty.h"
#include "XGlobal.h"
#include "QtCore/QMetaType"

namespace Eks
{

class EKSCORE_EXPORT Time
  {
public:
  Time();
  Time(const Time &t);
  static Time now();

  double seconds() const { return (double)_secs + ((double)_nanosecs / 1000000000.0); }
  double milliseconds() const { return ((double)_secs * 1000.0) + ((double)_nanosecs / 1000000.0); }
  double microseconds() const { return ((double)_secs * 1000000.0) + ((double)_nanosecs / 1000.0); }
  double nanoseconds() const { return ((double)_secs * 1000000000.0) + (double)_nanosecs; }

  Time &operator+=(const Time &t);
  Time operator+(const Time &t) const;

  Time operator-(const Time &t) const;
  Time operator/(double d) const;
  Time operator*(double d) const;


  bool operator==(const Time &t) const;
  bool operator<(const Time &t) const;
  bool operator<=(const Time &t) const;
  bool operator>(const Time &t) const;
  bool operator>=(const Time &t) const;

  static Time fromMilliseconds(double ms);

  static void beginAccurateTiming();

  void set(xint64 seconds, xint64 nanosecs);
  void get(xint64 &seconds, xint64 &nanosecs) const { seconds = _secs; nanosecs = _nanosecs; }

private:
  Time(xint64, xint64);
  xint64 _secs;
  xint64 _nanosecs;
  };

class EKSCORE_EXPORT TimeStatistics
  {
XProperties:
  XRORefProperty(Time, minimum);
  XRORefProperty(Time, maximum);
  XRORefProperty(Time, total);
  XRORefProperty(xsize, count);
public:
  TimeStatistics();

  Time average() const { if(count() == 0) { return Time(); } return total() / (double)count(); }

  void append(Time);
  void append(const TimeStatistics &);
  void clear();
  };

#if X_QT_INTEROP

EKSCORE_EXPORT QDataStream &operator<<(QDataStream &s, const Time &l);
EKSCORE_EXPORT QDataStream &operator>>(QDataStream &s, Time &l);

#endif

}

#if X_QT_INTEROP
Q_DECLARE_METATYPE(Eks::Time)
#endif

#endif // XTIMER_H
