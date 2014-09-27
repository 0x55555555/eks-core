#include "Utilities/XParseException.h"
#include "Containers/XStringBuffer.h"

namespace Eks
{

ParseError::ParseError(
    const DetailedCodeLocation& codeLocation,
    ContextType t,
    const String &lineText,
    xsize line,
    xsize chr,
    const String &msg)
  : _location(codeLocation),
    _msg(msg),
    _hasLocation(true),
    _type(t),
    _context(lineText),
    _line(line),
    _chr(chr)
  {
  format();
  }

ParseError::ParseError(
    const DetailedCodeLocation& codeLocation,
    ContextType t,
    const String &context,
    xsize line,
    const String &msg)
  : _location(codeLocation),
    _msg(msg),
    _hasLocation(true),
    _type(t),
    _context(context),
    _line(line),
    _chr(Eks::maxFor<xsize>())
  {
  format();
  }

ParseError::ParseError(
    const DetailedCodeLocation &codeLocation,
    const String &msg)
  : _location(codeLocation),
    _msg(msg),
    _hasLocation(false),
    _type(NoContext),
    _line(Eks::maxFor<xsize>()),
    _chr(Eks::maxFor<xsize>())
  {
  format();
  }

Eks::String ParseError::fileLocation() const
  {
  if (!_hasLocation)
    {
    return Eks::String();
    }

  xsize chr = _chr;
  Eks::StringBuilder f;
  if (_line != Eks::maxFor<xsize>())
    {
    f << "Line ";
    f << _line;
    f << ": ";

    chr += f.length();
    }

  if (_context.length())
    {
    if (_type == LineContext)
      {
      f << _context;
      }
    else if (_type == FullContext)
      {
      const char *c = _context.data();
      for (xsize i = _line; i > 1; --i)
        {
        c = strchr(c, '\n');
        if (!c)
          {
          c = _context.data();
          }
        ++c;
        }

      while (*c != '\0' && *c != '\n')
        {
        f << *c++;
        }
      }
    }

  if (_chr != Eks::maxFor<xsize>())
    {
    f << "\n";
    xsize s = std::max(2UL, chr);
    for (size_t i = 0; i < (s-1); ++i)
      {
      f << "-";
      }
    f << "^";
    }
  return f;
  }

void ParseError::format()
  {
  _stringError = fileLocation();
  if (_stringError.length())
    {
    _stringError += "\n";
    }

  _stringError = _msg;
  }

ParseException::ParseException(const ParseError &codeLocation)
  : ParseError(codeLocation)
  {
  }

const char *ParseException::what() const noexcept
  {
  return error().data();
  }

}
