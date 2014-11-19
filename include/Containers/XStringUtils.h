#pragma once

#include "Containers/XStringSimple.h"
#include <regex>
#include <QDebug>

namespace Eks
{

namespace StringUtils
{

void trimEnd(Eks::String &str, const std::regex &reg)
  {
  auto f = str.rbegin();
  auto l = str.rend();

  std::match_results<decltype(f)> cm;
  if (!std::regex_search(f, l, cm, reg))
    {
    return;
    }
    
  if (cm.position() != 0)
    {
    return;
    }

  str.resize(str.length() - cm.length(), ' ');
  }

}

}
