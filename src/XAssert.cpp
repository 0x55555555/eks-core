#include "XAssert"
#include "QMessageBox"
#include "QPushButton"
#include "QApplication"

#if defined(Q_CC_MSVC)
# define WIN32_LEAN_AND_MEAN
# include <Windows.h>
# define snprintf _snprintf_s
#elif defined(Q_CC_GNU)
# include <signal.h>
#endif

XAssert::XAssert(const XCodeLocation &location, const char *condition, const char* message)
    : _location(location), _condition(condition), _message(message)
  {
  }

QHash<XCodeLocation, bool> g_disabledAsserts;
bool defaultFire(const XAssert &a)
  {
  if(g_disabledAsserts.contains(a.location()))
    {
    return false;
    }

  static bool recursion = false;
  if(recursion || !QCoreApplication::instance() || !dynamic_cast<QApplication*>(QCoreApplication::instance()))
    {
    // crap. just break
    return true;
    }

  recursion = true;

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

  text += " in " + a.location().toString() + "\n\n";

  if(!qstrcmp(a.message(), "") == 0)
    {
    text = QString(a.message()) + ":\n\n" + text;
    }

  if(a.arguments().size())
    {
    text += "Arguments:\n";
    foreach(const XAssert::Argument &arg, a.arguments())
      {
      text + "  ";
      text += arg.name;
      text += ": " + arg.value.toString() + "\n";
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
    g_disabledAsserts.insert(a.location(), true);
    }

  return false;
  }

XAssert::FireFunction *g_currentFireFunction = 0;
XAssert::FireFunction *XAssert::currentFireFunction()
  {
  return g_currentFireFunction ? g_currentFireFunction : defaultFire;
  }

void XAssert::setCurrentFireFunction(FireFunction *f)
  {
  g_currentFireFunction = f;
  }

#ifdef Q_CC_GNU
void interuptBreak()
  {
  raise(SIGINT);
  }
#endif

XAssert::BreakFunction *g_currentBreakFunction = 0;
XAssert::BreakFunction *XAssert::currentBreakFunction()
  {
  XAssert::BreakFunction *f = g_currentBreakFunction;

  if(!f)
    {
#if defined(Q_CC_MSVC)
    f = DebugBreak;
#elif defined(Q_CC_GNU)
    f = interuptBreak;
#else
# error define a break type macro for this platform
#endif
    }

  return f;
  }

void XAssert::setCurrentBreakFunction(BreakFunction *f)
  {
  g_currentBreakFunction = f;
  }
