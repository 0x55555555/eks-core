#include "Utilities/XParseException.h"
#include "Containers/XStringBuffer.h"

namespace Eks
{

ParseException::ParseException(const DetailedCodeLocation &codeLocation, const String &lineText, xsize line, xsize chr, const String &msg)
  : _location(codeLocation),
    _msg(msg),
    _hasLocation(true),
    _lineText(lineText),
    _line(line),
    _chr(chr)
  {
  format();
  }

ParseException::ParseException(const DetailedCodeLocation &codeLocation, const String &msg)
  : _location(codeLocation),
    _msg(msg),
    _hasLocation(false),
    _line(Eks::maxFor<xsize>()),
    _chr(Eks::maxFor<xsize>())
  {
  format();
  }

const char *ParseException::what() const noexcept
  {
  return _stringError.data();
  }

Eks::String ParseException::fileLocation() const
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

  if (_lineText.length())
    {
    f << _lineText;
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

void ParseException::format()
  {
  _stringError = fileLocation();
  if (_stringError.length())
    {
    _stringError += "\n";
    }

  _stringError = _msg;
  }


}
