#ifndef XPARSEEXCEPTION_H
#define XPARSEEXCEPTION_H

#include <stdexcept>
#include "Containers/XStringSimple.h"
#include "Math/XMathHelpers.h"

namespace Eks
{

class DetailedCodeLocation;

class ParseError
  {
public:
  enum ContextType
    {
    LineContext,
    FullContext,
    NoContext,
    };

  ParseError(
    const DetailedCodeLocation& codeLocation,
    ContextType t,
    const Eks::String &context,
    xsize line,
    xsize chr,
    const Eks::String &msg);
  ParseError(
    const DetailedCodeLocation& codeLocation,
    ContextType t,
    const Eks::String &context,
    xsize line,
    const Eks::String &msg);
  ParseError(
    const DetailedCodeLocation& codeLocation,
    const Eks::String &msg);

  const DetailedCodeLocation &location() const { return _location; }
  Eks::String fileLocation() const;
  const Eks::String &message() const { return _msg; }

  const Eks::String &error() const { return _stringError; }

private:
  void format();

  DetailedCodeLocation _location;
  Eks::String _msg;

  bool _hasLocation;
  ContextType _type;
  Eks::String _context;
  xsize _line;
  xsize _chr;

  Eks::String _stringError;
  };

class EKSCORE_EXPORT ParseException : public std::exception, public ParseError
  {
public:
  ParseException(const ParseError &err);

  const char* what() const noexcept;
  };

class EKSCORE_EXPORT ParseErrorInterface
  {
public:
  virtual void error(const ParseError &) = 0;
  virtual void warning(const ParseError &) = 0;
  };

#define X_PARSE_ERROR(...) Eks::ParseError(X_CURRENT_CODE_LOCATION_DETAILED, __VA_ARGS__)

}

#endif // XPARSEEXCEPTION_H
