#pragma once

#include <cassert>
#include <cstddef>
#include <limits>

namespace sl3
{

  inline size_t
  as_size_t (int val)
  {
    assert (val >= 0);
    return static_cast<size_t> (val);
  }

  inline size_t
  as_size_t (long val)
  {
    assert (val >= 0);
    return static_cast<size_t> (val);
  }

  inline int
  as_int (size_t val)
  {
    assert (val < std::numeric_limits<int>::max ());
    return static_cast<int> (val);
  }

}
