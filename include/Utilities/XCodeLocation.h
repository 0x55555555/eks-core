#ifndef XCODELOCATION_H
#define XCODELOCATION_H

#include "XGlobal.h"
#include "Utilities/XProperty.h"
#include "Utilities/XMacroHelpers.h"
#include <functional>
#include <utility>

namespace Eks
{

class String;

class EKSCORE_EXPORT CodeLocation
  {
XProperties:
  XROProperty(const char *, file);
  XROProperty(xuint32, line);
  XROProperty(const char *, function);

public:
  CodeLocation(const char *file="", xuint32 line=0, const char *function="");
  CodeLocation(const CodeLocation &l);

  String toString() const;

  bool operator==(const CodeLocation &a) const;
  };



class EKSCORE_EXPORT StackWalker
  {
public:
  class Visitor
    {
  public:
    virtual void visit(xsize level, void *symbol) = 0;
    };

  static void getSymbolName(void *symbol, Eks::String &symbolName, xsize maxSize);

  static void intialiseSymbolNames();
  static void terminateSymbolNames();
  static void walk(xsize skip, Visitor *visit);
  };

}

#define X_CURRENT_CODE_LOCATION Eks::CodeLocation(__FILE__, __LINE__, xCurrentFunction)


namespace std
{
template <> struct hash<Eks::CodeLocation>
  {
public:
  size_t operator()(const Eks::CodeLocation &a) const
    {
    std::hash<const char*> hasher;
    std::hash<xuint32> hasherInt;

    return hasher(a.file()) ^ hasher(a.function()) ^ hasherInt(a.line());
    }

  };
}

#endif // XCODELOCATION_H
