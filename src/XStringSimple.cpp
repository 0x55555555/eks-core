#include "XStringSimple"
#include "QtCore/QHash"

QDataStream &operator>>(QDataStream &str, Eks::Char &vec)
  {
  xCompileTimeAssert(sizeof(Eks::Char) == sizeof(xuint8));

  xuint8 out;
  str >> out;
  vec = out;
  return str;
  }

QDataStream &operator<<(QDataStream &str, Eks::Char vec)
  {
  xCompileTimeAssert(sizeof(Eks::Char) == sizeof(xuint8));
  str << (xuint8)vec;
  return str;
  }
