#include "Utilities/XAssert.h"
#include "Utilities/XScopedValue.h"
#include "Containers/XUnorderedMap.h"
#include "XCore.h"
#include "Containers/XStringSimple.h"

#if X_ADVANCED_ASSERT
#if QT_VERSION >= 0x050000
# include "QtWidgets/QMessageBox"
# include "QtWidgets/QPushButton"
#else
# include "QtGui/QMessageBox"
# include "QtGui/QPushButton"
#endif

#include "QtHelper/XQApplication.h"
#include "QtCore/QThread"
#endif

#if defined(X_WIN)
# define WIN32_LEAN_AND_MEAN
# include <Windows.h>
# define snprintf _snprintf_s
#else
# include <signal.h>
# include <cassert>
#endif

namespace Eks
{

namespace detail
{

Assert::Assert(const DetailedCodeLocation &location, const char *condition, const char* message)
    : _location(location), _condition(condition), _message(message), _argCount(0)
  {
  }

bool Assert::defaultFire(const Assert &a)
  {
  UnorderedMap<CodeLocation, bool> *disabled = Core::disabledAsserts();
  if(disabled->contains(a.location()))
    {
    return false;
    }

#if X_ADVANCED_ASSERT
  if(QApplication::instance() && QThread::currentThread() != QApplication::instance()->thread())
    {
    return true;
    }
#endif

  static bool recursion = false;
  if(recursion
#if X_ADVANCED_ASSERT
     || !QCoreApplication::instance()
     || !dynamic_cast<QApplication*>(QCoreApplication::instance())
#endif
       )
    {
    // crap. just break
    return true;
    }

  Eks::ScopedValue<bool> l(recursion, true);

  std::string title;
  std::string text;

  title += "Assertion '";
  title += a.condition();
  title += "' failed";

  std::string location = a.location().toString().data();

  text += location + ":\n";

  if(strcmp(a.message(), "") != 0)
    {
    text += "> ";
    text += a.message();
    text += "\n";
    }

  text += "> " + title + "\n";

  if(a._argCount)
    {
    text += "> Arguments:\n";
    for(xsize i = 0; i < a._argCount; ++i)
      {
      const Assert::Argument &arg = a._arguments[i];

      text += "> ";
      text += arg.name;
# if X_ASSERT_VALUE_HANDLING
      text += ": ";
      text += arg.value.c_str();
      text += "\n";
# endif
      }
    }

  std::cerr << text << std::endl;

#if X_ADVANCED_ASSERT
  QMessageBox msgBox;
  msgBox.setWindowModality(Qt::ApplicationModal);
  QPushButton *breakButton = msgBox.addButton("Break Now", QMessageBox::YesRole);
  msgBox.addButton("Dont Break", QMessageBox::NoRole);
  QPushButton *neverBreakButton = msgBox.addButton("Never Break", QMessageBox::NoRole);

  msgBox.setWindowTitle(title.c_str());
  msgBox.setText(text.c_str());

  msgBox.exec();

  if (msgBox.clickedButton() == breakButton)
    {
    return true;
    }

  if (msgBox.clickedButton() == neverBreakButton)
    {
    disabled->insert(a.location(), true);
    }
#endif

  return true;
  }

Assert::FireFunction *g_currentFireFunction = 0;
Assert::FireFunction *Assert::currentFireFunction()
  {
  return g_currentFireFunction ? g_currentFireFunction : defaultFire;
  }

void Assert::setCurrentFireFunction(FireFunction *f)
  {
  g_currentFireFunction = f;
  }

#if defined(__clang__)
void interuptBreak()
  {
  __builtin_debugtrap();
  }
#elif defined(X_LINUX) || defined (X_OSX)
void interuptBreak()
  {
  raise(SIGINT);
  }
#endif

#ifdef X_ARCH_ARM
void __stdcall interuptBreak()
  {
  int *i = 0;
  *i = 1;
  }
#endif

Assert::BreakFunction *g_currentBreakFunction = 0;
Assert::BreakFunction *Assert::currentBreakFunction()
  {
  Assert::BreakFunction *f = g_currentBreakFunction;

  if(!f)
    {
#if defined(X_WIN) && !defined(X_ARCH_ARM)
    f = DebugBreak;
#else
    f = interuptBreak;
#endif
    }

  return f;
  }

void Assert::setCurrentBreakFunction(BreakFunction *f)
  {
  g_currentBreakFunction = f;
  }

}

}
