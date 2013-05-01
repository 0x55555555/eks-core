#include "XAssert"
#include "XUnorderedMap"
#include "XCore"
#include "XStringSimple"

#if X_ADVANCED_ASSERT
#if QT_VERSION >= 0x050000
# include "QtWidgets/QMessageBox"
# include "QtWidgets/QPushButton"
#else
# include "QtGui/QMessageBox"
# include "QtGui/QPushButton"
#endif

#include "QtHelper/XQApplication"
#include "QtCore/QThread"
#endif

#if defined(Q_CC_MSVC)
# define WIN32_LEAN_AND_MEAN
# include <Windows.h>
# define snprintf _snprintf_s
#elif defined(Q_CC_GNU)
# include <signal.h>
#endif

namespace Eks
{

namespace detail
{

Assert::Assert(const CodeLocation &location, const char *condition, const char* message)
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

  if(QThread::currentThread() != QApplication::instance()->thread())
    {
    return true;
    } 

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

  recursion = true;

#if X_ADVANCED_ASSERT
  QMessageBox msgBox;
  msgBox.setWindowModality(Qt::ApplicationModal);
  QPushButton *breakButton = msgBox.addButton("Break Now", QMessageBox::YesRole);
  msgBox.addButton("Dont Break", QMessageBox::NoRole);
  QPushButton *neverBreakButton = msgBox.addButton("Never Break", QMessageBox::NoRole);

  QString text;
  text += "Assertion ";
  text += a.condition();
  text += " failed";

  msgBox.setWindowTitle(text);

  QString location = QString::fromUtf8(a.location().toString().data());

  text += " in " + location + "\n\n";

  if(!qstrcmp(a.message(), "") == 0)
    {
    text = QString(a.message()) + ":\n\n" + text;
    }

  if(a._argCount)
    {
    text += "Arguments:\n";
    for(xsize i = 0; i < a._argCount; ++i)
      {
      const Assert::Argument &arg = a._arguments[i];

      text + "  ";
      text += arg.name;
# if X_ASSERT_VALUE_HANDLING
      text += ": " + arg.value.toString() + "\n";
# endif
      }
    }

  msgBox.setText(text);

  msgBox.exec();

  recursion = false;
  if (msgBox.clickedButton() == breakButton)
    {
    return true;
    }

  if (msgBox.clickedButton() == neverBreakButton)
    {
    disabled->insert(a.location(), true);
    }
#else
# ifdef Q_OS_WIN
  _ASSERT(0);
# else
#  error No simple assert defined
# endif
#endif

  return false;
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

#ifdef Q_CC_GNU
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
#if defined(Q_CC_MSVC) && !defined(X_ARCH_ARM)
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
