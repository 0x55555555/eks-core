#include "XEventLogger"
#include "QDebug"

void XEventManager::totalAvailableTime(XTime &min, XTime &max)
  {
  min = XTime();
  max = XTime();
  bool init = false;
  XEventLoggerInternal *e = firstEvent();
  while(e)
    {
    if(e->used() > 0)
      {
      const void *data = e->at(0);
      xAssert(data);
      const XTime *t = (const XTime *)data;

      if(*t < min || init == false)
        {
        min = *t;
        }

      data = e->last();
      xAssert(data);
      t = (XTime *)data;

      if(*t > max || init == false)
        {
        max = *t;
        }

      if(init == false)
        {
        init = true;
        }
      }

    e = e->next;
    }
  }

XEventLoggerInternal *g_firstEvent = 0;
XEventLoggerInternal *XEventManager::firstEvent()
  {
  return g_firstEvent;
  }

void XEventManager::addEvent(XEventLoggerInternal *e)
  {
  xAssert(e->next == 0); // double add?
  e->next = g_firstEvent;
  g_firstEvent = e;
  }

XEventLoggerInternal::XEventLoggerInternal(const char *n, int t, xsize s, xsize c, const char *units)
    : name(n), units(units), type(t), size(s), count(c), next(0), firstData(0), nextData(data)
  {
  XEventManager::addEvent(this);
  }


xsize XEventLoggerInternal::used() const
  {
  if(firstData == 0)
    {
    return 0;
    }

  if(nextData == firstData)
    {
    return count;
    }

  return (nextData - firstData) / size;
  }

const void *XEventLoggerInternal::at(xsize index) const
  {
  xAssert(firstData);
  xuint8 *d = firstData + (index * size);

  const xsize s = (count * size);
  if(d > (data + s))
    {
    d -= s;
    }

  return d;
  }

const void *XEventLoggerInternal::dataAt(xsize index) const
  {
  const void *d = at(index);
  if(d)
    {
    d = (xuint8*)d + sizeof(XTime);
    }

  return d;
  }

const void *XEventLoggerInternal::last() const
  {
  if(firstData == 0)
    {
    return 0;
    }

  xuint8 *p = nextData - size;
  if(p < data)
    {
    p += (used() * size);
    }

  return p;
  }

void *XEventLoggerInternal::last()
  {
  if(firstData == 0)
    {
    return 0;
    }

  xuint8 *p = nextData - size;
  if(p < data)
    {
    p += (used() * size);
    }

  return p;
  }

void XEventLoggerInternal::add()
  {
  bool syncFirst = nextData == firstData;

  if(!firstData)
    {
    firstData = data;
    }

  nextData += size;
  if(nextData > (data + count * size))
    {
    nextData = data;
    }

  if(syncFirst)
    {
    firstData = nextData;
    }
  }
