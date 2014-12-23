	#ifndef XSTRING_H
#define XSTRING_H

#include "XGlobal.h"
#include "XCore.h"
#include "Containers/XVector.h"

#if X_STL_INTEROP
# include <string>
# include <iostream>
#endif

namespace Eks
{

typedef char Char;

template <typename C, xsize P, typename A> class StringBuffer;

template <typename Character, xsize PreallocatedSize=0, typename Allocator=TypedAllocator<Character> >
    class StringBase : public Vector<Character, PreallocatedSize, TypedAllocator<Character> >
  {
public:
  typedef Character Char;
  typedef TypedAllocator<Char> Alloc;
  typedef std::char_traits<Char> Traits;
  typedef Vector<Char, PreallocatedSize, Alloc> String;
  typedef StringBase<Char, PreallocatedSize, Allocator> ThisType;
  typedef std::ostream OStream;
  typedef std::istream IStream;
  typedef StringBuffer<Char, PreallocatedSize, Allocator> Buffer;
  typedef typename String::size_type size_type;
  typedef typename String::iterator iterator;
  typedef typename String::const_iterator const_iterator;

  static const xsize npos = -1;

  StringBase(AllocatorBase *alloc=0)
      : String(Alloc(alloc))
    {
    }
  StringBase(const Allocator &alloc)
      : String(alloc)
    {
    }
  StringBase(const Char *n, AllocatorBase *alloc=0)
      : String(Alloc(alloc))
    {
    const size_type s = strlen(n);
    resizeAndCopy(s, n);
    xAssert(!String::size() || String::at(size()) == '\0');
    }
  StringBase(const Char *n, xsize s, AllocatorBase *alloc=0)
      : String(Alloc(alloc))
    {
    resizeAndCopy(s, n);
    xAssert(!String::size() || String::at(size()) == '\0');
    }
  StringBase(const String &n)
      : String(n, n)
    {
    xAssert(!String::size() || String::at(size()) == '\0');
    }

  StringBase(String &&n)
      : String(std::move(n))
    {
    xAssert(!String::size() || String::at(size()) == '\0');
    }

  StringBase(ThisType &&n)
      : String(std::move(n))
    {
    xAssert(!String::size() || String::at(size()) == '\0');
    }

  template <typename C, xsize S, typename A> StringBase(
      const StringBase<C, S, A>& n,
      AllocatorBase *alloc=0)
    : String(n, Alloc(alloc))
    {
    xAssert(!String::size() || String::at(size()) == '\0');
    }

#if X_STL_INTEROP
  StringBase(const std::string &s, AllocatorBase *alloc=Eks::Core::defaultAllocator())
    : String(alloc)
    {
    resizeAndCopy(s.length(), s.data());
    }
#endif

#if X_QT_INTEROP
  StringBase(const QString &s, AllocatorBase *alloc=Eks::Core::defaultAllocator())
    : String(alloc)
    {
    QByteArray arr = s.toUtf8();

    resizeAndCopy(arr.length(), arr.data());
    }

  QString toQString() const
    {
    return QString::fromUtf8(String::data(), (int)size());
    }

  ThisType& operator=(const QString& s)
    {
    clear();
    QByteArray arr = s.toUtf8();
    resizeAndCopy(arr.length(), arr.data());
    return *this;
    }

#endif

  ThisType& operator=(const ThisType& oth)
    {
    clear();
    append(oth);
    return *this;
    }
    
  ThisType& operator=(ThisType&& str)
    {
    String &ths = *this;
    String &oth = str;
    ths = std::move(oth);
    return *this;
    }

  ThisType& operator=(const Char* oth)
    {
    clear();
    append(oth);
    return *this;
    }

  using String::operator==;
  bool operator==(const Char *c) const
    {
    size_type s = size();
    return s == Traits::length(c) && String::compare(c, s);
    }

  using String::operator!=;
  bool operator!=(const Char *c) const
    {
    return !(*this == c);
    }

  bool operator<(const ThisType &oth) const
    {
    return std::lexicographical_compare(String::begin(), String::end(), oth.begin(), oth.end());
    }

  typename String::reverse_iterator rbegin()
    {
    return String::rbegin()+1;
    }

  void clear()
    {
    String::clear();
    append("");
    }

  size_type length() const
    {
    return size();
    }

  size_type size() const
    {
    size_type s = String::size();
    return s > 0 ? s - 1 : 0;
    }

  bool isEmpty() const
    {
    return size() == 0;
    }

  Char &back()
    {
    xAssert(end()-2);
    return *(end()-2);
    }

  const Char &back() const
    {
    xAssert(end()-2);
    return *(end()-2);
    }

  size_type capacity() const
    {
    return String::capacity() - 1;
    }

  iterator end()
    {
    return String::end() - 1;
    }

