#include "XCodeLocation"
#include "QHash"

XCodeLocation::XCodeLocation(const char *file, xsize line, const char *function)
    : _file(file), _line(line), _function(function)
  {
  }

QString XCodeLocation::toString() const
  {
  QString out;

  out += file();
  out += " ";
  out += QString::number(line());
  out += " ";
  out += function();

  return out;
  }

bool XCodeLocation::operator==(const XCodeLocation &a) const
  {
  return line() == a.line() &&
    qstrcmp(file(), a.file()) == 0 &&
    qstrcmp(function(), a.function()) == 0;
  }

uint qHash(const XCodeLocation &a)
  {
  uint hash = qHash(QPair<const char *, const char *>(a.file(), a.function()));

  return qHash(QPair<xsize, uint>(hash, a.line()));
  }
