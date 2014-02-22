#ifndef XTEMPLATEHELPERS_H
#define XTEMPLATEHELPERS_H

#include <limits>

namespace Eks
{

/**
   A metatemplate who's Type member resolves to IF if Cond is
   true, or ELSE if Cond is false. Its Value member evaluates
   to 1 or 0, accordingly.
*/
template <bool Cond, typename IF, typename ELSE> struct IfElse
  {
  typedef IF Type;
  };

template <typename IF, typename ELSE> struct IfElse<false, IF, ELSE>
  {
  typedef ELSE Type;
  };

template <typename T> T maxFor(const T&)
  {
  return std::numeric_limits<T>::max();
  }

}

#endif // XTEMPLATEHELPERS_H
