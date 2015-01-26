#pragma once
#include "XGlobal.h"
#include "Utilities/XMacroHelpers.h"
#include "Utilities/XAssert.h"
#include <vector>
#include <iostream>

namespace Eks
{

namespace NotifierUtils
{
template <typename T, typename ...Args> void call(const T &arr, Args &&...args)
  {
  xForeach(auto &fn, arr)
    {
    try
      {
      fn.second(args...);
      }
    catch(const std::exception &e)
      {
      std::cerr << e.what();
      }
    }
  }

template <typename T, typename Fn> int listen(T &arr, int &index, Fn &&f)
  {
  int idx = index++;
  arr.emplace_back(std::make_pair(idx, std::move(f)));
  return idx;
  }

template <typename T> void remove(T &arr, int index)
  {
  arr.erase(
    std::remove_if(
      arr.begin(),
      arr.end(),
      [&](auto &x){ return x.first == index; }
    ),
    arr.end());
  }
}

#define X_DECLARE_NOTIFIER(name, Fn) \
struct name { \
  int listen(Fn &&fn) { return Eks::NotifierUtils::listen(_fns, _index, std::move(fn)); } \
  void remove(int i) { Eks::NotifierUtils::remove(_fns, i); }  \
  template <typename ...Args> void operator()(Args &&...args) { Eks::NotifierUtils::call(_fns, std::forward<Args&&>(args)...); } \
private: \
  std::vector<std::pair<int, Fn>> _fns; \
  int _index; \
  }

}
