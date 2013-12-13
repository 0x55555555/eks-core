#ifndef XSTRINGBUFFER_H
#define XSTRINGBUFFER_H

#include "Containers/XStringSimple.h"
#include "Utilities/XAssert.h"
#include <iostream>

namespace Eks
{

template <typename Char, xsize PreallocSize, typename Allocator>
    class StringBuffer : public std::streambuf
  {
public:
  typedef StringBase<Char, PreallocSize, Allocator> StringType;

  StringBuffer(StringType *str)
      : _str(str), _readPos(0), _writePos(0)
    {
    }

  StringBuffer(const StringType *str)
      : _str(const_cast<StringType *>(str)), _readPos(0), _writePos(X_SIZE_SENTINEL)
    {
    }

  void imbue(const std::locale &) X_OVERRIDE
    {
    xAssertFail();
    }

  std::streambuf *setbuf(char *, std::streamsize) X_OVERRIDE
    {
    xAssert(_writePos != X_SIZE_SENTINEL);
    xAssertFail();
    return this;
    }

  std::streampos seekoff(
      std::streamoff off,
      std::ios_base::seekdir way,
      std::ios_base::openmode which) X_OVERRIDE
    {
    xsize &pos = which == std::ios_base::in ? _readPos : _writePos;

    if(way == std::ios_base::beg)
      {
      pos = off;
      }
    else if(way == std::ios_base::cur)
      {
      pos += off;
      }
    else if(way == std::ios_base::end)
      {
      pos = _str->size() + off;
      }

    return pos;
    }

  std::streampos seekpos(
      std::streampos sp,
      std::ios_base::openmode which) X_OVERRIDE
    {
    return seekoff(sp, std::ios_base::beg, which);
    }

  std::streamsize xsgetn(Char *s, std::streamsize n ) X_OVERRIDE
    {
    std::streamsize num = xMin(_str->size(), (String::size_type)n);
    memcpy(s, _str->data(), num);
    return num;
    }

  int underflow() X_OVERRIDE
    {
    if(_readPos < _str->size())
      {
      return _str->at(_readPos);
      }
    return String::Traits::eof();
    }

  int uflow() X_OVERRIDE
    {
    int val = underflow();
    ++_readPos;
    return val;
    }

  int pbackfail(int c) X_OVERRIDE
    {
    (void)c;
    xAssert(c == String::Traits::eof());

    if(_readPos >= 1)
      {
      --_readPos;
      return 1;
      }
    return String::Traits::eof();
    }

  std::streamsize xsputn(const Char *s, std::streamsize n) X_OVERRIDE
    {
    xAssert(_writePos != X_SIZE_SENTINEL);
    _str->resize(_writePos, ' ');
    _writePos += n;
    _str->resizeAndCopy(_writePos, s);
    return _writePos;
    }

  int overflow(int c)
    {
    xAssert(_writePos != X_SIZE_SENTINEL);
    xsize writePoint = _writePos;

    xsize newSize = xMax(++_writePos, _str->size());
    _str->resize(newSize, c);

    _str->at(writePoint) = (String::Char)c;
    return 1;
    }

private:
  StringType *_str;
  xsize _readPos;
  xsize _writePos;
  };

template <typename Char, xsize PreallocatedSize, typename Allocator>
    template <typename T> void StringBase<Char, PreallocatedSize, Allocator>::appendType(T n)
  {
  Buffer buf(this);
  OStream str(&buf);

  str.seekp(0, std::ios_base::end);
  str.setf(std::ios_base::fixed);

  str << n;
  }

template <typename Char, xsize PreallocatedSize, typename Allocator>
    template <typename T> T StringBase<Char, PreallocatedSize, Allocator>::toType(bool *error) const
  {
  T out;
  Buffer buf(this);
  IStream stream(&buf);

  stream >> out;
  if(error)
    {
    *error = !stream.fail();
    }
  return out;
  }

}

#endif // XSTRINGBUFFER_H