  const const_iterator end() const
    {
    return String::end() - 1;
    }

  const const_iterator cend() const
    {
    return String::cend() - 1;
    }

  void reserve(size_type newCapacity)
    {
    String::reserve(newCapacity+1);
    }

  template <typename T> void appendType(const T &n);
  void appendType(const String &n)
    {
    append(n);
    }
  void appendType(const Char *n)
    {
    append(n);
    }

  template <typename T> T toType(bool *error=0) const;

  template <typename Vec> void mid(const Vec &data, xsize pos, xsize len);

  void resize(size_type newSize, const Char &val)
    {
    // remove the \0
    if(String::size())
      {
      String::popBack();
      }
    // resize up
    String::resize(newSize+1, val);
    String::at(newSize) = '\0';
    }

  template <typename It>
  void resizeAndCopy(size_type newSize, It val)
    {
    // remove the \0
    if(String::size())
      {
      String::popBack();
      }

    // ensure theres enough space for the new string and its /0
    // note this calls the String::reserve which allows for trailing zero.
    reserve(newSize);

    // resize up, copying from val
    String::resizeAndCopy(newSize, val);

    // append a \0
    String::resize(newSize+1, '\0');
    }

  template <typename T>
  void append(const T &data)
    {
    if(String::size())
      {
      String::popBack();
      }
    String::append(data);
    String::pushBack('\0');
    xAssert(!String::size() || String::at(size()) == '\0');
    }
    
  void append(const Char *data)
    {
    if(String::size())
      {
      String::popBack();
      }
    String::resizeAndCopy(String::size()+strlen(data)+1, data);
    xAssert(String::at(size()) == '\0');
    }

  void append(Char data)
    {
    if(String::size())
      {
      String::popBack();
      }
    String::resize(String::size() + 2, data);
    String::at(String::size()-1) = '\0';
    xAssert(String::at(size()) == '\0');
    }

  template <typename Other>
  ThisType operator+(Other oth) const
    {
    ThisType res(String::allocator());

    res.append(*this);
    res.append(oth);
    return res;
    }

  ThisType& operator+=(const Char *data)
    {
    append(data);
    return *this;
    }

  void popBack()
    {
    resize(size() - 1, ' ');
    }

  void pushBack(Char c)
    {
    const Char arr[] = { c, '\0' };
    append(arr);
    }

  template <xsize P, typename A>
  String& operator+=(const StringBase<Char, P, A> &t)
    {
    append(t);
    return *this;
    }

  String& operator+=(const String &t)
    {
    append(t);
    return *this;
    }

  String& operator+=(Char t)
    {
    append(t);
    return *this;
    }

  String& operator<<(Char t)
    {
    append(t);
    return *this;
    }

  String& operator<<(const Char *t)
    {
    append(t);
    return *this;
    }

  ThisType& operator<<(const ThisType &t)
    {
    append(t);
    return *this;
    }

  struct Replacement
    {
    const Char *source;
    const Char *dest;
    };

  using String::replace;
  template <typename StringType>
  static void replace(const StringType &old, String *dest, const Replacement *rep, xsize replacementCount = 1)
    {
    // store as vector to avoid all the \0 copying;
    String *destVec = dest;

    destVec->reserve(old.size());
    destVec->clear();
    for(xsize i = 0, s = old.size(); i < s; ++i)
      {
      // see if any replacements match.
      bool found = false;
      for(xsize repIdx = 0; repIdx < replacementCount; ++repIdx)
        {
        const Replacement &replacement = rep[repIdx];
        bool match = true;

        xsize matchLength = Traits::length(replacement.source);
        for(size_type matchIdx = 0; matchIdx < matchLength; ++matchIdx)
          {
          if(old[i+matchIdx] != replacement.source[matchIdx])
            {
            match = false;
            break;
            }
          }

        if(match)
          {
          i = i + matchLength;
          found = true;

          destVec->append(replacement.dest, Traits::length(replacement.dest));
          }
        }

      if(!found)
        {
        destVec->pushBack(old[i]);
        }
      }

    destVec->pushBack('\0');
    }

  xsize find(const Char *sequence, xsize from = npos) const
    {
    xAssert(*sequence != '\0');
    if (from == npos)
      {
      from = 0;
      }
    xAssert(from <= size());

    xsize location = from;
    xsize sequenceLen = 0;
    const Char *current = sequence;
    while(true)
      {
      if (String::data()[location] == *current)
        {
        ++current;

        if (*current == '\0')
          {
          return location - sequenceLen;
          }
        ++sequenceLen;
        }
      else
        {
        current = sequence;
        }

      if (location == size())
        {
        return npos;
        }
      ++location;
      }
    }

