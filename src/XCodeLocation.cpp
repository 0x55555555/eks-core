#include "Utilities/XCodeLocation.h"
#include "Containers/XStringSimple.h"
#include "Containers/XStringBuffer.h"

#if defined(X_DEBUG) && !defined(X_ARCH_ARM)
# define X_ENABLE_STACK_WALKING 1
#else
# define X_ENABLE_STACK_WALKING 0
#endif

#if defined(X_WIN) && X_ENABLE_STACK_WALKING
# include <Windows.h>
# include <DbgHelp.h>
#endif

#if defined (X_OSX) && X_ENABLE_STACK_WALKING
#include <execinfo.h>
#endif

namespace Eks
{

CodeLocation::CodeLocation(const char *file, xuint32 line, const char *function)
    : _file(file), _line(line), _function(function)
  {
  }

CodeLocation::CodeLocation(const CodeLocation &l)
  : _file(l._file), _line(l._line), _function(l._function)
  {
  }

String CodeLocation::toString() const
  {
  String out;

  out += file();
  out += ":";
  out.appendType(line());
  out += " - ";
  out += function();

  return out;
  }

bool CodeLocation::operator==(const CodeLocation &a) const
  {
  return line() == a.line() &&
    strcmp(file(), a.file()) == 0 &&
    strcmp(function(), a.function()) == 0;
  }

void StackWalker::intialiseSymbolNames()
  {
#if X_ENABLE_STACK_WALKING
#ifdef Q_CC_MSVC
  SymInitialize(::GetCurrentProcess(), 0, true);
#endif
#endif
  }

void StackWalker::terminateSymbolNames()
  {
#if X_ENABLE_STACK_WALKING
#ifdef Q_CC_MSVC
  SymCleanup(::GetCurrentProcess());
#endif
#endif
  }

void StackWalker::getSymbolName(
    void *symbolLocation,
    Eks::String &symbolName,
    xsize maxSize)
  {
#if X_ENABLE_STACK_WALKING
#ifdef Q_CC_MSVC
  BYTE *buffer = (BYTE *)alloca(sizeof(SYMBOL_INFO) + maxSize);
  memset(buffer, 0, sizeof(buffer));

  PSYMBOL_INFO symbol = (PSYMBOL_INFO)buffer;
  symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
  symbol->MaxNameLen = (ULONG)maxSize;

  DWORD64 symDisplacement = 0;
  if (!SymFromAddr(::GetCurrentProcess(),
                   reinterpret_cast<xsize>(symbolLocation),
                   &symDisplacement,
                   symbol))
    {
    symbol->Name[0] = 0;
    symDisplacement = 0;
    }

  symbolName.clear();
  symbolName.resizeAndCopy(strlen(symbol->Name), symbol->Name);

#elif defined(X_OSX)
  std::unique_ptr<char *, void (*)(void *)> strs(backtrace_symbols(&symbolLocation, 1), std::free);

  if (strs && strs.get()[0])
    {
    symbolName = strs.get()[0];
    if (symbolName.length() > maxSize)
      {
      symbolName.resize(maxSize, '\0');
      }
    }

#else
  (void)symbolLocation;
  (void)symbolName;
  (void)maxSize;
#endif
#else
#endif
  }

void StackWalker::walk(
    xsize skip,
    Visitor *visit)
  {
#if X_ENABLE_STACK_WALKING
  struct Utils
    {
    static void doVisit(Visitor *visit, xsize i, void *pcOffset)
      {
      visit->visit(i, pcOffset);
      }
    };

  enum
    {
    BlockCaptureSize = 16,
    MaxSymbols = 128,
    };

#if defined(X_WIN)

  xsize position = skip + 1;
  const xsize start = position;
  void *frames[BlockCaptureSize];

  xuint16 captured = 0;
  do
    {
    captured = CaptureStackBackTrace((DWORD)position, BlockCaptureSize, frames, 0);

    for(xsize i = 0; i < captured; ++i)
      {
      Utils::doVisit(visit, position + i - start, frames[i]);
      }

    position += captured;
    } while(captured == BlockCaptureSize);

#elif defined(X_OSX)

  void *frames[MaxSymbols];

  int captured = backtrace(frames, MaxSymbols);
  if (captured <= 0 || (xsize)captured > X_ARRAY_COUNT(frames))
    {
    return;
    }

  for(xsize i = 0; i < (xsize)captured; ++i)
    {
    Utils::doVisit(visit, i, frames[skip+i]);
    }

#else
  (void)skip;
  (void)visit;
#endif
#endif
  }
}




