#pragma once

#include <cassert>
#include <cstddef>
#include <limits>

namespace sl3
{

  inline size_t
  as_size_t (int val)
  {
    assert (val >= 0); // LCOV_EXCL_BR_LINE
    return static_cast<size_t> (val);
  }

  inline size_t
  as_size_t (ptrdiff_t val)
  {
    assert (val >= 0); // LCOV_EXCL_BR_LINE
    return static_cast<size_t> (val);
  }

  inline int
  as_int (size_t val)
  {
    assert (val < std::numeric_limits<int>::max ()); // LCOV_EXCL_BR_LINE
    return static_cast<int> (val);
  }

  template <typename T1, typename T2>
  bool
  is_less (const T1& a, const T2& b)
  {
    using common_type = std::common_type_t<T1, T2>;
    return std::less<common_type>{}(static_cast<common_type> (a),
                                    static_cast<common_type> (b));
  }

  template <typename T1, typename T2>
  bool
  is_greater (const T1& a, const T2& b)
  {
    using common_type = std::common_type_t<T1, T2>;
    return std::greater<common_type>{}(static_cast<common_type> (a),
                                       static_cast<common_type> (b));
  }

  template <typename T1, typename T2>
  bool
  is_less_equal (const T1& a, const T2& b)
  {
    using common_type = std::common_type_t<T1, T2>;
    return std::less_equal<common_type>{}(static_cast<common_type> (a),
                                          static_cast<common_type> (b));
  }

  template <typename T1, typename T2>
  bool
  is_equal (const T1& a, const T2& b)
  {
    using common_type = std::common_type_t<T1, T2>;
    return std::equal_to<common_type>{}(static_cast<common_type> (a),
                                        static_cast<common_type> (b));
  }

}
