#pragma once

namespace Eks
{

namespace Algorithm
{

template <typename T, typename U> void removeIf(T &data, const U& pred)
  {
  data.erase(
    std::remove_if(
      data.begin(),
      data.end(), pred),
    data.end());
  }

}

}
