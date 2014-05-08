#ifndef XTEMPLATEHELPERS_H
#define XTEMPLATEHELPERS_H

#include <limits>
#include <utility>
#include <functional>

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

}

namespace std
{
template <typename A, typename B> struct hash<std::pair<A, B> >
  {
public:
  size_t operator()(const std::pair<A, B> &p) const
    {
    std::hash<A> a;
    std::hash<B> b;

    return a(p.first) ^ b(p.second);
    }

  };
}

#endif // XTEMPLATEHELPERS_H
