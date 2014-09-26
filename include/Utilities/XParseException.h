#ifndef XPARSEEXCEPTION_H
#define XPARSEEXCEPTION_H

#include <stdexcept>
#include "Containers/XStringSimple.h"
#include "Math/XMathHelpers.h"

namespace Eks
{

class DetailedCodeLocation;

class EKSCORE_EXPORT ParseException : std::exception
  {
public:
  ParseException(const DetailedCodeLocation& codeLocation, const Eks::String &lineText, xsize line, xsize chr, const Eks::String &msg);
  ParseException(const DetailedCodeLocation& codeLocation, const Eks::String &msg);

  const char* what() const noexcept;

  const DetailedCodeLocation &location() const { return _location; }
  Eks::String fileLocation() const;
  const Eks::String &message() const { return _msg; }

private:
  void format();

  DetailedCodeLocation _location;
  Eks::String _msg;

  bool _hasLocation;
  Eks::String _lineText;
  xsize _line;
  xsize _chr;

  Eks::String _stringError;
  };

#define X_PARSE_ERROR(...) ParseException(X_CURRENT_CODE_LOCATION_DETAILED, __VA_ARGS__)

}

#endif // XPARSEEXCEPTION_H