  xsize rfind(const Char *sequence, xsize from = npos) const
    {
    xAssert(*sequence != '\0');
    const Char *last = sequence + strlen(sequence) - 1;
    if (from == npos)
      {
      from = size()-1;
      }
    xAssert(from < size());

    xsize location = from;
    const Char *current = last;
    while(true)
      {
      if (String::data()[location] == *current)
        {
        if (current == sequence)
          {
          return location;
          }
        --current;
        }
      else
        {
        current = last;
        }

      if (location == 0)
        {
        return npos;
        }
      --location;
      }
    }
  };

template <typename Char, xsize PreallocatedSize, typename Allocator>
  template <typename Vec>
    inline void StringBase<Char, PreallocatedSize, Allocator>::mid(const Vec &data, xsize pos, xsize len)
  {
  xAssert(pos < data.size());
  xAssert(pos+len < data.size());

  clear();

  const char* start = (data.data() + pos);

  resizeAndCopy(len, start);
  }

class String : public StringBase<Char>
  {
public:
  typedef StringBase<Char> BaseType;

  explicit String(AllocatorBase *alloc=Core::defaultAllocator())
      : BaseType(alloc)
    {
    }
  String(const Char *n, AllocatorBase *alloc=Core::defaultAllocator())
      : BaseType(n, alloc)
    {
    }
  String(const Char *n, xsize s, AllocatorBase *alloc=Core::defaultAllocator())
      : BaseType(n, s, alloc)
    {
    }
  template<xsize Prealloc, typename Alloc>
  String(const StringBase<Char, Prealloc, Alloc> &n)
      : BaseType(n, n.allocator().allocator() ? n.allocator().allocator() : Core::defaultAllocator())
    {
    }
  template<xsize Prealloc, typename Alloc>
  String(const StringBase<Char, Prealloc, Alloc> &n, AllocatorBase *alloc)
      : BaseType(n, alloc)
    {
    }
  String(const String &n)
      : BaseType(n, n.allocator().allocator() ? n.allocator().allocator() : Core::defaultAllocator())
    {
    }
  String(String &&n)
      : BaseType(std::move(n))
    {
    }
  String(String &&n, AllocatorBase *alloc)
      : BaseType(std::move(n), alloc)
    {
    }

#if X_STL_INTEROP
  String(const std::string &s, AllocatorBase *alloc=Core::defaultAllocator())
    : BaseType(alloc)
    {
    resizeAndCopy(s.length(), s.data());
    }
#endif

#if X_QT_INTEROP
  String(const QString &s, AllocatorBase *alloc=Core::defaultAllocator())
    : BaseType(alloc)
    {
    QByteArray arr = s.toUtf8();

    resizeAndCopy(arr.length(), arr.data());
    }
#endif

  String &operator=(const String &str)
    {
    clear();
    append(str);
    return *this;
    }
    
  String &operator=(String &&str)
    {
    BaseType &ths = *this;
    BaseType &oth = str;
    ths = std::move(oth);
    return *this;
    }
  };

EKSCORE_EXPORT size_t hash(const xuint8 *data, size_t length);

}

#if X_QT_INTEROP

#include "QHash"

template <typename C, xsize P, typename A> uint qHash(const Eks::StringBase<C, P, A> &a)
  {
  return qHash(QLatin1String(a.data(), a.length()));
  }

EKSCORE_EXPORT QDataStream &operator>>(QDataStream &str, Eks::Char &vec);
EKSCORE_EXPORT QDataStream &operator<<(QDataStream &str, Eks::Char vec);

Q_DECLARE_METATYPE(Eks::String);

#include "QtCore/QDebug"

template <typename C, xsize P, typename A> QDebug operator<<(
    QDebug dbg,
    const Eks::StringBase<C, P, A> &str)
  {
  return dbg << str.data();
  }

#endif

template <typename T, xsize S, typename A>
    std::istream &operator>>(std::istream &str, Eks::StringBase<T, S, A> &vec)
  {
  while(!str.eof())
    {
    T t;
    str >> t;
    vec << t;
    }
  return str;
  }

template <typename C, xsize P, typename A> std::ostream &operator<<(
    std::ostream &dbg,
    const Eks::StringBase<C, P, A> &str)
  {
  if (!str.size())
    {
    return dbg;
    }

  auto end = str.end();
  for (auto i = str.begin(); i != end; ++i)
    {
    dbg << *i;
    }
  return dbg;
  }

namespace std
{
template <typename T, xsize S, typename A> struct hash<Eks::StringBase<T, S, A>>
  {
public:
  size_t operator()(const Eks::StringBase<T, S, A> &vec) const
    {
    return Eks::hash((const xuint8*)vec.data(), sizeof(T)*vec.size());
    }
  };

template <> struct hash<Eks::String> : hash<Eks::StringBase<Eks::String::Char>> { };
}



#endif // XSTRING_H
