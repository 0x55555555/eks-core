#include "XCodeLocation"
#include "XStringSimple"
#include "XStringBuffer"
#include "QHash"
#include "QDebug"

#if defined(Q_CC_MSVC) && X_ENABLE_STACK_WALKING
# include <Windows.h>
# include <DbgHelp.h>
#endif

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

void StackWalker::intialise()
  {
#if X_ENABLE_STACK_WALKING
#ifdef Q_CC_MSVC
  SymInitialize(::GetCurrentProcess(), 0, true);
#endif
#endif
  }

void StackWalker::walk(xsize skip, Visitor *visit)
  {
#if X_ENABLE_STACK_WALKING
#ifdef Q_CC_MSVC
  struct Utils
    {
    static void doVisit(Visitor *visit, xsize i, void *pcOffset, xsize frameOffset)
      {
      static const size_t MAX_NAME_LEN = 1024;
      BYTE buffer[sizeof(SYMBOL_INFO) + MAX_NAME_LEN];
      memset(buffer, 0, sizeof(buffer));

      PSYMBOL_INFO symbol = (PSYMBOL_INFO)buffer;
      symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
      symbol->MaxNameLen = MAX_NAME_LEN;

      DWORD64 symDisplacement = 0;
      if (!SymFromAddr(::GetCurrentProcess(),
                       reinterpret_cast<xsize>(pcOffset),
                       &symDisplacement,
                       symbol))
        {
        symbol->Name[0] = 0;
        symDisplacement = 0;
        }

      (void)frameOffset;
      visit->visit(i, symbol->Name, symDisplacement);
      }

    static void walk(CONTEXT *ctxIn, xsize skip, Visitor *visit)
      {
      const HANDLE hProcess = ::GetCurrentProcess();
      CONTEXT ctx = *ctxIn;

      DWORD dwMachineType;

      // set the initial frame
      STACKFRAME sf;
      memset(&sf, 0, sizeof(sf));

#ifdef _M_IX86
      sf.AddrPC.Offset       = ctx.Eip;
      sf.AddrPC.Mode         = AddrModeFlat;
      sf.AddrStack.Offset    = ctx.Esp;
      sf.AddrStack.Mode      = AddrModeFlat;
      sf.AddrFrame.Offset    = ctx.Ebp;
      sf.AddrFrame.Mode      = AddrModeFlat;

      dwMachineType = IMAGE_FILE_MACHINE_I386;
#else
#error "Need to initialize STACKFRAME on non x86 windows"
#endif

      // Iterate over all stack frames
      xsize level = 0;
      while(true)
        {
        if(!StackWalk(dwMachineType,
                      hProcess,
                      ::GetCurrentThread(),
                      &sf,
                      &ctx,
                      NULL,
                      SymFunctionTableAccess,
                      SymGetModuleBase,
                      NULL))
          {
          if (::GetLastError())
            {
            qDebug() << "Error getting call stack " << ::GetLastError();
            xAssertFail();
            }

          break;
          }

          if(level >= skip)
            {
            doVisit(visit, level - skip, (void *)sf.AddrPC.Offset, sf.AddrFrame.Offset);
            }
          ++level;
        }
      }
    };

  __try
    {
    RaiseException(0x1976, 0, 0, NULL);
    }
  __except(Utils::walk(((EXCEPTION_POINTERS *)GetExceptionInformation())->ContextRecord, skip + 2, visit), EXCEPTION_CONTINUE_EXECUTION)
    {
    // not reached
    }
#endif
#endif
  }
}

uint qHash(const Eks::CodeLocation &a)
  {
  uint hash = qHash(QPair<const char *, const char *>(a.file(), a.function()));

  return qHash(QPair<uint, xsize>(hash, a.line()));
  }

