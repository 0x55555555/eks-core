#include "Containers/XStringSimple.h"

namespace Eks
{

size_t hash(const xuint8 *data, size_t length)
  {
    size_t h = 0;

    while (length--) {
        h = (h << 4) + *data++;
        h ^= (h & 0xf0000000) >> 23;
        h &= 0x0fffffff;
    }

    return h;
  }

}
