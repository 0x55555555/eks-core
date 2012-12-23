#include "XCodeLocation"
#include "XStringSimple"
#include "XStringBuffer"
#include "QHash"

namespace Eks
{

CodeLocation::CodeLocation(const char *file, xsize line, const char *function)
    : _file(file), _line(line), _function(function)
  {
  }

String CodeLocation::toString() const
  {
  String out;

  out += file();
  out += " ";
  out.appendType(line());
  out += " ";
  out += function();

  return out;
  }

bool CodeLocation::operator==(const CodeLocation &a) const
  {
  return line() == a.line() &&
    strcmp(file(), a.file()) == 0 &&
    strcmp(function(), a.function()) == 0;
  }

}

uint qHash(const Eks::CodeLocation &a)
  {
  uint hash = qHash(QPair<const char *, const char *>(a.file(), a.function()));

  return qHash(QPair<xsize, uint>(hash, a.line()));
  }
